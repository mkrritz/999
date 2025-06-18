
#include "pch.h"

#include "game/Assert.h"
#include "game/Memory.h"

static const char* HeapNames[(uint32_t)Heaps::Count] = {
    // Keep these in sync with 'enum struct Heaps'
    // Used for prints only
    "Shader",
    "UI",
    "Main",
    "Font",
    "Camera",
    "Grid",
};

static Heap* HeapInit(size_t size) {
    Heap* heap = (Heap*)malloc(sizeof(Heap));
    ASSERT(heap == nullptr, "\nHoly shit");
    heap->memory = calloc(1, size);
    ASSERT(heap->memory == nullptr, "Not better");
    heap->size = size;
    heap->offset = 0;
    return heap;
}

void HeapsInit(Heap* heaps[]) {
    heaps[(uint32_t)Heaps::Camera]     = HeapInit(1024);
    heaps[(uint32_t)Heaps::Font]       = HeapInit(1024);
    heaps[(uint32_t)Heaps::Shader]     = HeapInit(5 * 1024);
    heaps[(uint32_t)Heaps::UI]         = HeapInit(1024);
    
    //Stores 'struct Puzzle'.
    heaps[(uint32_t)Heaps::Main]       = HeapInit(64);
    
    // Stores 'struct Grid'.
    heaps[(uint32_t)Heaps::Grid]       = HeapInit(8 + 4 * 40 * 40); // Memory for up to level 40. 
}

void* HeapAllocate(Heap* const heap, size_t size) {
    ASSERT (heap->offset + size > heap->size, "Heap full. Give more memory.");
    void* ptr = (char*)heap->memory + heap->offset;
    heap->offset += size;
    return ptr;
}

void HeapReset(Heap* const heap) {
    heap->offset = 0;
}

static std::string FormatWithCommas(uint32_t value) {
    std::string str = std::to_string(value);
    int index = str.length() - 3;
    while (index > 0) {
        str.insert(index, ".");
        index -= 3;
    }
    return str;
}

void PrintMemory(const Heap* const heaps[]) {
    printf("\n");
    for (uint32_t i = 0; i < (uint32_t)Heaps::Count; ++i) {
        std::string offset = FormatWithCommas(heaps[i]->offset);
        std::string size = FormatWithCommas(heaps[i]->size);
        printf("%s memory:  %s / %s\n", HeapNames[i], offset.c_str(), size.c_str());
    }
    printf("\n");
}

