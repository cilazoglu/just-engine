
#include "justengine.h"

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
    EntityBase entity;
    union {
        CircleEntity circle_entity;
        RectangleEntity rectangle_entity;
    };
} GameEntity;

typedef struct {
    RenderEntityBase render_entity;
    Color color;
} GameRenderEntity;

EntityKey spawn_game_entity(EntityStore* store, GameEntity entity) {
    return spawn_entity(store, entity);
}

void extract_entity(EntityBase* entity, RenderEntityBase* set_render_entity) {
    GameEntity* game_entity = (void*)entity;
    GameRenderEntity* set_game_render_entity = (void*)set_render_entity;

    *set_game_render_entity = (GameRenderEntity) {
        .render_entity = (RenderEntityBase) {
            .kind = game_entity->entity.kind,
            .data_size = game_entity->entity.data_size,
            .sort_index = game_entity->entity.sort_index,
        },
        .color = game_entity->entity.kind == EntityKind_Circle ? game_entity->circle_entity.color : game_entity->rectangle_entity.color
    };
}

void render_entity(RenderEntityBase* render_entity) {
    GameRenderEntity* game_render_entity = (void*)render_entity;

    Color color = game_render_entity->color;
    switch (game_render_entity->render_entity.kind) {
    case EntityKind_Circle:
        DrawCircle(100, 100, 50, color);
        break;
    case EntityKind_Rectangle:
        DrawRectangle(100, 100, 100, 100, color);
        break;
    }
}

// --
#define PRIMARY_CAMERA_ID 0
typedef usize CameraId;

typedef struct {
    CameraId id;
    uint8 sort_index;
    // --
    Camera2D camera;
    RenderTarget target;
    Layers layers;
} EntityCamera2D;

typedef struct {
    usize count;
    usize capacity;
    usize primary_camera_index;
    EntityCamera2D* cameras;
} EntityCamera2DStore;

EntityCamera2DStore make_entity_camera2d_store() {
    EntityCamera2DStore store = {0};
    return store;
}

void destroy_entity_camera2d_store(EntityCamera2DStore* store) {
    dynarray_free(*store, .cameras);
}

EntityCamera2D* get_primary_entity_camera2d(EntityCamera2DStore* store) {
    EntityCamera2D* primary_camera = &store->cameras[store->primary_camera_index];
    return primary_camera->id == PRIMARY_CAMERA_ID ? primary_camera : NULL;
}

void set_primary_entity_camera2d(EntityCamera2DStore* store, EntityCamera2D camera) {
    camera.id = PRIMARY_CAMERA_ID;

    if (store->count == 0) {
        store->primary_camera_index = 0;
        dynarray_push_back(*store, .cameras, camera);
        return;
    }

    EntityCamera2D* primary_camera = get_primary_entity_camera2d(store);
    if (primary_camera != NULL) {
        *primary_camera = camera;
        return;
    }

    // unsorted insert, but should be sorted before render
    store->primary_camera_index = store->count;
    dynarray_push_back(*store, .cameras, camera);
}

EntityCamera2D* get_entity_camera2d(EntityCamera2DStore* store, CameraId camera_id) {
    for (usize i = 0; i < store->count; i++) {
        EntityCamera2D* camera_i = &store->cameras[i];
        if (camera_id == camera_i->id) {
            return camera_i;
        }
    }
    return NULL; 
}

void add_entity_camera2d(EntityCamera2DStore* store, EntityCamera2D camera) {
    ASSERT(camera.id != PRIMARY_CAMERA_ID); // use set_primary_entity_camera2d
    for (usize i = 0; i < store->count; i++) {
        if (store->cameras[i].sort_index <= camera.sort_index) {
            dynarray_insert(*store, i, .cameras, camera);
            if (i <= store->primary_camera_index) {
                store->primary_camera_index++;
            }
            return;
        }
    }
    dynarray_push_back(*store, .cameras, camera);
}

void sort_entity_camera2d_store(EntityCamera2DStore* store) {
    if (store->count == 0) {
        return;
    }

    EntityCamera2D temp;
    bool no_swap_done = true;
    for (usize begin = 0; begin < store->count - 1; begin++) {
        no_swap_done = true;
        for (usize i = store->count - 1; i > begin; i--) {
            if (store->cameras[i].sort_index < store->cameras[i-1].sort_index) {
                // swap
                temp = store->cameras[i];
                store->cameras[i] = store->cameras[i-1];
                store->cameras[i-1] = temp;
                no_swap_done = false;
            }
            if (i == begin+1) {
                break;
            }
        }
        if (no_swap_done) {
            break;
        }
    }

    EntityCamera2D* primary_camera = get_entity_camera2d(store, PRIMARY_CAMERA_ID);
    if (primary_camera != NULL) {
        store->primary_camera_index = (usize)primary_camera - (usize)store->cameras;
    }
}
// --

typedef struct {
    CameraId camera_id;
    EntityRenderList render_list;
} EntityCamera2D_EntityRenderList;

typedef struct {
    usize count; // == camera_count
    usize capacity;
    EntityCamera2D_EntityRenderList* camera_render_lists;
} EntityCamera2D_EntityRenderLists;

void entity_camera2d_store_prepare_for_render(EntityCamera2DStore* camera_store, EntityCamera2D_EntityRenderLists* crender_lists) {
    if (crender_lists->count < camera_store->count) {
        usize reserve_count = camera_store->count - crender_lists->count;
        dynarray_reserve(*crender_lists, .camera_render_lists, reserve_count);

        for (usize i = 0; i < camera_store->count; i++) {
            crender_lists->camera_render_lists[i].camera_id = camera_store->cameras[i].id;
        }
        for (usize i = crender_lists->count; i < camera_store->count; i++) {
            crender_lists->camera_render_lists[i].render_list = make_uniform_entity_render_list(GameRenderEntity, 10, extract_entity, render_entity);
        }
        crender_lists->count = camera_store->count;
    }
}

void entity_render_list_extract_with_entity_camera2d(EntityRenderList* render_list, EntityStore* store, CameraId camera_id, EntityCamera2D* entity_camera) {
    EntityIter store_iter = entity_begin_iter(store->data, store->count, store->data_layout.size);
    EntityBase* entity = NULL;
    while ((entity = next_entity(&store_iter)) != NULL) {
        entity->__internal__.render_decided = false;
        if (
            entity->visible
            && (
                (!entity->use_layer_system && camera_id == PRIMARY_CAMERA_ID)
                || (entity->use_layer_system && check_layer_overlap(entity->layers, entity_camera->layers))
            )
        ) {
            entity->__internal__.render_decided = true;
        }
    }
    entity_render_list_extract_base(render_list, store);
}

void entity_render_extract_for_each_entity_camera2d(EntityCamera2D_EntityRenderLists* crender_lists, EntityStore* store, EntityCamera2DStore* camera_store) {
    for (usize i = 0; i < crender_lists->count; i++) {
        EntityCamera2D_EntityRenderList* crender_list = &crender_lists->camera_render_lists[i];
        CameraId camera_id = crender_list->camera_id;
        EntityCamera2D* camera = get_entity_camera2d(camera_store, camera_id);
        entity_render_list_extract_with_entity_camera2d(&crender_list->render_list, store, camera_id, camera);
    }
}

void entity_render_sort_for_each_entity_camera2d(EntityCamera2D_EntityRenderLists* crender_lists) {
    for (usize i = 0; i < crender_lists->count; i++) {
        EntityCamera2D_EntityRenderList* crender_list = &crender_lists->camera_render_lists[i];
        entity_render_list_sort(&crender_list->render_list);
    }
}

void entity_render_render_for_each_entity_camera2d(EntityCamera2D_EntityRenderLists* crender_lists, EntityCamera2DStore* camera_store) {
    for (usize i = 0; i < crender_lists->count; i++) {
        EntityCamera2D_EntityRenderList* crender_list = &crender_lists->camera_render_lists[i];
        EntityCamera2D* camera = get_entity_camera2d(camera_store, crender_list->camera_id);
        // TODO: begin texture mode
        BeginMode2D(camera->camera);
        entity_render_list_render(&crender_list->render_list);
        EndMode2D();
    }
}

int main_2() {
    EntityStore ENTITY_STORE = make_uniform_entity_store(GameEntity, 1);
    EntityCamera2DStore CAMERA_STORE = make_entity_camera2d_store();
    EntityCamera2D_EntityRenderLists CAMERA_ENTITY_RENDER_LISTS = {0};

    EntityStore* entity_store = &ENTITY_STORE;
    EntityCamera2DStore* camera_store = &CAMERA_STORE;
    EntityCamera2D_EntityRenderLists* camera_entity_render_lists = &CAMERA_ENTITY_RENDER_LISTS;
    
    InitWindow(1000, 1000, "Entity Render System Test");
    SetTargetFPS(60);

    // INIT
    set_primary_entity_camera2d(
        camera_store,
        (EntityCamera2D) {
            // .id = PRIMARY_CAMERA_ID,
            .sort_index = 10,
            .camera = (Camera2D) {
                .offset = {
                    .x = 1000 / 2.0,
                    .y = 1000 / 2.0,
                },
                .target = {0},
                .rotation = 0,
                .zoom = 1,
            },
            .target = (RenderTarget) {
                .type = RENDER_TARGET_WINDOW,
                .clear_color = WHITE,
            },
            .layers = on_primary_layer(),
        }
    );

    EntityKey entity1_key = spawn_game_entity(
        entity_store,
        (GameEntity) {
            .entity = (EntityBase) {
                .kind = EntityKind_Circle,
                // .data_size = sizeof(GameEntity),
                .visible = false,
                .sort_index = 20,
            },
            // .transform = (Transform2D) {
            //     .anchor = make_anchor(Anchor_Center),
            //     .position = (Vector2) { 100, 100 },
            //     .size = (Vector2) { 10, 10 },
            //     .scale = (Vector2) { 1, 1 },
            // },
            .circle_entity = (CircleEntity) {
                .color = GREEN,
            },
        }
    );
    
    EntityKey entity2_key = spawn_game_entity(
        entity_store,
        (GameEntity) {
            .entity = (EntityBase) {
                .kind = EntityKind_Rectangle,
                // .data_size = sizeof(GameEntity),
                .visible = false,
                .sort_index = 10,
            },
            // .transform = (Transform2D) {
            //     .anchor = make_anchor(Anchor_Center),
            //     .position = (Vector2) { 100, 100 },
            //     .size = (Vector2) { 10, 10 },
            //     .scale = (Vector2) { 1, 1 },
            // },
            .rectangle_entity = (RectangleEntity) {
                .color = BLUE,
                .render_frame = true,
                .frame_color = MAGENTA,
            },
        }
    );

    // TEST
    {
        EntityBase* entity1 = get_entity(entity_store, entity1_key);
        EntityKey entity1_key_2 = get_entity_key(entity_store, entity1);
        ASSERT(!ENTITY_KEY_IS_INVALID(entity1_key_2));
        ASSERT(entity1_key.index == entity1_key_2.index && entity1_key.generation == entity1_key_2.generation);
    }

    EntityIter entity_iter = entity_begin_iter(entity_store->data, entity_store->count, entity_store->data_layout.size);
    EntityBase* entity;
    while ((entity = next_entity(&entity_iter)) != NULL) {
        entity->visible = true;
    }

    while (!WindowShouldClose()) {
        // INPUT
        bool toggle_visible_entity1 = IsKeyPressed(KEY_A);
        bool toggle_visible_entity2 = IsKeyPressed(KEY_S);
        bool swap_sort_indices = IsKeyPressed(KEY_SPACE);

        // UPDATE
        if (toggle_visible_entity1) {
            EntityBase* entity1 = get_entity(entity_store, entity1_key);
            entity1->visible ^= 1;
        }
        if (toggle_visible_entity2) {
            EntityBase* entity2 = get_entity(entity_store, entity2_key);
            entity2->visible ^= 1;
        }
        if (swap_sort_indices) {
            EntityBase* entity1 = get_entity(entity_store, entity1_key);
            EntityBase* entity2 = get_entity(entity_store, entity2_key);
            uint8 temp = entity1->sort_index;
            entity1->sort_index = entity2->sort_index;
            entity2->sort_index = temp;
        }

        // RENDER_PREPARE
        entity_camera2d_store_prepare_for_render(camera_store, camera_entity_render_lists);

        // RENDER_EXTRACT
        entity_render_extract_for_each_entity_camera2d(camera_entity_render_lists, entity_store, camera_store);
        // entity_render_list_extract(render_list, store);

        // RENDER_SORT
        entity_render_sort_for_each_entity_camera2d(camera_entity_render_lists);
        // entity_render_list_sort(render_list);

        // RENDER
        BeginDrawing();
            ClearBackground(WHITE);
            entity_render_render_for_each_entity_camera2d(camera_entity_render_lists, camera_store);
        EndDrawing();
    }

    // destroy_entity_render_pair(&ENTITY_RENDER);

    return 0;
}

int main() {
    return main_2();

    EntityRenderPair ENTITY_RENDER = make_uniform_entity_render_pair(
        GameEntity, GameRenderEntity, 1, 1, extract_entity, render_entity
    );
    EntityStore* store = &ENTITY_RENDER.store;
    EntityRenderList* render_list = &ENTITY_RENDER.render_list;

    InitWindow(1000, 1000, "Entity Render System Test");
    SetTargetFPS(60);
    
    // INIT
    EntityKey entity1_key = spawn_game_entity(
        store,
        (GameEntity) {
            .entity = (EntityBase) {
                .kind = EntityKind_Circle,
                // .data_size = sizeof(GameEntity),
                .visible = false,
                .sort_index = 20,
            },
            // .transform = (Transform2D) {
            //     .anchor = make_anchor(Anchor_Center),
            //     .position = (Vector2) { 100, 100 },
            //     .size = (Vector2) { 10, 10 },
            //     .scale = (Vector2) { 1, 1 },
            // },
            .circle_entity = (CircleEntity) {
                .color = GREEN,
            },
        }
    );
    
    EntityKey entity2_key = spawn_game_entity(
        store,
        (GameEntity) {
            .entity = (EntityBase) {
                .kind = EntityKind_Rectangle,
                // .data_size = sizeof(GameEntity),
                .visible = false,
                .sort_index = 10,
            },
            // .transform = (Transform2D) {
            //     .anchor = make_anchor(Anchor_Center),
            //     .position = (Vector2) { 100, 100 },
            //     .size = (Vector2) { 10, 10 },
            //     .scale = (Vector2) { 1, 1 },
            // },
            .rectangle_entity = (RectangleEntity) {
                .color = BLUE,
                .render_frame = true,
                .frame_color = MAGENTA,
            },
        }
    );

    // TEST
    {
        EntityBase* entity1 = get_entity(store, entity1_key);
        EntityKey entity1_key_2 = get_entity_key(store, entity1);
        ASSERT(!ENTITY_KEY_IS_INVALID(entity1_key_2));
        ASSERT(entity1_key.index == entity1_key_2.index && entity1_key.generation == entity1_key_2.generation);
    }

    EntityIter entity_iter = entity_begin_iter(store->data, store->count, store->data_layout.size);
    EntityBase* entity;
    while ((entity = next_entity(&entity_iter)) != NULL) {
        entity->visible = true;
    }

    while (!WindowShouldClose()) {
        // INPUT
        bool toggle_visible_entity1 = IsKeyPressed(KEY_A);
        bool toggle_visible_entity2 = IsKeyPressed(KEY_S);
        bool swap_sort_indices = IsKeyPressed(KEY_SPACE);

        // UPDATE
        if (toggle_visible_entity1) {
            EntityBase* entity1 = get_entity(store, entity1_key);
            entity1->visible ^= 1;
        }
        if (toggle_visible_entity2) {
            EntityBase* entity2 = get_entity(store, entity2_key);
            entity2->visible ^= 1;
        }
        if (swap_sort_indices) {
            EntityBase* entity1 = get_entity(store, entity1_key);
            EntityBase* entity2 = get_entity(store, entity2_key);
            uint8 temp = entity1->sort_index;
            entity1->sort_index = entity2->sort_index;
            entity2->sort_index = temp;
        }

        // RENDER_EXTRACT
        entity_render_list_extract(render_list, store);

        // RENDER_SORT
        entity_render_list_sort(render_list);

        // RENDER
        BeginDrawing();
            ClearBackground(WHITE);
            entity_render_list_render(render_list);
        EndDrawing();
    }

    destroy_entity_render_pair(&ENTITY_RENDER);

    return 0;
}
