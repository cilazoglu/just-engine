#pragma once

#include "base.h"
#include "memory/memory.h"

typedef struct {
    usize index;
    usize generation;
} EntityKey;

#define ENTITY_KEY_IS_INVALID(key) (key.index == JUST_USIZE_MAX)
#define ENTITY_KEY_SLOT_IS_FREE(key) (IS_EVEN(key.generation))
#define ENTITY_KEY_SLOT_IS_OCCUPIED(key) (IS_ODD(key.generation))
static inline EntityKey invalid_entity_key() {
    return (EntityKey) { .index = JUST_USIZE_MAX };
}

typedef struct {
    bool render_decided;
} EntityBaseInternal;

typedef struct {
    usize kind;
    usize data_size;
    // --
    uint8 sort_index;
    // --
    bool visible;
    // --
    bool use_layer_system;
    Layers layers;
    // --
    EntityBaseInternal __internal__;
} EntityBase;

#define entity_data_i(data_ptr, entity_size, i) ((byte*)(((byte*)(data_ptr)) + ((entity_size) * (i))))

typedef struct {
    MemoryLayout data_layout; // sizeof(Type impl Entity2D)
    // --
    usize count;
    usize capacity;
    usize free_list_head;
    EntityKey* indices; // occupied: .index -> data, free: .index -> indices (next_free), last_free ([.capacity]) := USIZE_MAX
    byte* data; // Entity2D*
    usize* erase;
} EntityStore;

EntityStore make_entity_store(usize entity_size, usize capacity);
void destroy_entity_store(EntityStore* store);
void entity_store_sort_z_index(EntityStore* store);
bool entity_store_grow(EntityStore* store, usize new_capacity);
// EntityKey spawn_entity(EntityStore* store, EntityBase entity);
EntityKey insert_entity_data(EntityStore* store, byte* entity_data, usize entity_size);
bool entity_is_valid(EntityStore* store, EntityKey key);
EntityBase* get_entity(EntityStore* store, EntityKey key);
EntityBase* get_entity_checked(EntityStore* store, EntityKey key);
EntityKey get_entity_key(EntityStore* store, EntityBase* entity);
bool despawn_entity(EntityStore* store, EntityKey key);

#define make_uniform_entity_store(EntityType, capacity) make_entity_store(sizeof(EntityType), (capacity))
#define spawn_entity(store_ptr, entity) insert_entity_data((store_ptr), (void*)&((entity)), sizeof((entity)))

typedef struct {
    usize kind;
    usize data_size;
    uint8 sort_index;
} RenderEntityBase;

typedef void (*EntityRenderExtractFn)(EntityBase* entity, RenderEntityBase* set_render_entity);
typedef void (*RenderEntityRenderFn)(RenderEntityBase* render_entity);

#define RENDER_LIST_SORT_RADIX 256
typedef struct {
    usize count;
    usize capacity;
    usize render_entity_size; // sizeof(Type impl RenderEntityBase)
    usize freq[RENDER_LIST_SORT_RADIX];
    byte* data;
    byte* sorted_data;
    // --
    RenderEntityBase* render_entity_src_slot;
    EntityRenderExtractFn extract_fn;
    RenderEntityRenderFn render_fn;
} EntityRenderList;

EntityRenderList make_entity_render_list(usize render_entity_size, usize capacity, EntityRenderExtractFn extract_fn, RenderEntityRenderFn render_fn);
void destroy_entity_render_list(EntityRenderList* render_list);

/**
 * call just after `entity->__internal__.render_decided` is set for each entity
 */
void entity_render_list_extract_base(EntityRenderList* render_list, EntityStore* store);
void entity_render_list_extract(EntityRenderList* render_list, EntityStore* store);
void entity_render_list_sort(EntityRenderList* render_list);
void entity_render_list_render(EntityRenderList* render_list);

#define make_uniform_entity_render_list(RenderEntityType, capacity, extract_fn, render_fn) \
    make_entity_render_list(sizeof(RenderEntityType), (capacity), (extract_fn), (render_fn))

typedef struct {
    usize index;
    usize count;
    usize entity_size;
    byte* data;
} EntityIter;

EntityIter entity_begin_iter(byte* data, usize data_count, usize data_size);
void entity_iter_reset(EntityIter* iter);
EntityBase* next_entity(EntityIter* iter);
RenderEntityBase* next_render_entity(EntityIter* iter);

typedef struct {
    EntityStore store;
    EntityRenderList render_list;
} EntityRenderPair;

EntityRenderPair make_entity_render_pair(
    usize entity_size, usize entity_store_capacity,
    usize render_entity_size, usize render_list_capacity, EntityRenderExtractFn extract_fn, RenderEntityRenderFn render_fn
);
void destroy_entity_render_pair(EntityRenderPair* entity_render);

#define make_uniform_entity_render_pair(EntityType, RenderEntityType, entity_store_capacity, render_list_capacity, extract_fn, render_fn) \
    make_entity_render_pair( \
        sizeof(EntityType), (entity_store_capacity), \
        sizeof(RenderEntityType), (render_list_capacity), (extract_fn), (render_fn) \
    )

// --
