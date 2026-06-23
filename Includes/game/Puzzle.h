#ifndef Puzzle_HPP
#define Puzzle_HPP

constexpr uint32_t MAX_LEVEL = 40;

struct CharacterQuad;
struct Puzzle {
    uint32_t* cells;
    
    uint32_t quads_count; // The total number of quads in all cells.
    
    GLuint cells_handle;
    CharacterQuad* cells_buffer;
    
    uint32_t number_of_digits_in_current_level;
    GLuint level_handle;
    CharacterQuad* level_buffer;
};

#endif
