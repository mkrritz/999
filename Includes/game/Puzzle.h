#ifndef Puzzle_HPP
#define Puzzle_HPP

const uint32_t MAX_RESOLUTION = 35;

inline uint32_t CountDigits(uint32_t Value) {
    uint32_t DigitCount = 0u;
    while (Value > 0u) {
        Value /= 10u;
        DigitCount++;
    }
    return DigitCount ? DigitCount : 1u;
}

struct CharacterQuad;
struct Puzzle {
    uint32_t quads_count;
    uint32_t* cells;
    
    GLuint cells_handle;
    CharacterQuad* cells_buffer;
    
    GLuint level_handle;
    CharacterQuad* level_buffer;
    
    uint32_t level_quads_count;
};

#endif
