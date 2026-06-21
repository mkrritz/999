#ifndef Puzzle_HPP
#define Puzzle_HPP

const uint32_t MAX_LEVEL = 20;

struct Puzzle {
    uint32_t* cells;
    
    GLuint vbo;
    size_t quads_count;               // Name these proper.
    void* quads_ptr;                  // Name these proper.
                                      // Name these proper.
    GLuint level_vbo;                 // Name these proper.
    uint32_t level_quads_count;       // Name these proper.
    void* level_quads_ptr;            // Name these proper.
};

#endif