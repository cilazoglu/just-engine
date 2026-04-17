#pragma once

#include "core.h"
#include "memory.h"
#include "dynarray.h"

typedef void* (*AllocFn)(void* allocator_data, MemoryLayout layout);
typedef void* (*ReallocFn)(void* allocator_data, void* ptr, MemoryLayout layout);
typedef void (*FreeFn)(void* allocator_data, void* ptr);

typedef struct {
    AllocFn alloc_fn;
    ReallocFn realloc_fn;
    FreeFn free_fn;
} AllocatorFns;

typedef struct {
    usize count;
    usize capacity;
    AllocatorFns* rows;
} AllocatorVTable;

void allocator_vtable_reserve(usize count);

// NOTE: requires strict all-reserves => all-adds order
AllocatorFns* allocator_vtable_add_entry(AllocatorFns allocator_fns_impl);

typedef struct {
    void* data;
    AllocatorFns* vtable_ptr;
} Allocator;

#define NO_ALLOCATOR (Allocator){0}
#define ALLOCATOR_IS_NULL(allocator) ((allocator).vtable_ptr == NULL)

void set_default_allocator(Allocator allocator);

#define just_alloc_single(allocator, Type)          just_alloc_aligned((allocator), layoutof(Type))
#define just_alloc_array(allocator, Type, count)    just_alloc_aligned((allocator), array_layoutof(Type, (count)))
#define just_realloc_single(allocator, Type, ptr)          just_realloc_aligned((allocator), (ptr), layoutof(Type))
#define just_realloc_array(allocator, Type, ptr, count)    just_realloc_aligned((allocator), (ptr), array_layoutof(Type, (count)))

void* just_alloc(Allocator allocator, usize size);
void* just_alloc_aligned(Allocator allocator, MemoryLayout layout);
void* just_realloc(Allocator allocator, void* ptr, usize size);
void* just_realloc_aligned(Allocator allocator, void* ptr, MemoryLayout layout);
void* just_free(Allocator allocator, void* ptr);

// Allocator Impls

#define JUST_ENGINE_ALLOCATOR_IMPL_COUNT 3
void just_engine__allocator_vtable_add_entries();

Allocator std_heap_allocator();                     // C malloc-free
Allocator just_bump_allocator(usize size);          // BumpAllocator
Allocator just_arena_allocator(usize region_size);  // ArenaAllocator

Allocator just_as_bump_allocator(BumpAllocator* bump_allocator);
Allocator just_as_arena_allocator(ArenaAllocator* arena_allocator);

// Example
#if 0

#define GAME_ALLOCATOR_IMPL_COUNT 1

static AllocatorFns* game_non_allocator_fns;
static void* game_non_alloc(void* allocator_data, MemoryLayout layout) {
    return NULL;
}
static void* game_non_realloc(void* allocator_data, void* ptr, MemoryLayout layout) {
    return NULL;
}
static void* game_non_free(void* allocator_data, void* ptr) {
    // pass
}
void game_non_allocator__allocator_vtable_add_entry() {
    game_non_allocator_fns = allocator_vtable_add_entry((AllocatorFns) {
        .alloc_fn = game_non_alloc,
        .realloc_fn = game_non_realloc,
        .free_fn = game_non_free,
    });
}
Allocator game_non_allocator() {
    return (Allocator) {
        .data = NULL,
        .vtable_ptr = game_non_allocator_fns,
    };
}

void game__allocator_vtable_add_entries() {
    game_non_allocator__allocator_vtable_add_entry();
}

void test() {
    allocator_vtable_reserve(0
        + JUST_ENGINE_ALLOCATOR_IMPL_COUNT
        + GAME_ALLOCATOR_IMPL_COUNT
    );
    just_engine__allocator_vtable_add_entries();
    game__allocator_vtable_add_entries();

    Allocator allocators[] = {
        std_heap_allocator(),
        just_bump_allocator(1024),
        just_arena_allocator(1024),
        game_non_allocator(),
    };
}

#endif