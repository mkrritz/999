

#include "pch.h"
#include <fstream>
#include <sstream>

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

void SetText(Puzzle* const puzzle,
             CharacterQuad* const buffer,
             const uint32_t resolution) {
    const string Level = LoadLevel(puzzle, resolution); // Maybe this is parameter.
    
    uint32_t ValidChacater = 0;
    
    const float CellScale = 1.0f / float(resolution);
    const float HalfCellScale = CellScale / 2.0f;
    const float VerticalPadding = 0.6f;
    
    uint32_t Width = 0;  // Characters in line.
    uint32_t Height = 0; // Lines.
    uint32_t Digits = CountDigits(puzzle->cells[Height * resolution + Width]);
    float CharacterScale = 1.0f + 1.0f / float(Digits);
    float LocalX = -CharacterScale * ((float(Digits) - 1.0f) / 2.0f);
    float CellCenterX = -1.0f + CellScale;
    float CellCenterY = -1.0f + CellScale;
    
    uint32_t CharacterIndex = 0;
    while(true) {
        const char Character = Level[CharacterIndex++];
        
        if (Character == '\0') {
            break;
        }
        if (Character == '\n') {
            Height += 1;
            Width = 0;
            Digits = CountDigits(puzzle->cells[Height * resolution + Width]);
            CharacterScale = 1.0f + 1.0f / float(Digits);
            LocalX = -CharacterScale * ((float(Digits) - 1.0f) / 2.0f);
            CellCenterX = -1.0f + CellScale;
            CellCenterY += 2.0f * CellScale;
            continue;
        }
        if (Character == ' ') {
            Width += 1;
            Digits = CountDigits(puzzle->cells[Height * resolution + Width]);
            CharacterScale = 1.0f + 1.0f / float(Digits);
            LocalX = -CharacterScale * ((float(Digits) - 1.0f) / 2.0f);
            CellCenterX += 2.0f * CellScale;
            continue;
        }
        
        glm::vec2 UV = GetAtlasUV(Character);
        
        CharacterQuad& quad = buffer[ValidChacater++];
        
        float X = (LocalX * VerticalPadding - CharacterScale) * HalfCellScale;
        float Y = -CharacterScale * HalfCellScale;
        float W = 2.0f * CharacterScale * HalfCellScale;
        float H = 2.0f * CharacterScale * HalfCellScale;
        quad.rect = glm::vec4(X, Y, W, H);
        quad.uv_rect = glm::vec4(UV.x, UV.y, 0.125f, 0.125f);
        quad.ndc = glm::vec4(CellCenterX, CellCenterY, 0.0f, 0.0f);
        // Remove the two 0.125 and the two 0.0 and keep 2 vec4s.
        // It's 0.125 because the font image contains a grid of 8x8 characters
        // The two 0.0 are useless.
        
        LocalX += CharacterScale;
    }
    puzzle->quads_count = ValidChacater;
}

void SetLevelText(UI* const ui,
                  Puzzle* const puzzle,
                  uint32_t level_number) {
    const string text = std::to_string(level_number);
    uint32_t num_digits = CountDigits(level_number);
    
    CharacterQuad* CQ = puzzle->level_buffer;
    
    float CharacterScale = 1.0f;
    float LocalX = -CharacterScale * ((float(num_digits) - 1.0f) / 2.0f);
    const float VerticalPadding = 0.3f;
    float scale2 = 0.15f; // What is this really? Or what is the CharacterScale above really?
    
    for(size_t i = 0; i < text.length(); i++) {
        char Character = text[i];
        
        float W = scale2;
        float H = scale2;
        float X = (LocalX * VerticalPadding) * scale2;
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
