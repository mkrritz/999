#ifndef MEMORY_HPP
#define MEMORY_HPP

struct Heap {
    void* memory;
    size_t size;
    size_t offset;
};

enum struct Heaps {
    Shader,
    UI,
    Main,
    Font,
    Camera,
    Grid,
    Count
};

void* HeapAllocate(Heap* const, size_t);
void HeapReset(Heap* const);

#endif