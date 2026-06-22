#ifndef Puzzle_HPP
#define Puzzle_HPP

const uint32_t MAX_LEVEL = 40;

struct CharacterQuad;
struct Puzzle {
    uint32_t* cells; // CPU Side
    
    uint32_t quads_count; // The total number of quads in all cells.
    
    GLuint cells_handle;
    CharacterQuad* cells_buffer;
    
    uint32_t number_of_digits_in_current_level;
    GLuint level_handle;
    CharacterQuad* level_buffer;
};

#endif

// cells in CPU side storing the value of each cell
// quads_ptr is GPU side, storing CharacterQuad.
// Can these two be merged in 1 mapped buffer?
// By adding a 'value' field in the CharacterQuad struct?
// Maybe not because CharacterQuad must be aligned based on
// GPU requirements, and cells is information that
// the GPU doesn't need at all anyway. Maybe another way.