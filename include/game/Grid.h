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




// The game stored in Main heap
// Grid stored in Grid heap
// TextField stored in Main heap because there are GPU resources there.
// On new level Grid heap is reset, and GPU resources are updated.
struct Puzzle {
    Grid* grid;
    TextField level;
};

#endif