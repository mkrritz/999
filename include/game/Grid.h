#ifndef LEVEL_HPP
#define LEVEL_HPP

struct Grid {
    float     resolution;
    size_t    rows; // same replace with just N
    size_t    cols; // same replace with just N
    uint32_t *cells;
};

#endif