#ifndef Puzzle_HPP
#define Puzzle_HPP

constexpr uint32_t MAX_RESOLUTION = 5;

struct CharacterQuad;
struct Puzzle {
    uint32_t quads_count;
    uint32_t* cells;
    
    GLuint cells_handle;
    CharacterQuad* cells_buffer;
    
    uint32_t number_of_digits_in_current_level; // not here.
    GLuint level_handle;
    CharacterQuad* level_buffer;
};

#endif
