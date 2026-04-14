
#include "core.h"
#include "memory.h"

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

static AllocatorVTable ALLOCATOR_VTABLE = {0};

void allocator_vtable_reserve(usize count) {
    dynarray_reserve_custom(ALLOCATOR_VTABLE, .rows, count);
}

// NOTE: requires strict all-reserves => all-adds order
AllocatorFns* allocator_vtable_add_entry(AllocatorFns allocator_fns_impl) {
    dynarray_push_back_custom(ALLOCATOR_VTABLE, .rows, allocator_fns_impl);
    return &ALLOCATOR_VTABLE.rows[ALLOCATOR_VTABLE.count - 1];
}

typedef struct {
    void* data;
    AllocatorFns* vtable_ptr;
} Allocator;

void* just_alloc(Allocator allocator, MemoryLayout layout) {
    return allocator.vtable_ptr->alloc_fn(allocator.data, layout);
}

void* just_realloc(Allocator allocator, void* ptr, MemoryLayout layout) {
    return allocator.vtable_ptr->realloc_fn(allocator.data, ptr, layout);
}

void* just_free(Allocator allocator, void* ptr) {
    allocator.vtable_ptr->free_fn(allocator.data, ptr);
}

// -----

#define JUST_ENGINE_ALLOCATOR_TYPE_COUNT 3

static AllocatorFns* std_heap_allocator_fns;
static void* std_heap_alloc(void* allocator_data, MemoryLayout layout) {
    return std_malloc(layout.size);
}
static void* std_heap_realloc(void* allocator_data, void* ptr, MemoryLayout layout) {
    return std_realloc(ptr, layout.size);
}
static void* std_heap_free(void* allocator_data, void* ptr) {
    std_free(ptr);
}
void std_heap_allocator__allocator_vtable_add_entry() {
    std_heap_allocator_fns = allocator_vtable_add_entry((AllocatorFns) {
        .alloc_fn = std_heap_alloc,
        .realloc_fn = std_heap_realloc,
        .free_fn = std_heap_free,
    });
}
Allocator std_heap_allocator() {
    return (Allocator) {
        .data = NULL,
        .vtable_ptr = std_heap_allocator_fns,
    };
}

static AllocatorFns* just_bump_allocator_fns;
static void* just_bump_alloc(void* allocator_data, MemoryLayout layout) {
    BumpAllocator* bump_allocator = allocator_data;
    return bump_alloc_aligned(bump_allocator, layout);
}
static void* just_bump_realloc(void* allocator_data, void* ptr, MemoryLayout layout) {
    BumpAllocator* bump_allocator = allocator_data;
    void* new_ptr = bump_alloc_aligned(bump_allocator, layout);
    std_memcpy(new_ptr, ptr, layout.size);
    return new_ptr;
}
static void* just_bump_free(void* allocator_data, void* ptr) {
    // TODO: could free if ptr is the last allocated
    // pass
}
void just_bump_allocator__allocator_vtable_add_entry() {
    just_bump_allocator_fns = allocator_vtable_add_entry((AllocatorFns) {
        .alloc_fn = just_bump_alloc,
        .realloc_fn = just_bump_realloc,
        .free_fn = just_bump_free,
    });
}
Allocator just_bump_allocator(usize size) {
    BumpAllocator* bump_allocator = std_malloc(sizeof(BumpAllocator));
    *bump_allocator = make_bump_allocator_with_size(size);
    return (Allocator) {
        .data = bump_allocator,
        .vtable_ptr = just_bump_allocator_fns,
    };
}

static AllocatorFns* just_arena_allocator_fns;
static void* just_arena_alloc(void* allocator_data, MemoryLayout layout) {
    ArenaAllocator* arena_allocator = allocator_data;
    return arena_alloc_aligned(arena_allocator, layout);
}
static void* just_arena_realloc(void* allocator_data, void* ptr, MemoryLayout layout) {
    ArenaAllocator* arena_allocator = allocator_data;
    void* new_ptr = arena_alloc_aligned(arena_allocator, layout);
    std_memcpy(new_ptr, ptr, layout.size);
    return new_ptr;
}
static void* just_arena_free(void* allocator_data, void* ptr) {
    // TODO: could free if ptr is the last allocated in any region
    // pass
}
void just_arena_allocator__allocator_vtable_add_entry() {
    just_arena_allocator_fns = allocator_vtable_add_entry((AllocatorFns) {
        .alloc_fn = just_arena_alloc,
        .realloc_fn = just_arena_realloc,
        .free_fn = just_arena_free,
    });
}
Allocator just_arena_allocator(usize region_size) {
    ArenaAllocator* arena_allocator = std_malloc(sizeof(ArenaAllocator));
    *arena_allocator = make_arena_allocator_with_region_size(region_size);
    return (Allocator) {
        .data = arena_allocator,
        .vtable_ptr = just_arena_allocator_fns,
    };
}

void just_engine__allocator_vtable_add_entries() {
    std_heap_allocator__allocator_vtable_add_entry();
    just_bump_allocator__allocator_vtable_add_entry();
    just_arena_allocator__allocator_vtable_add_entry();
}

#define GAME_ALLOCATOR_TYPE_COUNT 1

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
        + JUST_ENGINE_ALLOCATOR_TYPE_COUNT
        + GAME_ALLOCATOR_TYPE_COUNT
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
