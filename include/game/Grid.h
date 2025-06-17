#ifndef GRID_HPP
#define GRID_HPP

struct Grid {
    uint32_t N;
    uint32_t* cells;
};

struct TextField {
    GLuint vao, vbo, ibo;
    std::string text;
    size_t length;
};


#endif