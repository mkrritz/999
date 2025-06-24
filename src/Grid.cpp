
#include "pch.h"

#include "game/Assert.h"
#include "game/Memory.h"
#include "game/Grid.h"
#include "game/Puzzle.h"
#include "game/Font.h"

using std::string;

static std::mt19937 gen(time(nullptr));

glm::vec2 GetAtlasUV(char);

static uint32_t* FillLevelInfo(Puzzle* const puzzle,
                              const std::string& level) {
    // TODO: That's just constants per level, make the tables instead?
    const uint32_t N = puzzle->grid->N;
    uint32_t* NumberOfDigitsOnEachCell = new uint32_t[N * N];
    uint32_t quads = 0;
    uint32_t digits = 0;
    uint32_t CIX = 0;
    uint32_t CIY = 0;
    for (const char &Character : level) {
        if (Character == '\n') {
            NumberOfDigitsOnEachCell[CIY * N + CIX] = digits;
            CIX = 0;
            CIY++;
            digits = 0;
            continue;
        }
        if (Character == ' ') {
            NumberOfDigitsOnEachCell[CIY * N + CIX] = digits;
            CIX++;
            digits = 0;
            continue;
        }
        quads++;
        digits++;
    }
    
    puzzle->surface.quads_count = quads;
    return NumberOfDigitsOnEachCell;
}

static string GenerateLevel(Grid* const grid) {
    uint32_t cells_count = grid->N * grid->N;
    
    std::vector<uint32_t> numbers(cells_count);
    for (uint32_t i = 0; i < cells_count; ++i) {
        numbers[i] = i + 1;
    }
    std::shuffle(numbers.begin(), numbers.end(), gen);
    for (uint32_t i = 0; i < cells_count; ++i) {
        grid->cells[i] = numbers[i];
    }
    
    string level;
    string digits;
    for (uint32_t i = 0; i < cells_count; ++i) {
        const string number = std::to_string(numbers[i]);
        const string digitCount = std::to_string(number.length());
        
        //if (numbers[i] % 2 == 0) {
            level  += number;
            digits += digitCount;
        //}
        
        if (((i + 1) % grid->N == 0) && i != (cells_count-1)) {
            level += '\n';
            digits += '\n';
        } else {
            level += ' ';
            digits += ' ';
        }
    }
    //printf("\n--\n%s\n--\n", grid.c_str());
    //printf("\n--\n%s\n--\n", digits.c_str());
    return level;
}

void GridInit(Heap* const heap,
              Puzzle* const puzzle,
              const uint32_t resolution) {
    HeapReset(heap);
    puzzle->grid = (Grid*)HeapAllocate(heap, sizeof(Grid));
    puzzle->grid->N = resolution;
    puzzle->grid->cells = (uint32_t*)HeapAllocate(heap, puzzle->grid->N * puzzle->grid->N * sizeof(uint32_t));
}

void SetText(Puzzle* const puzzle) {
    Grid* const grid = puzzle->grid;
    
    const string level = GenerateLevel(grid);
    
    uint32_t* NumberOfDigitsOnEachCell = FillLevelInfo(puzzle, level);
    
    const uint32_t VertexCount = puzzle->surface.quads_count * 4 * TEXT_VERTEX_ATTRIBUTE_COUNT;
    const uint32_t IndexCount = puzzle->surface.quads_count * 6;
    float* Vertices = new float[VertexCount];
    uint32_t* Indices = new uint32_t[IndexCount];
    uint32_t IndexOffset = 0, CharacterIndex = 0;
    
    const float CellScale = 1.0f / float(grid->N);
    const float HalfCellScale = CellScale / 2.0f;
    const float VerticalPadding = 0.6f;
    
    // TODO put them in quad bounds too. what?
    
    uint32_t CIX = 0;
    uint32_t CIY = 0;
    float CharacterScale = 1.0f + 1.0f / float(NumberOfDigitsOnEachCell[CIY * grid->N + CIX]);
    float LocalX = -CharacterScale * ((float(NumberOfDigitsOnEachCell[CIY * grid->N + CIX]) - 1.0f) / 2.0f);
    float CellCenterX = -1.0f + CellScale;
    float CellCenterY = -1.0f + CellScale;
    
    for (size_t i = 0; level[i] != '\0'; i++) {
        const char Character = level[i];
        if (Character == '\n') {
            CIY += 1;
            CIX = 0;
            CharacterScale = 1.0f + 1.0f / float(NumberOfDigitsOnEachCell[CIY * grid->N + CIX]);
            LocalX = -CharacterScale * ((float(NumberOfDigitsOnEachCell[CIY * grid->N + CIX]) - 1.0f) / 2.0f);
            
            CellCenterX = -1.0f + CellScale;
            CellCenterY += 2.0f * CellScale;
            continue;
        }
        if (Character == ' ') {
            CIX += 1;
            CharacterScale = 1.0f + 1.0f / float(NumberOfDigitsOnEachCell[CIY * grid->N + CIX]);
            LocalX = -CharacterScale * ((float(NumberOfDigitsOnEachCell[CIY * grid->N + CIX]) - 1.0f) / 2.0f);
            
            CellCenterX += 2.0f * CellScale;
            continue;
        }
        
        // Get the BL point and offset the rest, which is by a constant,
        // unless font image changes and each glyph is not 128x128 any more
        glm::vec2 UV = GetAtlasUV(Character);
        
        uint32_t CellVerticesIndex = CharacterIndex * 4 * TEXT_VERTEX_ATTRIBUTE_COUNT;
        uint32_t CellIndicesIndex  = CharacterIndex * 6;
        CharacterIndex += 1;
        
        // BL
        Vertices[ CellVerticesIndex + (0 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 0 ] = CellCenterX + (LocalX * VerticalPadding - CharacterScale) * HalfCellScale;
        Vertices[ CellVerticesIndex + (0 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 1 ] = CellCenterY - CharacterScale * HalfCellScale;
        Vertices[ CellVerticesIndex + (0 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 2 ] = 0.0f;
        Vertices[ CellVerticesIndex + (0 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 3 ] = UV.x;
        Vertices[ CellVerticesIndex + (0 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 4 ] = UV.y;
        
        // TL
        Vertices[ CellVerticesIndex + (1 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 0 ] = CellCenterX + (LocalX * VerticalPadding - CharacterScale) * HalfCellScale;
        Vertices[ CellVerticesIndex + (1 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 1 ] = CellCenterY + CharacterScale * HalfCellScale;
        Vertices[ CellVerticesIndex + (1 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 2 ] = 0.0f;
        Vertices[ CellVerticesIndex + (1 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 3 ] = UV.x;
        Vertices[ CellVerticesIndex + (1 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 4 ] = UV.y + 0.125f;

        // BR
        Vertices[ CellVerticesIndex + (2 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 0 ] = CellCenterX + (LocalX * VerticalPadding + CharacterScale) * HalfCellScale;
        Vertices[ CellVerticesIndex + (2 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 1 ] = CellCenterY - CharacterScale * HalfCellScale;
        Vertices[ CellVerticesIndex + (2 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 2 ] = 0.0f;
        Vertices[ CellVerticesIndex + (2 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 3 ] = UV.x + 0.125f;
        Vertices[ CellVerticesIndex + (2 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 4 ] = UV.y;

        // TR
        Vertices[ CellVerticesIndex + (3 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 0 ] = CellCenterX + (LocalX * VerticalPadding + CharacterScale) * HalfCellScale;
        Vertices[ CellVerticesIndex + (3 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 1 ] = CellCenterY + CharacterScale * HalfCellScale;
        Vertices[ CellVerticesIndex + (3 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 2 ] = 0.0f;
        Vertices[ CellVerticesIndex + (3 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 3 ] = UV.x + 0.125f;
        Vertices[ CellVerticesIndex + (3 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 4 ] = UV.y + 0.125f;
        
        Indices[CellIndicesIndex + 0] = 1 + IndexOffset;
        Indices[CellIndicesIndex + 1] = 0 + IndexOffset;
        Indices[CellIndicesIndex + 2] = 2 + IndexOffset;
        Indices[CellIndicesIndex + 3] = 1 + IndexOffset;
        Indices[CellIndicesIndex + 4] = 2 + IndexOffset;
        Indices[CellIndicesIndex + 5] = 3 + IndexOffset;
        IndexOffset += 4;
        
        LocalX += CharacterScale;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, puzzle->surface.vbo);
    glBufferData(GL_ARRAY_BUFFER, VertexCount * sizeof(float), &Vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, puzzle->surface.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexCount * sizeof(uint32_t), &Indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    delete[] Vertices;
    delete[] Indices;
    free(NumberOfDigitsOnEachCell);
}
