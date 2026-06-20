#ifndef Puzzle_HPP
#define Puzzle_HPP

const uint32_t MAX_LEVEL = 20;

struct Puzzle {
    uint32_t* cells;
    GLuint vbo;
    size_t quads_count;
};

#endif