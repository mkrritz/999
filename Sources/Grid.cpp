
#include "pch.h"

#include "game/Assert.h"
#include "game/Memory.h"
#include "game/UI.h"
#include "game/Font.h"        // Do Text.cpp ?
#include "game/Puzzle.h"
#include "game/Random.h"
#include "game/Shader.h"
#include "game/Camera.h"

using std::string;

glm::vec2 GetAtlasUV(char);

# if 1
static uint32_t* FillLevelInfo(Puzzle* const puzzle,
                               const std::string& level,
                               const uint32_t resolution) {
    uint32_t* digitsPerCell = new uint32_t[resolution * resolution];
    
    uint32_t cellX = 0;
    uint32_t cellY = 0;
    uint32_t digitCount = 0;
    uint32_t quadCount = 0;
    
    for (char c : level) {
        if (c == ' ' || c == '\n') {
            digitsPerCell[cellY * resolution + cellX] = digitCount;
            digitCount = 0;
            cellX++;
            if (c == '\n') {
                cellX = 0;
                cellY++;
            }
            continue;
        }
        digitCount++;
        quadCount++;
    }
    
    digitsPerCell[cellY * resolution + cellX] = digitCount;
    
    puzzle->quads_count = quadCount;
    
    return digitsPerCell;
}

static std::string GenerateLevel(Puzzle* const puzzle,
                                 const uint32_t resolution) {
    const uint32_t cellCount = resolution * resolution;
    
    std::vector<uint32_t> numbers(cellCount);
    std::iota(numbers.begin(), numbers.end(), 1);
    std::shuffle(numbers.begin(), numbers.end(), gen);
    
    for (uint32_t i = 0; i < cellCount; ++i) {
        puzzle->cells[i] = numbers[i];
    }
    
    std::string level;
    
    for (uint32_t i = 0; i < cellCount; ++i) {
        level += std::to_string(numbers[i]);
        
        if (i == cellCount - 1) {
            break;
        }
        
        if ((i + 1) % resolution == 0) {
            level += '\n';
        } else {
            level += ' ';
        }
    }
    
    return level;
}

#endif

struct CharacterQuad {
    glm::vec4 rect;
    glm::vec4 uv_rect;
    glm::vec4 ndc;
};


/*
There are two competing coordinate spaces in the code:
    Cell/layout space ( CPU code)
        CellCenterX/Y, CellScale, etc. ---> produces per-character absolute NDC rectangles
    Per-number local layout (your intent)
        LocalX, CharacterScale ---> produces offsets within a multi-digit number

The SDF shader in RenderGame renders each number onto each cell of the rendered grid.

The Level shader is supposed to render a multi
digit number in some screen location
dictated by ui->ubo_handle[(uint32_t)Quads::Level].

For multi-digit numbers:
    CPU:
        rect.xy = LOCAL offset within number
        rect.zw = glyph size
    GPU:
        base position (NDCX, NDCY) + local offset + UV offset

If we remove CellCenterX/Y from baked position then SDF rendering of all the numbers breaks
All numbers are overlapping because one subtle but important consequence
we now implicitly assume all quads in a draw call share the same (NDCX, NDCY)

*/

void SetText(Puzzle* const puzzle,
             const uint32_t resolution) {
    const string level = GenerateLevel(puzzle, resolution);
    uint32_t* NumberOfDigitsOnEachCell = FillLevelInfo(puzzle, level, resolution);
    
    CharacterQuad* CQ = new CharacterQuad[puzzle->quads_count];
    uint32_t CharacterIndex = 0;
    
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
        
        CharacterQuad& quad = CQ[CharacterIndex];
        CharacterIndex += 1;
        
        float X = (LocalX * VerticalPadding - CharacterScale) * HalfCellScale;
        float Y = -CharacterScale * HalfCellScale;
        float W = 2.0f * CharacterScale * HalfCellScale;
        float H = 2.0f * CharacterScale * HalfCellScale;
        quad.rect = glm::vec4(X, Y, W, H);
        
        glm::vec2 UV = GetAtlasUV(Character);
        quad.uv_rect = glm::vec4(UV.x, UV.y, 0.125f, 0.125f);
        quad.ndc = glm::vec4(CellCenterX, CellCenterY, 0.0f, 0.0f);
        
        LocalX += CharacterScale;
    }
    
    // Do it with mapped vbo and direct write to field (or memcopy) instead of this gl call
    glNamedBufferData(puzzle->vbo, puzzle->quads_count * sizeof(CharacterQuad), &CQ[0], GL_STATIC_DRAW);
    
    delete[] CQ;
    free(NumberOfDigitsOnEachCell);
}

void SetLevelText(UI* const ui, const uint32_t level_number) {
    const string text = std::to_string(level_number);
    ui->level_quads_count = (uint32_t)text.length();
    
    CharacterQuad* CQ = new CharacterQuad[ui->level_quads_count];
    
    float CharacterScale = 1.0f;
    float LocalX = -CharacterScale * ((float(ui->level_quads_count) - 1.0f) / 2.0f);
    const float VerticalPadding = 0.3f;
    
    glm::vec4 ui_rect = ui->ubo[(uint32_t)Quads::Level]->rect;
    float scale = ui_rect.w; // height acts as scale
    
    for(size_t i = 0; i < text.length(); i++) {
        char Character = text[i];
        
        float W = scale;
        float H = scale;
        float X = (LocalX * VerticalPadding) * scale;
        float Y = 0.0f;
        
        CQ[i].rect = glm::vec4(X, Y, W, H);
        
        glm::vec2 UV = GetAtlasUV(Character);
        CQ[i].uv_rect = glm::vec4(UV.x, UV.y, 0.125f, 0.125f);
        CQ[i].ndc = glm::vec4(0.0f);
        
        LocalX += CharacterScale;
    }
    
    glNamedBufferData(ui->level_vbo, ui->level_quads_count * sizeof(CharacterQuad), &CQ[0], GL_STATIC_DRAW);
    delete[] CQ;
}

void RenderGame(const Shader* const shaders[],
                const Camera* const camera,
                const Font* const fonts[],
                const UI* const ui,
                const Puzzle* const puzzle,
                GLuint scene_ubo_handle) {
    const Font* const font = fonts[(uint32_t)FontType::English];
    
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, scene_ubo_handle);
    
    glUseProgram(shaders[(uint32_t)Shaders::Grid]->ID);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, ui->ubo_handle[(uint32_t)Quads::Fullscreen]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, 0);
    
    glEnable(GL_BLEND);
    
    glBindSampler(0, camera->sampler_clamp_linear);
    glBindTextureUnit(0, font->texture);
    
    glUseProgram(shaders[(uint32_t)Shaders::SDF]->ID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, puzzle->vbo);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, puzzle->quads_count);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, 0);
    
    glBindSampler(0, 0);
    glBindTextureUnit(0, 0);
    
    glDisable(GL_BLEND);
}
