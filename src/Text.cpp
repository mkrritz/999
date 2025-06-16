
#include "pch.h"

#include "game/Assert.h"
#include "game/Memory.h"
#include "game/Text.h"

#include "stb_image/stb_image.h"

using std::string;
using std::filesystem::path;

static const path FONT_DIRECTORY = "Fonts/";

static const glm::vec2 FontUV[64] = {
    glm::vec2(0    , 0.875), // 0
    glm::vec2(0.125, 0.875), // 1
    glm::vec2(0.25 , 0.875), // 2
    glm::vec2(0.375, 0.875), // 3
    glm::vec2(0.5  , 0.875), // 4
    glm::vec2(0.625, 0.875), // 5
    glm::vec2(0.75 , 0.875), // 6
    glm::vec2(0.875, 0.875), // 7
    glm::vec2(0    , 0.75 ), // 8
    glm::vec2(0.125, 0.75 ), // 9
    glm::vec2(0.25 , 0.75 ), // A
    glm::vec2(0.375, 0.75 ), // B
    glm::vec2(0.5  , 0.75 ), // C
    glm::vec2(0.625, 0.75 ), // D
    glm::vec2(0.75 , 0.75 ), // E
    glm::vec2(0.875, 0.75 ), // F
    glm::vec2(0    , 0.625), // G
    glm::vec2(0.125, 0.625), // H
    glm::vec2(0.25 , 0.625), // I
    glm::vec2(0.375, 0.625), // J
    glm::vec2(0.5  , 0.625), // K
    glm::vec2(0.625, 0.625), // L
    glm::vec2(0.75 , 0.625), // M
    glm::vec2(0.875, 0.625), // N
    glm::vec2(0    , 0.5  ), // O
    glm::vec2(0.125, 0.5  ), // P
    glm::vec2(0.25 , 0.5  ), // Q
    glm::vec2(0.375, 0.5  ), // R
    glm::vec2(0.5  , 0.5  ), // S
    glm::vec2(0.625, 0.5  ), // T
    glm::vec2(0.75 , 0.5  ), // U
    glm::vec2(0.875, 0.5  ), // V
    glm::vec2(0    , 0.375), // W
    glm::vec2(0.125, 0.375), // X
    glm::vec2(0.25 , 0.375), // Y
    glm::vec2(0.375, 0.375), // Z
    glm::vec2(0.5  , 0.375), // a
    glm::vec2(0.625, 0.375), // b
    glm::vec2(0.75 , 0.375), // c
    glm::vec2(0.875, 0.375), // d
    glm::vec2(0    , 0.25 ), // e
    glm::vec2(0.125, 0.25 ), // f
    glm::vec2(0.25 , 0.25 ), // g
    glm::vec2(0.375, 0.25 ), // h
    glm::vec2(0.5  , 0.25 ), // i
    glm::vec2(0.625, 0.25 ), // j
    glm::vec2(0.75 , 0.25 ), // k
    glm::vec2(0.875, 0.25 ), // l
    glm::vec2(0    , 0.125), // m
    glm::vec2(0.125, 0.125), // n
    glm::vec2(0.25 , 0.125), // o
    glm::vec2(0.375, 0.125), // p
    glm::vec2(0.5  , 0.125), // q
    glm::vec2(0.625, 0.125), // r
    glm::vec2(0.75 , 0.125), // s
    glm::vec2(0.875, 0.125), // t
    glm::vec2(0    ,     0), // u
    glm::vec2(0.125,     0), // v
    glm::vec2(0.25 ,     0), // w
    glm::vec2(0.375,     0), // x
    glm::vec2(0.5  ,     0), // y
    glm::vec2(0.625,     0), // z
    glm::vec2(0.75 ,     0), // Space
    glm::vec2(0.875,     0), // w/e 
};

static inline uint8_t char_to_normalized_ascii(char c) {
    if (c >= '0' && c <= '9') c -= '0';
    else if (c >= 'A' && c <= 'Z') c -= ('A' - 10);
    else if (c >= 'a' && c <= 'z') c -= ('a' - 36);
    else if (c == ' ') c = 62;
    else {
        printf("Character '%c' not supported by font.\n", c);
        c = 62;
    }
    return (uint8_t) c;
}

static inline glm::vec2 GetAtlasUV(char c) {
    uint8_t index = char_to_normalized_ascii(c);
    return FontUV[index];
}

static uint32_t* FillTextInfo(TextField& level,
                         const std::string& text,
                         const uint32_t N) {
    uint32_t* digit_count = new uint32_t[N * N];
    
    level.text = text;
    level.length = 0;
    
    uint32_t digits = 0;
    uint32_t cell_x = 0;
    uint32_t cell_y = 0;
    for (const char &Character : level.text) {
        if (Character == '\n') {
            digit_count[cell_y * N + cell_x] = digits;
            cell_x = 0;
            cell_y++;
            digits = 0;
            continue;
        }
        if (Character == ' ') {
            digit_count[cell_y * N + cell_x] = digits;
            cell_x++;
            digits = 0;
            continue;
        }
        level.length++;
        digits++;
    }
    
    return digit_count;
}

static const string FONTS[(uint32_t)FontType::Count] = {
    // Keep these in the same order as the 'enum struct FontType'
    "font_1.png",
    //"font_1.png",
    //"font_1.png",
};

static void LoadFont(const std::string& name) {
    path font_path = FONT_DIRECTORY / name;
    int W, H, C;
    unsigned char *img = stbi_load(font_path.string().c_str(), &W, &H, &C, 1);
    ASSERT (!img, "Font image missing.");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, W, H, 0, GL_RED, GL_UNSIGNED_BYTE, img);
    stbi_image_free(img);
}


using std::string;
static std::random_device rd;
static std::mt19937 gen(time(nullptr));

static string GenerateGrid(uint32_t N) {
    uint32_t GridSize = N * N;
    
    std::vector<uint32_t> numbers(GridSize);
    for (uint32_t i = 0; i < GridSize; ++i) {
        numbers[i] = i + 1;
    }
    std::shuffle(numbers.begin(), numbers.end(), gen);
    
    string grid;
    string digits;
    for (uint32_t i = 0; i < GridSize; ++i) {
        const string number = std::to_string(numbers[i]);
        const string digitCount = std::to_string(number.length());
        
        //if (numbers[i] % 2 == 0) {
            grid  += number;
            digits += digitCount;
        //}
        
        if (((i + 1) % N == 0) && i != (GridSize-1)) {
            grid += '\n';
            digits += '\n';
        } else {
            grid += ' ';
            digits += ' ';
        }
    }
    //printf("\n--\n%s\n--\n", grid.c_str());
    //printf("\n--\n%s\n--\n", digits.c_str());
    return grid;
}

void SetText(TextField& level, float resolution) {
    
    const uint32_t N = uint32_t(glm::abs(glm::floor(resolution)));
    
    const string grid = GenerateGrid(N);
    
    uint32_t* DigitsOnEachCell = FillTextInfo(level, grid, N);
    
    const uint32_t VertexCount = level.length * 4 * TEXT_VERTEX_ATTRIBUTE_COUNT;
    const uint32_t IndexCount = level.length * 6;
    
    float* Vertices = new float[VertexCount];
    uint32_t* Indices = new uint32_t[IndexCount];
    uint32_t IndexOffset = 0, CharacterIndex = 0;
    
    const float CellScale = 1.0f / float(N);
    const float HalfCellScale = CellScale / 2.0f;
    
    const float VerticalPadding = 0.6f;
    
    uint32_t CIX = 0;
    uint32_t CIY = 0;
    float CharacterScale = 1.0f + 1.0f / float(DigitsOnEachCell[CIY * N + CIX]);
    float LocalX = -CharacterScale * ((float(DigitsOnEachCell[CIY * N + CIX]) - 1.0f) / 2.0f);
    float CellCenterX = -1.0f + CellScale;
    float CellCenterY = -1.0f + CellScale;
    
    for (size_t i = 0; level.text[i] != '\0'; i++) {
        const char Character = level.text[i];
        if (Character == '\n') {
            CIY += 1;
            CIX = 0;
            CharacterScale = 1.0f + 1.0f / float(DigitsOnEachCell[CIY * N + CIX]);
            LocalX = -CharacterScale * ((float(DigitsOnEachCell[CIY * N + CIX]) - 1.0f) / 2.0f);
            
            CellCenterX = -1.0f + CellScale;
            CellCenterY += 2.0f * CellScale;
            continue;
        }
        if (Character == ' ') {
            CIX += 1;
            CharacterScale = 1.0f + 1.0f / float(DigitsOnEachCell[CIY * N + CIX]);
            LocalX = -CharacterScale * ((float(DigitsOnEachCell[CIY * N + CIX]) - 1.0f) / 2.0f);
            
            CellCenterX += 2.0f * CellScale;
            continue;
        }
        
        // Get the BL point and offset the rest, which is by a constant,
        // unless font image changes and each glyph is not 128x128 any more
        glm::vec2 UV = GetAtlasUV(Character);
        
        uint32_t CellVerticesIndex = CharacterIndex * (4 * TEXT_VERTEX_ATTRIBUTE_COUNT);
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
    
    glBindBuffer(GL_ARRAY_BUFFER, level.vbo);
    glBufferData(GL_ARRAY_BUFFER, VertexCount * sizeof(float), &Vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, level.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexCount * sizeof(uint32_t), &Indices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    delete[] Vertices;
    delete[] Indices;
    free(DigitsOnEachCell);
}

void FontsInit(Heap* const heap, Font* fonts[]) {
    stbi_set_flip_vertically_on_load(true);
    for (uint32_t index = 0; index < (uint32_t)FontType::Count; index++) {
        fonts[index] = (Font*)HeapAllocate(heap, sizeof(Font));
        glGenTextures(1, &fonts[index]->texture);
        glBindTexture(GL_TEXTURE_2D, fonts[index]->texture);
        LoadFont(FONTS[index]);
    }
    stbi_set_flip_vertically_on_load(false);
    glBindTexture(GL_TEXTURE_2D, 0);
}
