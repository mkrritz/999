#ifndef Puzzle_HPP
#define Puzzle_HPP

struct Overlay {
    GLuint vao, vbo, ibo;
    size_t quads_count;
};

struct Grid;
struct Puzzle {
    Grid* grid;
    Overlay surface;
    
    std::string level;
};

#endif