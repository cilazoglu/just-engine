
#include "allocator.h"

static Allocator DEFAULT_ALLOCATOR = {0};

// AllocatorVTable

static AllocatorVTable ALLOCATOR_VTABLE = {0};

void allocator_vtable_reserve(usize count) {
    dynarray_reserve(ALLOCATOR_VTABLE, .rows, count);
}

// NOTE: requires strict all-reserves => all-adds order
AllocatorFns* allocator_vtable_add_entry(AllocatorFns allocator_fns_impl) {
    dynarray_push_back(ALLOCATOR_VTABLE, .rows, allocator_fns_impl);
    return &ALLOCATOR_VTABLE.rows[ALLOCATOR_VTABLE.count - 1];
}

// Allocator

void set_default_allocator(Allocator allocator) {
    DEFAULT_ALLOCATOR = allocator;
}

#define get_allocator(allocator) ((allocator).vtable_ptr ? (allocator) : DEFAULT_ALLOCATOR)

void* just_alloc(Allocator allocator, usize size) {
    return get_allocator(allocator).vtable_ptr->alloc_fn(allocator.data, array_layoutof(byte, size));

    // if (allocator.vtable_ptr) {
    //     return allocator.vtable_ptr->alloc_fn(allocator.data, array_layoutof(byte, size));
    // }
    // else if (DEFAULT_ALLOCATOR.vtable_ptr) {
    //     return DEFAULT_ALLOCATOR.vtable_ptr->alloc_fn(allocator.data, array_layoutof(byte, size));
    // }
    // return std_heap_alloc(allocator.data, array_layoutof(byte, size));
}

void* just_alloc_aligned(Allocator allocator, MemoryLayout layout) {
    return get_allocator(allocator).vtable_ptr->alloc_fn(allocator.data, layout);
}

void* just_realloc(Allocator allocator, void* ptr, usize size) {
    return get_allocator(allocator).vtable_ptr->realloc_fn(allocator.data, ptr, array_layoutof(byte, size));
}

void* just_realloc_aligned(Allocator allocator, void* ptr, MemoryLayout layout) {
    return get_allocator(allocator).vtable_ptr->realloc_fn(allocator.data, ptr, layout);
}

void* just_free(Allocator allocator, void* ptr) {
    get_allocator(allocator).vtable_ptr->free_fn(allocator.data, ptr);
}

// -----

static AllocatorFns* std_heap_allocator_fns;
static void* std_heap_alloc(void* allocator_data, MemoryLayout layout) {
    return std_malloc(layout.size); // malloc is always system highest aligned
}
static void* std_heap_realloc(void* allocator_data, void* ptr, MemoryLayout layout) {
    return std_realloc(ptr, layout.size); // realloc is always system highest aligned
}
static void std_heap_free(void* allocator_data, void* ptr) {
    std_free(ptr);
}
static void std_heap_allocator__allocator_vtable_add_entry() {
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
static void just_bump_free(void* allocator_data, void* ptr) {
    // TODO: could free if ptr is the last allocated
    // pass
}
static void just_bump_allocator__allocator_vtable_add_entry() {
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
Allocator just_as_bump_allocator(BumpAllocator* bump_allocator) {
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
static void just_arena_free(void* allocator_data, void* ptr) {
    // TODO: could free if ptr is the last allocated in any region
    // pass
}
static void just_arena_allocator__allocator_vtable_add_entry() {
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
Allocator just_as_arena_allocator(ArenaAllocator* arena_allocator) {
    return (Allocator) {
        .data = arena_allocator,
        .vtable_ptr = just_arena_allocator_fns,
    };
}

void just_engine__allocator_vtable_add_entries() {
    std_heap_allocator__allocator_vtable_add_entry();
    just_bump_allocator__allocator_vtable_add_entry();
    just_arena_allocator__allocator_vtable_add_entry();
    set_default_allocator(std_heap_allocator());
}

