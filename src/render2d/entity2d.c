
#include "math/vector.h"
#include "memory/dynarray.h"

#include "entity2d.h"

EntityStore make_entity_store(usize entity_size, usize capacity) {
    ASSERT(capacity > 0);
    EntityStore store = {
        .data_layout = (MemoryLayout) { .size = entity_size, .alignment = _Alignof(usize) }, 
        .count = 0,
        .capacity = capacity,
        .free_list_head = 0,
        // .free_list_tail = capacity-1,
        .indices = NULL,
        .data = NULL,
        .erase = NULL,
    };
    store.indices = std_malloc(sizeof(store.indices[0]) * capacity);
    store.data = std_malloc(entity_size * capacity);
    store.erase = std_malloc(sizeof(store.erase[0]) * capacity);

    for (usize i = 0; i < capacity; i++) {
        store.indices[i].index = i+1;
    }
    store.indices[capacity-1].index = USIZE_MAX;

    return store;
}

void destroy_entity_store(EntityStore* store) {
    dynarray_free3(*store, .indices, .data, .erase);
}

void entity_store_sort_z_index(EntityStore* store) {
    UNREACHABLE(); // TODO
}

bool entity_store_grow(EntityStore* store, usize new_capacity) {
    if (new_capacity == USIZE_MAX) {
        return false;
    }
    if (new_capacity <= store->capacity) {
        return true;
    }

    usize reserve_count = new_capacity - store->count;
    usize store_capacity_backup = store->capacity;
    dynarray_reserve2(*store, .indices, .erase, reserve_count);
    store->capacity = store_capacity_backup;
    store->data = std_realloc(store->data, store->data_layout.size * new_capacity);

    if (store->free_list_head != USIZE_MAX) {
        store->indices[store->capacity-1].index = store->capacity;
    }
    else {
        store->free_list_head = store->capacity;
    }

    for (usize i = store->capacity; i < new_capacity; i++) {
        store->indices[i].index = i+1;
    }
    store->indices[new_capacity-1].index = USIZE_MAX;

    store->capacity = new_capacity;
    return true;
}

static inline byte* get_data_i(EntityStore* store, usize i) {
    return store->data + (i * store->data_layout.size);
}

EntityKey insert_entity_data(EntityStore* store, byte* entity_data, usize entity_size) {
    if (store->free_list_head == USIZE_MAX) {
        #define GROW_FACTOR 2

        usize new_capacity =
            (store->capacity < (USIZE_MAX / 2))
                ? GROW_FACTOR * store->capacity
                : store->capacity + ((USIZE_MAX - store->capacity)/2); // asymptotic approach to USIZE_MAX, new_capacity: <= USIZE_MAX-1
        new_capacity = MIN(new_capacity, USIZE_MAX-1); // NOTE: unnecessary

        if (!entity_store_grow(store, new_capacity)) {
            // OUT-OF-POINTER-SPACE
            return invalid_entity_key();
        }

        #undef GROW_FACTOR
    }

    usize index = store->free_list_head;
    store->free_list_head = store->indices[index].index; // last_free (index: store->capacity-1) has index USIZE_MAX
    usize generation = store->indices[index].generation;
    if (generation == USIZE_MAX) {
        // OUT-OF-GENERATION-SPACE
        return invalid_entity_key();
    }

    EntityKey return_key = {
        .index = index,
        .generation = generation + 1,
    };
    EntityKey data_key = {
        .index = store->count,
        .generation = generation + 1,
    };

    store->indices[return_key.index] = data_key;
    // store->data[data_key.index] = entity;
    std_memcpy(get_data_i(store, data_key.index), entity_data, entity_size);
    store->erase[data_key.index] = return_key.index;
    store->count++;

    return return_key;
}

bool entity_is_valid(EntityStore* store, EntityKey key) {
    return key.index < store->capacity && ENTITY_KEY_SLOT_IS_OCCUPIED(key) && key.generation == store->indices[key.index].generation;
}

Entity2D* get_entity(EntityStore* store, EntityKey key) {
    usize data_index = store->indices[key.index].index;
    byte* entity_data = get_data_i(store, data_index);
    return (Entity2D*) entity_data;
    // return &store->data[store->indices[key.index].index];
}

Entity2D* get_entity_checked(EntityStore* store, EntityKey key) {
    if (!entity_is_valid(store, key)) {
        return NULL;
    }
    return get_entity(store, key);
}

bool despawn_entity(EntityStore* store, EntityKey key) {
    if (!entity_is_valid(store, key)) {
        return false;
    }

    EntityKey remove_data_key = store->indices[key.index];

    store->indices[key.index].generation++;
    store->indices[key.index].index = store->free_list_head;
    store->free_list_head = key.index;

    byte* src_data = get_data_i(store, store->count);
    byte* dst_data = get_data_i(store, remove_data_key.index);
    std_memcpy(dst_data, src_data, store->data_layout.size);
    dynarray_swap_remove(*store, remove_data_key.index, .erase); // store->count++

    if (store->count > 0) {
        store->indices[store->erase[remove_data_key.index]].index = remove_data_key.index;
    }
}

// -----

EntityStoreIter entity_store_begin_iter(EntityStore* store) {
    return (EntityStoreIter) {
        .index = 0,
        .count = store->count,
        .entity_size = store->data_layout.size,
        .data = store->data,
    };
}

static inline byte* iter_get_data_i(EntityStoreIter* iter, usize i) {
    return iter->data + (i * iter->entity_size);
}

Entity2D* next_entity(EntityStoreIter* iter) {
    if (iter->count <= iter->index) {
        return NULL;
    }
    return (Entity2D*) iter_get_data_i(iter, iter->index++);
}

// -----

typedef enum {
    EntityKind_Circle,
    EntityKind_Rectangle,
} EntityKind;

typedef struct {
    Color color;
} CircleEntity;

typedef struct {
    Color color;
    bool render_frame;
    Color frame_color;
} RectangleEntity;

typedef struct {
    Entity2D entity;
    union {
        CircleEntity circle_entity;
        RectangleEntity rectangle_entity;
    };
} GameEntity;

void spawn_game_entity(EntityStore* store, GameEntity entity) {
    spawn_entity(store, entity);
}

static void entity2d_test() {
    EntityStore STORE = make_uniform_entity_store(GameEntity, 100);
    EntityStore* store = &STORE;

    spawn_game_entity(
        store,
        (GameEntity) {
            .entity = (Entity2D) {
                .kind = EntityKind_Circle,
                // .data_size = sizeof(GameEntity),
                .transform = (Transform2D) {
                    .anchor = make_anchor(Anchor_Center),
                    .position = (Vector2) { 100, 100 },
                    .size = (Vector2) { 10, 10 },
                    .scale = (Vector2) { 1, 1 },
                }
            },
            .circle_entity = (CircleEntity) {
                .color = RED,
            }
        }
    );
    
    spawn_game_entity(
        store,
        (GameEntity) {
            .entity = (Entity2D) {
                .kind = EntityKind_Rectangle,
                // .data_size = sizeof(GameEntity),
                .transform = (Transform2D) {
                    .anchor = make_anchor(Anchor_Center),
                    .position = (Vector2) { 100, 100 },
                    .size = (Vector2) { 10, 10 },
                    .scale = (Vector2) { 1, 1 },
                }
            },
            .rectangle_entity = (RectangleEntity) {
                .color = BLUE,
                .render_frame = true,
                .frame_color = MAGENTA,
            }
        }
    );
}
