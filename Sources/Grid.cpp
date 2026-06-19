
#include "pch.h"

#include "game/Assert.h"
#include "game/Memory.h"
#include "game/UI.h"
#include "game/Font.h"        // Do Text.cpp ?
#include "game/Input.h"
#include "game/Puzzle.h"
#include "game/Shader.h"
#include "game/Camera.h"

using std::string;

static std::mt19937 gen(time(nullptr)); // Random.h

glm::vec2 GetAtlasUV(char);

# if 1 // Fix all that. Remove algorithm, replace new with heap. Data tables insteads.

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

void GridInit(Heap* const heap,
              Puzzle* const puzzle,
              const uint32_t resolution) {
    HeapReset(heap);
    puzzle->cells = (uint32_t*)HeapAllocate(heap, resolution * resolution * sizeof(uint32_t));
    glCreateBuffers(1, &puzzle->vbo);
}

struct CharQuad {
    glm::vec4 rect;
    glm::vec4 uv_rect;
};

void SetText(Puzzle* const puzzle,
             const uint32_t resolution) {
    const string level = GenerateLevel(puzzle, resolution);
    uint32_t* NumberOfDigitsOnEachCell = FillLevelInfo(puzzle, level, resolution);
    
    CharQuad* Quads = new CharQuad[puzzle->quads_count];
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
        
        CharQuad& quad = Quads[CharacterIndex];
        CharacterIndex += 1;
        
        float X = CellCenterX + (LocalX * VerticalPadding - CharacterScale) * HalfCellScale;
        float Y = CellCenterY - CharacterScale * HalfCellScale;
        float W = 2.0f * CharacterScale * HalfCellScale;
        float H = 2.0f * CharacterScale * HalfCellScale;
        quad.rect = glm::vec4(X, Y, W, H);
        
        glm::vec2 UV = GetAtlasUV(Character);
        quad.uv_rect = glm::vec4(UV.x, UV.y, 0.125f, 0.125f);
        
        LocalX += CharacterScale;
    }
    
    // Do it with mapped vbo and direct write to field (or memcopy) instead of this gl call
    glNamedBufferData(puzzle->vbo, puzzle->quads_count * sizeof(CharQuad), &Quads[0], GL_STATIC_DRAW);
    
    delete[] Quads;
    free(NumberOfDigitsOnEachCell);
}

void RenderGame(const Shader* const shaders[],
                const Camera* const camera,
                const Font* const fonts[],
                const UI* const ui,
                const Puzzle* const puzzle,
                GLuint scene_ubo_handle) {
    const Font* const font = fonts[(uint32_t)FontType::English];
    
    glBindSampler(0, camera->sampler_clamp_linear);
    glBindTextureUnit(0, font->texture);
    
    glBindVertexArray(empty_vao);
    
    glUseProgram(shaders[(uint32_t)Shaders::Grid]->ID);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, scene_ubo_handle);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, ui->ubo_handle[static_cast<uint32_t>(UIIndex::Fullscreen)]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glEnable(GL_BLEND);
    
    glUseProgram(shaders[(uint32_t)Shaders::SDF]->ID);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, scene_ubo_handle);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, puzzle->vbo);
    glBindVertexArray(empty_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6 * puzzle->quads_count);
    
    glDisable(GL_BLEND);
    
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, 0);
    glBindVertexArray(0);
    
    glBindSampler(0, 0);
}