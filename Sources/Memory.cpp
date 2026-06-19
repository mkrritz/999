
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
    
    heaps[(uint32_t)Heaps::Grid]   = HeapInit(sizeof(uint32_t*) + sizeof(uint32_t) * MAX_LEVEL * MAX_LEVEL);
    
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