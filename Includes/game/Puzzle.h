#ifndef Puzzle_HPP
#define Puzzle_HPP

struct Overlay {
    GLuint vao, vbo, ibo;
    size_t quads_count;
};

struct Grid {
    uint32_t* cells;
};

struct Puzzle {
    Grid* grid;
    Overlay surface;
};

//struct Text2D {
//    GLuint vao, vbo;
//    uint32_t characters;
//    uint32_t capacity;
//    float* vertex_buffer;
//    
//    GLuint ubo_handle;
//    UniformBufferObjectText* ubo;
//};

#endif