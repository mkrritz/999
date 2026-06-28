
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

# if 1
/*
    Using this LoadLevel to load predetermined levels instead of 
    having functions here to generate new levels at runtime.
    
    That way level design is the python script, or whatever.
    
    Level format is handled elsewhere.
    So SetText is just a parser.
    
    Is the iteration supposed to happen on the character?
    What about on the things that are calculated because of the character?
    For example CharacterQuad buffers and such.
    Meaning, that instead of the level being a string of space seperated values,
    each value being log(value) number of digits, each digit being a quad to be rendered.
    What ekse us tgere t gid?
    Similarly for SetLevelText, which is pretty much the same thing as SetText,
    but instead it's setup to be processed by the UICharacters vertex shader,
    instead of the GridCharacters vertex shader, which again they are pretty much the same.
    The difference is which thing decides where to place the quads?
    
    Also, randomness can be added here.
    Simply adding a (number % level*level) or something similar. (-1 maybe)
    Still not random, but it is.
    
*/

static std::string LoadLevel(Puzzle* const puzzle,
                             const uint32_t resolution) {
    std::string filename = "Levels/" + std::to_string(resolution) + ".txt";
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        ASSERT(true, "Failed to load level file!");
    }
    
    std::string level((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    const uint32_t cellCount = resolution * resolution;
    uint32_t cellIndex = 0;
    std::stringstream ss(level);
    std::string token;
    while (ss >> token) {
        if (cellIndex < cellCount) {
            puzzle->cells[cellIndex++] = std::stoul(token);
        }
    }
    
    return level;
}

#endif

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
    const std::string Level = LoadLevel(puzzle, resolution);
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