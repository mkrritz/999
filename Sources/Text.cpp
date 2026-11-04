
#include "pch.h"

#include "game/Assert.h"
#include "game/Memory.h"
#include "game/UI.h"
#include "game/Font.h"
#include "game/Text.h"
#include "game/Puzzle.h"
#include "game/Random.h"
#include "game/Shader.h"

using std::string;

glm::vec2 GetAtlasUV(char);

std::string GenerateRandomLevel(Puzzle* const puzzle,
                                const uint32_t resolution) {
    const uint32_t cellCount = resolution * resolution;
    
    for (uint32_t i = 0; i < cellCount; ++i) {
        puzzle->cells[i] = i + 1;
    }
    std::shuffle(puzzle->cells, puzzle->cells + cellCount, gen);
    
    std::string level;
    for (uint32_t y = 0; y < resolution; ++y) {
        for (uint32_t x = 0; x < resolution; ++x) {
            level += std::to_string(puzzle->cells[y * resolution + x]);
            if (x + 1 < resolution) {
                level += ' ';
            }
        }
        if (y + 1 < resolution) {
            level += '\n';
        }
    }
    
    return level;
}

std::string GenerateLevel(Puzzle* const puzzle,
                          const uint32_t resolution) {
    return GenerateRandomLevel(puzzle, resolution);
}

uint32_t SetText(const std::string& text,
                 CharacterQuad* const buffer,
                 const glm::vec2 anchor,
                 const float scale,
                 const float padding,
                 const bool center_text,
                 const glm::vec2 offset) {
    uint32_t written = 0;
    float local_x = 0.0f;
    if (center_text) {
        local_x = -((float(text.length()) - 1.0f) / 2.0f);
    }
    
    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        if (c == ' ') {
            local_x += 1.0f;
            continue;
        }
        
        glm::vec2 uv = GetAtlasUV(c);
        CharacterQuad& quad = buffer[written++];
        
        float x = (local_x * padding + offset.x) * scale;
        float y = offset.y * scale;
        
        if (c >= 'A' && c <= 'Z') {
            y += 0.1f * scale;         // This is not the way, is it?
        }
        
        quad.rect = glm::vec4(x, y, scale, scale);
        quad.uv = uv;
        quad.anchor = anchor;
        
        local_x += 1.0f;
    }
    return written;
}

void SetGridText(Puzzle* const puzzle,
                 CharacterQuad* const buffer,
                 const uint32_t resolution) {
    const std::string Level = GenerateRandomLevel(puzzle, resolution);
    uint32_t ValidChacater = 0;
    
    const float CellScale = 1.0f / float(resolution);
    const float VerticalPadding = 0.6f;
    uint32_t Width = 0;
    uint32_t Height = 0;
    float CellCenterX = -1.0f + CellScale;
    float CellCenterY = -1.0f + CellScale;
    
    std::string current_word = "";
    
    for (size_t CharacterIndex = 0; CharacterIndex <= Level.length(); ++CharacterIndex) {
        char Character = (CharacterIndex < Level.length()) ? Level[CharacterIndex] : '\0';
        if (Character == '\r') {
            continue;
        }
        
        if (Character == ' ' || Character == '\n' || Character == '\0') {
            if (!current_word.empty()) {
                uint32_t Digits = CountDigits(puzzle->cells[Height * resolution + Width]);
                float CharacterScale = 1.0f + 1.0f / float(Digits);
                float scale = CharacterScale * CellScale;
                glm::vec2 anchor = glm::vec2(CellCenterX, CellCenterY);
                glm::vec2 offset = glm::vec2(-0.5f, -0.5f);
                
                ValidChacater += SetText(current_word,
                                         buffer + ValidChacater,
                                         anchor,
                                         scale,
                                         VerticalPadding / 2.0f,
                                         true,
                                         offset);
                current_word.clear();
            }
            
            if (Character == '\n') {
                Height += 1;
                Width = 0;
                CellCenterX = -1.0f + CellScale;
                CellCenterY += 2.0f * CellScale;
            } else if (Character == ' ') {
                Width += 1;
                CellCenterX += 2.0f * CellScale;
            }
            continue;
        }
        
        current_word += Character;
    }
    
    puzzle->quads_count = ValidChacater;
}
