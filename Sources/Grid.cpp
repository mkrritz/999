
#include "pch.h"

#include "game/Assert.h"
#include "game/Memory.h"
#include "game/UI.h"
#include "game/Font.h"
#include "game/Input.h"
#include "game/Puzzle.h"
#include "game/Grid.h"
#include "game/Shader.h"
#include "game/Camera.h"

using std::string;

static std::mt19937 gen(time(nullptr));

glm::vec2 GetAtlasUV(char);

static uint32_t* FillLevelInfo(Puzzle* const puzzle,
                              const std::string& level,
                              const uint32_t resolution) {
    // TODO: That's just constants per level, make the tables instead?
    uint32_t* NumberOfDigitsOnEachCell = new uint32_t[resolution * resolution];
    uint32_t quads = 0;
    uint32_t digits = 0;
    uint32_t CIX = 0;
    uint32_t CIY = 0;
    for (const char &Character : level) {
        if (Character == '\n') {
            NumberOfDigitsOnEachCell[CIY * resolution + CIX] = digits;
            CIX = 0;
            CIY++;
            digits = 0;
            continue;
        }
        if (Character == ' ') {
            NumberOfDigitsOnEachCell[CIY * resolution + CIX] = digits;
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

static string GenerateLevel(Grid* const grid,
                            const uint32_t resolution) {
    const uint32_t cells_count = resolution * resolution;
    
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
        
        if (((i + 1) % resolution == 0) && i != (cells_count-1)) {
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
    puzzle->grid->cells = (uint32_t*)HeapAllocate(heap, resolution * resolution * sizeof(uint32_t));
}

void SetText(Puzzle* const puzzle,
             const uint32_t resolution) {
    Grid* const grid = puzzle->grid;
    
    const string level = GenerateLevel(grid, resolution);
    
    uint32_t* NumberOfDigitsOnEachCell = FillLevelInfo(puzzle, level, resolution);
    
    const uint32_t VertexCount = puzzle->surface.quads_count * 4 * TEXT_VERTEX_ATTRIBUTE_COUNT;
    const uint32_t IndexCount = puzzle->surface.quads_count * 6;
    float* Vertices = new float[VertexCount];
    uint32_t* Indices = new uint32_t[IndexCount];
    uint32_t IndexOffset = 0, CharacterIndex = 0;
    
    const float CellScale = 1.0f / float(resolution);
    const float HalfCellScale = CellScale / 2.0f;
    const float VerticalPadding = 0.6f;
    
    uint32_t CIX = 0;
    uint32_t CIY = 0;
    float CharacterScale = 1.0f + 1.0f / float(NumberOfDigitsOnEachCell[CIY * resolution + CIX]);
    float LocalX = -CharacterScale * ((float(NumberOfDigitsOnEachCell[CIY * resolution + CIX]) - 1.0f) / 2.0f);
    float CellCenterX = -1.0f + CellScale;
    float CellCenterY = -1.0f + CellScale;
    
    for (size_t i = 0; level[i] != '\0'; i++) {
        const char Character = level[i];
        if (Character == '\n') {
            CIY += 1;
            CIX = 0;
            CharacterScale = 1.0f + 1.0f / float(NumberOfDigitsOnEachCell[CIY * resolution + CIX]);
            LocalX = -CharacterScale * ((float(NumberOfDigitsOnEachCell[CIY * resolution + CIX]) - 1.0f) / 2.0f);
            
            CellCenterX = -1.0f + CellScale;
            CellCenterY += 2.0f * CellScale;
            continue;
        }
        if (Character == ' ') {
            CIX += 1;
            CharacterScale = 1.0f + 1.0f / float(NumberOfDigitsOnEachCell[CIY * resolution + CIX]);
            LocalX = -CharacterScale * ((float(NumberOfDigitsOnEachCell[CIY * resolution + CIX]) - 1.0f) / 2.0f);
            
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
        Vertices[CellVerticesIndex + (0 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 0] = CellCenterX + (LocalX * VerticalPadding - CharacterScale) * HalfCellScale;
        Vertices[CellVerticesIndex + (0 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 1] = CellCenterY - CharacterScale * HalfCellScale;
        Vertices[CellVerticesIndex + (0 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 2] = 0.0f;
        Vertices[CellVerticesIndex + (0 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 3] = UV.x;
        Vertices[CellVerticesIndex + (0 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 4] = UV.y;
        
        // TL
        Vertices[CellVerticesIndex + (1 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 0] = CellCenterX + (LocalX * VerticalPadding - CharacterScale) * HalfCellScale;
        Vertices[CellVerticesIndex + (1 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 1] = CellCenterY + CharacterScale * HalfCellScale;
        Vertices[CellVerticesIndex + (1 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 2] = 0.0f;
        Vertices[CellVerticesIndex + (1 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 3] = UV.x;
        Vertices[CellVerticesIndex + (1 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 4] = UV.y + 0.125f;
        
        // BR
        Vertices[CellVerticesIndex + (2 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 0] = CellCenterX + (LocalX * VerticalPadding + CharacterScale) * HalfCellScale;
        Vertices[CellVerticesIndex + (2 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 1] = CellCenterY - CharacterScale * HalfCellScale;
        Vertices[CellVerticesIndex + (2 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 2] = 0.0f;
        Vertices[CellVerticesIndex + (2 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 3] = UV.x + 0.125f;
        Vertices[CellVerticesIndex + (2 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 4] = UV.y;
        
        // TR
        Vertices[CellVerticesIndex + (3 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 0] = CellCenterX + (LocalX * VerticalPadding + CharacterScale) * HalfCellScale;
        Vertices[CellVerticesIndex + (3 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 1] = CellCenterY + CharacterScale * HalfCellScale;
        Vertices[CellVerticesIndex + (3 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 2] = 0.0f;
        Vertices[CellVerticesIndex + (3 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 3] = UV.x + 0.125f;
        Vertices[CellVerticesIndex + (3 * TEXT_VERTEX_ATTRIBUTE_COUNT) + 4] = UV.y + 0.125f;
        
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

void RenderGame(const Shader* const shaders[],
                const Camera* const camera,
                const Font* const fonts[],
                const UI* const ui,
                const Puzzle* const puzzle,
                GLuint scene_ubo_handle) {
    const Shader* shader;
    const Font* const font = fonts[(uint32_t)FontType::English];
    
    glBindSampler(0, camera->sampler_clamp_linear);
    glBindTextureUnit(0, font->texture);
    
    glBindVertexArray(empty_vao);
    
    shader = shaders[(uint32_t)Shaders::Grid];
    
    glUseProgram(shader->ID);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, scene_ubo_handle);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, ui->ubo_handle[static_cast<uint32_t>(UIIndex::Grid)]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glEnable(GL_BLEND);
    shader = shaders[(uint32_t)Shaders::SDF];
    glUseProgram(shader->ID);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, scene_ubo_handle);
    glBindVertexArray(puzzle->surface.vao);
    glDrawElements(GL_TRIANGLES, 6 * puzzle->surface.quads_count, GL_UNSIGNED_INT, 0);
    glDisable(GL_BLEND);
    
    glBindVertexArray(0);
    
    glBindSampler(0, 0);
}