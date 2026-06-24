
#include "pch.h"

#include "game/Assert.h"
#include "game/Memory.h"
#include "game/Puzzle.h"

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
    heaps[(uint32_t)Heaps::Camera] = HeapInit(1024);
    heaps[(uint32_t)Heaps::Font]   = HeapInit(1024);
    heaps[(uint32_t)Heaps::Shader] = HeapInit(5 * 1024);
    heaps[(uint32_t)Heaps::UI]     = HeapInit(1024);
    
    heaps[(uint32_t)Heaps::Main]   = HeapInit(sizeof(Puzzle));
    
    heaps[(uint32_t)Heaps::Grid]   = HeapInit(sizeof(uint32_t) * MAX_RESOLUTION * MAX_RESOLUTION);
}

void* HeapAllocate(Heap* const heap, size_t size) {
    ASSERT (heap->offset + size > heap->size, "Heap full. Give more memory. But which one?"); // Next time this triggers, do some better printing
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

void PrintMemory(const Heap* const* heaps) {
    printf("\n");
    for (uint32_t index = 0; index < (uint32_t)Heaps::Count; index++) {
        std::string offset = FormatWithCommas(heaps[index]->offset);
        std::string size = FormatWithCommas(heaps[index]->size);
        printf("%s memory:  %s / %s\n", HeapNames[index], offset.c_str(), size.c_str());
    }
    printf("\n");
}

