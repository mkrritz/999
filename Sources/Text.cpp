

#include "pch.h"

#include "game/Assert.h"
#include "game/Memory.h"
#include "game/UI.h"
#include "game/Font.h"
#include "game/Text.h"
#include "game/Puzzle.h" // Remove this and operate on buffers via pointer parameter?
#include "game/Random.h"
#include "game/Shader.h"


using std::string;

glm::vec2 GetAtlasUV(char);

# if 1 // Are these up for consteval?
       // This is CPU side only. Called by SetText only.
       // Move to main and process them tere and then SetText will accept
       // pointer parameters, or not. These can all be files, instead of
       // generated at runtime. Then another script is responsible for generating
       // the levels. (For the case where they don't need to be random, but whatever).
       // And then SetText just loads the file and updates buffers.

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
    uint32_t CharacterIndex = 0;
    while (true) {
        level += std::to_string(puzzle->cells[CharacterIndex]);
        
        if (CharacterIndex == cellCount - 1) {
            break;
        }
        
        if ((CharacterIndex + 1) % resolution == 0) {
            level += '\n';
        } else {
            level += ' ';
        }
        CharacterIndex++;
    }
    
    /* Example for level 3:
        "123456789"
        "1 2 3\n4 5 6\n7 8 9"
        
        Just read file to load the level instead of generating stuff...
        Then just assert that level is valid and load and gg.
        
    */
    return level;
}

#endif


void SetText(Puzzle* const puzzle,
             CharacterQuad* const buffer, // Where to ASSERT and what?
             const uint32_t resolution) {
    const string level = GenerateLevel(puzzle, resolution);
    //GenerateLevel stores the level in strong format with spaces and newlines.
    // Then we iterate the level character by character and update the CharacterQuad buffer.
    uint32_t* NumberOfDigitsOnEachCell = FillLevelInfo(puzzle, level, resolution);
    // This buffer has the same number of elements as the cells buffer..
    // So that's a parallel buffer. That looks like something that can be
    // calculated procedurally. so there's that.
    // base 10 with log10 or something.
    
    // Make resolution a constexpr and refactor so everything else is constexpr.
    
    
    const float CellScale = 1.0f / float(resolution);
    const float HalfCellScale = CellScale / 2.0f;
    const float VerticalPadding = 0.6f;
    
    uint32_t CIX = 0;
    uint32_t CIY = 0;
    float CharacterScale = 1.0f + 1.0f / float(NumberOfDigitsOnEachCell[CIY * resolution + CIX]);
    float LocalX = -CharacterScale * ((float(NumberOfDigitsOnEachCell[CIY * resolution + CIX]) - 1.0f) / 2.0f);
    float CellCenterX = -1.0f + CellScale;
    float CellCenterY = -1.0f + CellScale;
    
    uint32_t CharacterIndex = 0;
    
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
        
        glm::vec2 UV = GetAtlasUV(Character);
        
        CharacterQuad& quad = buffer[CharacterIndex];
        CharacterIndex += 1;
        
        float X = (LocalX * VerticalPadding - CharacterScale) * HalfCellScale;
        float Y = -CharacterScale * HalfCellScale;
        float W = 2.0f * CharacterScale * HalfCellScale;
        float H = 2.0f * CharacterScale * HalfCellScale;
        quad.rect = glm::vec4(X, Y, W, H);
        quad.uv_rect = glm::vec4(UV.x, UV.y, 0.125f, 0.125f);
        quad.ndc = glm::vec4(CellCenterX, CellCenterY, 0.0f, 0.0f);
        
        LocalX += CharacterScale;
    }
    
    free(NumberOfDigitsOnEachCell);
}

void SetLevelText(UI* const ui,
                  Puzzle* const puzzle,
                  uint32_t level_number) {
    const string text = std::to_string(level_number);                     //Do better.
    puzzle->number_of_digits_in_current_level = (uint32_t)text.length();  //Do better.
    
    CharacterQuad* CQ = puzzle->level_buffer;
    
    float CharacterScale = 1.0f;
    float LocalX = -CharacterScale * ((float(puzzle->number_of_digits_in_current_level) - 1.0f) / 2.0f);
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
        CQ[i].ndc = glm::vec4(0.0f); // This is useless here. Because NDC values
                                     // are taken from the UIUBO for that quad.
                                     // How to split the similarities?
                                     // One system to handle grid rendering
                                     //     (SetText)
                                     // And one to handle the UI rendering
                                     //     (SetLevelText)
        
        LocalX += CharacterScale;
    }
}
