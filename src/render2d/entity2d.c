
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
    std_memset(store.indices, 0, sizeof(store.indices[0]) * capacity);

    store.data = std_malloc(entity_size * capacity);
    // std_memset(store.data, 0, entity_size * capacity);

    store.erase = std_malloc(sizeof(store.erase[0]) * capacity);
    // std_memset(store.erase, 0, sizeof(store.erase[0]) * capacity);

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

    usize old_capacity = store->capacity;
    usize reserve_count = new_capacity - store->count;
    dynarray_reserve2(*store, .indices, .erase, reserve_count);
    store->data = std_realloc(store->data, store->data_layout.size * new_capacity);

    if (store->free_list_head != USIZE_MAX) {
        store->indices[old_capacity-1].index = old_capacity;
    }
    else {
        store->free_list_head = old_capacity;
    }

    for (usize i = old_capacity; i < new_capacity; i++) {
        store->indices[i].index = i+1;
        store->indices[i].generation = 0;
    }
    store->indices[new_capacity-1].index = USIZE_MAX;

    return true;
}

static inline byte* store_get_data_i(EntityStore* store, usize i) {
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
        return invalid_entity_key(); // TODO: instead try the next free until found good generation, this slot is forever lost
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
    std_memcpy(store_get_data_i(store, data_key.index), entity_data, entity_size);
    store->erase[data_key.index] = return_key.index;
    store->count++;

    return return_key;
}

bool entity_is_valid(EntityStore* store, EntityKey key) {
    return key.index < store->capacity && ENTITY_KEY_SLOT_IS_OCCUPIED(key) && key.generation == store->indices[key.index].generation;
}

Entity2D* get_entity(EntityStore* store, EntityKey key) {
    usize data_index = store->indices[key.index].index;
    byte* entity_data = store_get_data_i(store, data_index);
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

    byte* src_data = store_get_data_i(store, store->count);
    byte* dst_data = store_get_data_i(store, remove_data_key.index);
    std_memcpy(dst_data, src_data, store->data_layout.size);
    dynarray_swap_remove(*store, remove_data_key.index, .erase); // store->count--

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

typedef struct {
    usize kind;
    usize data_size;
    uint8 sort_index;
} RenderEntity2D;

typedef RenderEntity2D* (*EntityRenderExtractFn)(Entity2D* entity);
typedef void (*RenderEntityRenderFn)(RenderEntity2D* render_entity);

#define RADIX 256
typedef struct {
    usize count;
    usize capacity;
    usize render_entity_size; // sizeof(Type impl RenderEntity2D)
    usize freq[RADIX];
    byte* data;
    byte* sorted_data;
    // --
    EntityRenderExtractFn extract_fn;
    RenderEntityRenderFn render_fn;
} EntityRenderList;

static inline byte* render_list_get_data_i(EntityRenderList* render_list, usize i) {
    return render_list->data + (i * render_list->render_entity_size);
}
static inline byte* render_list_get_sort_data_i(EntityRenderList* render_list, usize i) {
    return render_list->sorted_data + (i * render_list->render_entity_size);
}

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

typedef struct {
    RenderEntity2D render_entity;
    Color color;
} GameRenderEntity;

RenderEntity2D* extract_test(Entity2D* entity) {
    static GameRenderEntity temp = {0};
    GameEntity* game_entity = (void*)entity;
    temp.render_entity.kind = game_entity->entity.kind;
    temp.render_entity.data_size = game_entity->entity.data_size;
    temp.render_entity.sort_index = game_entity->entity.sort_index;
    temp.color = game_entity->entity.kind == EntityKind_Circle ? game_entity->circle_entity.color : game_entity->rectangle_entity.color;
    return &temp;
}

EntityRenderList make_entity_render_list(usize render_entity_size, EntityRenderExtractFn extract_fn, RenderEntityRenderFn render_fn) {
    return (EntityRenderList) {
        .count = 0,
        .capacity = 0,
        .render_entity_size = render_entity_size,
        .freq = {0},
        .data = NULL,
        .sorted_data = NULL,
        // --
        .extract_fn = extract_fn,
        .render_fn = render_fn,
    };
}

void entity_render_list_extract(EntityRenderList* render_list, EntityStore* store) {
    usize visible_count = 0;
    for (usize i = 0; i < store->count; i++) {
        Entity2D* entity = (void*)store_get_data_i(store, i);
        if (entity->visible) {
            visible_count++;
        }
    }

    if (render_list->capacity < visible_count) {
        #define OVER_FACTOR 2
        render_list->capacity = MIN(OVER_FACTOR * visible_count, store->count);
        usize new_size = render_list->render_entity_size * render_list->capacity;
        render_list->data = std_realloc(render_list->data, new_size);
        render_list->sorted_data = std_realloc(render_list->sorted_data, new_size);
        #undef OVER_FACTOR
    }

    render_list->count = 0;
    for (usize i = 0; i < store->count; i++) {
        Entity2D* entity = (void*)store_get_data_i(store, i);
        if (entity->visible) {
            RenderEntity2D* render_entity_src = render_list->extract_fn(entity);
            RenderEntity2D* render_entity_slot = (void*)render_list_get_data_i(render_list, render_list->count++);
            std_memcpy(render_entity_slot, render_entity_src, render_list->render_entity_size);
        }
    }
}

void entity_render_list_sort(EntityRenderList* render_list) {
    // init
    for (usize i = 0; i < RADIX; i++) {
        render_list->freq[i] = 0;
    }

    // count frequencies
    for (usize i = 0; i < render_list->count; i++) {
        RenderEntity2D* entity = (void*)render_list_get_data_i(render_list, i);
        render_list->freq[entity->sort_index]++;
    }

    // compute cumulative
    for (usize i = 1; i < RADIX; i++) {
        render_list->freq[i] += render_list->freq[i-1];
    }

    // build sorted
    for (usize i = 0; i < render_list->count; i++) {
        usize back_i = render_list->count - 1 - i;

        RenderEntity2D* entity_i = (void*)render_list_get_data_i(render_list, back_i);
        render_list->freq[entity_i->sort_index]--;

        byte* sorted_data_i = render_list_get_sort_data_i(render_list, render_list->freq[entity_i->sort_index]);
        std_memcpy(sorted_data_i, entity_i, render_list->render_entity_size);
    }

    // swap
    byte* temp = render_list->data;
    render_list->data = render_list->sorted_data;
    render_list->sorted_data = temp;
}

// -----



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
