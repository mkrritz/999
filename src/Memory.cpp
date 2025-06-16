
#include "pch.h"

#include "game/Assert.h"
#include "game/Memory.h"

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
    heaps[(uint32_t)Heaps::Threads]    = HeapInit(1024);
    
    // Memory for up to level 40. Based on 'struct Grid'.
    heaps[(uint32_t)Heaps::Grid]       = HeapInit(8 + 4 * 40 * 40);
}

void* HeapAllocate(Heap* const heap, size_t size) {
    ASSERT (heap->offset + size > heap->size, "Heap full. Give more memory."); // Next time this triggers, do some better printing
    void* ptr = (char*)heap->memory + heap->offset;
    heap->offset += size;
    return ptr;
}

void HeapReset(Heap* const heap) {
    heap->offset = 0;
}

//void HeapFree(Heap* heap) {
//    free(heap->memory);
//    heap->memory = nullptr;
//    heap->size = 0;
//    heap->offset = 0;
//}
