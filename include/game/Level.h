#ifndef LEVEL_HPP
#define LEVEL_HPP

struct Level {
    float     grid_resolution; // N
    float     highlight_x;
    float     highlight_y;
    float     highlight_window;
    
    uint32_t *grid; // store these on an actual Heap maybe?
    size_t    rows;
    size_t    cols;
};

#endif