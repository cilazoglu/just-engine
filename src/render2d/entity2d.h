#pragma once

#include "base.h"
#include "memory/memory.h"

typedef enum {
    Rotation_CW = 1,
    Rotation_CCW = -1,
} RotationWay;

typedef struct {
    uint32 z_index;
    Anchor anchor;
    Vector2 position;       // position of the anchor
    bool use_source_size;
    Vector2 size;
    Vector2 scale;
    float32 rotation;       // rotation around its anchor
    RotationWay rway;
} Transform2D;

typedef struct {
    usize kind;
    usize data_size;
    Transform2D transform;
} Entity2D;

typedef struct {
    usize index;
    usize generation;
} EntityKey;

#define ENTITY_KEY_IS_INVALID(key) (key.index == USIZE_MAX)
#define ENTITY_KEY_SLOT_IS_FREE(key) (IS_EVEN(key.generation))
#define ENTITY_KEY_SLOT_IS_OCCUPIED(key) (IS_ODD(key.generation))
static inline EntityKey invalid_entity_key() {
    return (EntityKey) { .index = USIZE_MAX };
}

typedef struct {
    MemoryLayout data_layout;
    // --
    usize count;
    usize capacity;
    usize free_list_head;
    // usize free_list_tail; // NOTE: I dont find any use
    EntityKey* indices; // occupied: .index -> data, free: .index -> indices (next_free), last_free ([.capacity]) := USIZE_MAX
    byte* data; // Entity2D*
    usize* erase;
} EntityStore;

EntityStore make_entity_store(usize entity_size, usize capacity);
void destroy_entity_store(EntityStore* store);
void entity_store_sort_z_index(EntityStore* store);
bool entity_store_grow(EntityStore* store, usize new_capacity);
// EntityKey spawn_entity(EntityStore* store, Entity2D entity);
EntityKey insert_entity_data(EntityStore* store, byte* entity_data, usize entity_size);
bool entity_is_valid(EntityStore* store, EntityKey key);
Entity2D* get_entity(EntityStore* store, EntityKey key);
Entity2D* get_entity_checked(EntityStore* store, EntityKey key);
bool despawn_entity(EntityStore* store, EntityKey key);

#define make_uniform_entity_store(EntityType, capacity) make_entity_store(sizeof(EntityType), (capacity))
#define spawn_entity(store_ptr, entity) insert_entity_data((store_ptr), &((entity)), sizeof((entity)))

typedef struct {
    usize index;
    usize count;
    usize entity_size;
    byte* data;
} EntityStoreIter;

EntityStoreIter entity_store_begin_iter(EntityStore* store);
Entity2D* next_entity(EntityStoreIter* iter);
