
#include "justengine.h"

typedef enum {
    EntityKind_Circle,
    EntityKind_Rectangle,
    EntityKind_Sprite,
} EntityKind;

typedef enum {
    Rotation_CW = 1,
    Rotation_CCW = -1,
} RotationWay;

typedef struct {
    Anchor anchor;
    Vector2 position;       // position of the anchor
    bool use_source_size;
    Vector2 size;
    Vector2 scale;
    float32 rotation;       // rotation around its anchor
    RotationWay rway;
} Transform2D;

typedef struct {
    bool render_anchor;
    uint32 render_anchor_radius;
    Color render_anchor_color;
    bool render_frame;
    uint32 render_frame_thickness;
    Color render_frame_color;
} SpriteDebugRenderOptions;

typedef struct {
    // -- render start
    TextureHandle texture;
    Color tint;
    bool use_custom_source;
    Rectangle source;
    bool flip_x;
    bool flip_y;
    // -- render end
    SpriteDebugRenderOptions debug_render_options;
} Sprite;

typedef struct {
    float32 radius;
    Color color;
} CircleEntity;

typedef struct {
    Color color;
} RectangleEntity;

typedef struct {
    Sprite sprite;
} SpriteEntity;

typedef struct {
    EntityBase entity;
    Transform2D transform;
    bool hidden;
    bool camera_visible;
    union {
        CircleEntity circle_entity;
        RectangleEntity rectangle_entity;
        SpriteEntity sprite_entity;
    };
} GameEntity;

typedef struct {
    RenderEntityBase render_entity;
    // --
    float32 radius;
    // --
    Texture texture;
    Color color;
    Rectangle source;
    Vector2 position;
    Vector2 size;
    Vector2 origin;
    float32 rotation;
    // --
    SpriteDebugRenderOptions sprite_debug_render_options;
} GameRenderEntity;

typedef struct {
    TextureAssets* texture_assets;
} GameEntityExtractRes;

typedef struct {
    TextureAssets* texture_assets;
} GameEntityRenderRes;

EntityKey spawn_game_entity(EntityStore* store, GameEntity entity) {
    return spawn_entity(store, entity);
}

void extract_entity(void* EXTRACT_RES, EntityBase* entity, RenderEntityBase* set_render_entity) {
    GameEntityExtractRes* RES = EXTRACT_RES;
    GameEntity* game_entity = (void*)entity;
    GameRenderEntity* set_game_render_entity = (void*)set_render_entity;

    switch (game_entity->entity.kind) {
    case EntityKind_Circle: {
        *set_game_render_entity = (GameRenderEntity) {
            .render_entity = (RenderEntityBase) {
                // .data_size = sizeof(GameRenderEntity),
            },
            .color = game_entity->circle_entity.color,
            .position = game_entity->transform.position,
            .radius = game_entity->circle_entity.radius,
        };
    } break;
    case EntityKind_Rectangle: {
        *set_game_render_entity = (GameRenderEntity) {
            .render_entity = (RenderEntityBase) {
                // .data_size = sizeof(GameRenderEntity),
            },
            .color = game_entity->rectangle_entity.color,
            .position = game_entity->transform.position,
            .size = game_entity->transform.size,
        };
    } break;
    case EntityKind_Sprite: {
        Transform2D* transform;
        Sprite* sprite;

        Rectangle source;
        Rectangle source_flip;
        Vector2 size;
        Rectangle destination;
        Vector2 origin;
        float32 rotation;

        transform = &game_entity->transform;
        sprite = &game_entity->sprite_entity.sprite;

        TextureResponse texture_response = texture_assets_get_texture(RES->texture_assets, sprite->texture);
        ASSERT(texture_response.exists);
        // Texture* texture = texture_assets_get_texture_or_default(RES->texture_assets, game_render_entity->texture);
        Texture texture = *texture_response.texture;

        source = sprite->use_custom_source
            ? sprite->source
            : (Rectangle) {0, 0, texture.width, texture.height};
        source_flip = source;
        source_flip.width *= sprite->flip_x ? -1 : 1;
        source_flip.height *= sprite->flip_y ? -1 : 1;
        size = transform->use_source_size
            ? (Vector2) {source.width, source.height}
            : transform->size;
        size = Vector2Multiply(size, transform->scale);
        // destination
        origin = Vector2Multiply(transform->anchor.origin, size);
        rotation = transform->rotation * transform->rway;

        *set_game_render_entity = (GameRenderEntity) {
            .render_entity = (RenderEntityBase) {
                // .data_size = sizeof(GameRenderEntity),
            },
            .texture = texture,
            .color = sprite->tint,
            .source = source_flip,
            .position = transform->position,
            .size = size,
            .origin = origin,
            .rotation = rotation,
            .sprite_debug_render_options = sprite->debug_render_options,
        };
    } break;
    }
}

static EntityKey SCREEN_ENTITY = {100, 100};
void render_entity(void* RENDER_RES, RenderEntityBase* render_entity) {
    GameEntityRenderRes* RES = RENDER_RES;
    GameRenderEntity* game_render_entity = (void*)render_entity;

    switch (game_render_entity->render_entity.kind) {
    case EntityKind_Circle: {
        DrawCircle(100, 100, 50, game_render_entity->color);
    } break;
    case EntityKind_Rectangle: {
        DrawRectangleV(game_render_entity->position, game_render_entity->size, game_render_entity->color);
    } break;
    case EntityKind_Sprite: {
        Rectangle destination = (Rectangle) {
            .x = game_render_entity->position.x,
            .y = game_render_entity->position.y,
            .width = game_render_entity->size.x,
            .height = game_render_entity->size.y,
        };
        DrawTexturePro(
            game_render_entity->texture,
            game_render_entity->source,
            destination,
            game_render_entity->origin,
            game_render_entity->rotation,
            game_render_entity->color
        );

        SpriteDebugRenderOptions debug = game_render_entity->sprite_debug_render_options;
        if (debug.render_anchor) {
            Vector2 pos = { game_render_entity->position.x, game_render_entity->position.y };
            DrawCircleV(pos, debug.render_anchor_radius, debug.render_anchor_color);
        }
        if (debug.render_frame) {
            Vector2 pos = { game_render_entity->position.x, game_render_entity->position.y };
            pos = Vector2Subtract(pos, game_render_entity->origin);
            Rectangle dest = {
                .x = pos.x,
                .y = pos.y,
                .width = game_render_entity->size.x,
                .height = game_render_entity->size.y,
            };
            DrawRectangleLinesEx(dest, debug.render_frame_thickness, debug.render_frame_color);
        }
    } break;
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
    RenderTargetId target;
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
} CameraRenderList;

typedef struct {
    usize count; // == camera_count
    usize capacity;
    CameraRenderList* items;
} CameraRenderLists;

void entity_camera2d_store_prepare_for_render(EntityCamera2DStore* camera_store, CameraRenderLists* crender_lists) {
    if (crender_lists->count < camera_store->count) {
        usize reserve_count = camera_store->count - crender_lists->count;
        dynarray_reserve(*crender_lists, .items, reserve_count);

        for (usize i = 0; i < camera_store->count; i++) {
            crender_lists->items[i].camera_id = camera_store->cameras[i].id;
        }
        for (usize i = crender_lists->count; i < camera_store->count; i++) {
            crender_lists->items[i].render_list = make_uniform_entity_render_list(GameRenderEntity, 10, extract_entity, render_entity);
        }
        crender_lists->count = camera_store->count;
    }
}

void entity_render_list_extract_with_entity_camera2d(
    TextureAssets* RES_texture_assets,
    EntityRenderList* render_list,
    EntityStore* store,
    CameraId camera_id,
    EntityCamera2D* entity_camera
) {
    GameEntityExtractRes EXTRACT_RES = {
        .texture_assets = RES_texture_assets,
    };

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
    entity_render_list_extract_base(render_list, store, &EXTRACT_RES);
}

void entity_render_extract_for_each_entity_camera2d(
    TextureAssets* RES_texture_assets,
    CameraRenderLists* crender_lists,
    EntityStore* store,
    EntityCamera2DStore* camera_store
) {
    for (usize i = 0; i < crender_lists->count; i++) {
        CameraRenderList* crender_list = &crender_lists->items[i];
        CameraId camera_id = crender_list->camera_id;
        EntityCamera2D* camera = get_entity_camera2d(camera_store, camera_id);
        entity_render_list_extract_with_entity_camera2d(RES_texture_assets, &crender_list->render_list, store, camera_id, camera);
    }
}

void entity_render_sort_for_each_entity_camera2d(CameraRenderLists* crender_lists) {
    for (usize i = 0; i < crender_lists->count; i++) {
        CameraRenderList* crender_list = &crender_lists->items[i];
        entity_render_list_sort(&crender_list->render_list);
    }
}

void entity_render2d_begin_camera_render(EntityCamera2D* entity_camera) {
    BeginMode2D(entity_camera->camera);
}

void entity_render2d_end_camera_render() {
    EndMode2D();
}

void entity_render_for_each_entity_camera2d(
    TextureAssets* RES_texture_assets,
    CameraRenderLists* crender_lists,
    EntityCamera2DStore* camera_store,
    RenderTargetId active_render_target
) {
    GameEntityRenderRes RENDER_RES = {
        .texture_assets = RES_texture_assets,
    };
    for (usize i = 0; i < crender_lists->count; i++) {
        CameraRenderList* crender_list = &crender_lists->items[i];
        EntityCamera2D* camera = get_entity_camera2d(camera_store, crender_list->camera_id);
        if (camera->target == active_render_target) {
            entity_render2d_begin_camera_render(camera);
                entity_render_list_render(&crender_list->render_list, &RENDER_RES);
            entity_render2d_end_camera_render();
        }
    }
}

static RenderTargetType RENDER2D_CURRENT_RENDER_TARGET_TYPE = -1;

void entity_render2d_begin_render_target_render(RenderTarget* render_target) {
    RENDER2D_CURRENT_RENDER_TARGET_TYPE = render_target->type;

    switch (RENDER2D_CURRENT_RENDER_TARGET_TYPE) {
    case RENDER_TARGET_WINDOW:
        RenderTargetWindow* render_target_window = &render_target->target.window;
        BeginDrawing();
        ClearBackground(render_target_window->clear_color);
        break;
    case RENDER_TARGET_TEXTURE:
        RenderTargetTexture* render_target_texture = &render_target->target.texture;
        BeginTextureMode(render_target_texture->texture);
        ClearBackground(render_target_texture->clear_color);
        break;
    default:
        PANIC("Unsupported RenderTargetType.");
    }
}

void entity_render2d_end_render_target_render() {
    switch (RENDER2D_CURRENT_RENDER_TARGET_TYPE) {
    case RENDER_TARGET_WINDOW:
        EndDrawing();
        break;
    case RENDER_TARGET_TEXTURE:
        EndTextureMode();
        break;
    default:
        PANIC("Unsupported RenderTargetType.");
    }

    RENDER2D_CURRENT_RENDER_TARGET_TYPE = -1;
}

static RenderTargetId WINDOW_RENDER_TARGET;
static RenderTargetId SCREEN_TEXTURE_TARGET;

void render_entities_on_each_render_target(
    TextureAssets* RES_texture_assets,
    RenderTargets* render_targets,
    CameraRenderLists* crender_lists,
    EntityCamera2DStore* camera_store
) {
    RenderTargetId RENDER_TARGET_ORDER[] = {
        SCREEN_TEXTURE_TARGET,
        WINDOW_RENDER_TARGET,
    };

    for (usize i = 0; i < ARRAY_LENGTH(RENDER_TARGET_ORDER); i++) {
        RenderTargetId active_render_target = RENDER_TARGET_ORDER[i];
        RenderTarget* render_target = get_render_target(render_targets, active_render_target);
        entity_render2d_begin_render_target_render(render_target);
            entity_render_for_each_entity_camera2d(
                RES_texture_assets,
                crender_lists,
                camera_store,
                active_render_target
            );
        entity_render2d_end_render_target_render();
    }
}

int main_2() {
    RenderTargets RENDER_TARGETS = {0};

    URectSize window_size = { 1000, 1000 };
    WINDOW_RENDER_TARGET = create_render_target_window(&RENDER_TARGETS, "Entity Render System Test", window_size, WHITE);

    // InitWindow(1000, 1000, "Entity Render System Test");
    SetTargetFPS(60);

    TextureAssets TEXTURE_ASSETS = new_texture_assets();
    EntityStore ENTITY_STORE = make_uniform_entity_store(GameEntity, 1);
    EntityCamera2DStore CAMERA_STORE = make_entity_camera2d_store();
    CameraRenderLists CAMERA_RENDER_LISTS = {0};

    TextureAssets* texture_assets = &TEXTURE_ASSETS;
    EntityStore* entity_store = &ENTITY_STORE;
    EntityCamera2DStore* camera_store = &CAMERA_STORE;
    CameraRenderLists* camera_render_lists = &CAMERA_RENDER_LISTS;

    Image texture_image = LoadImage("C:\\dev\\c\\just-engine\\test-assets\\fire.png");
    TextureHandle texture_handle = texture_assets_reserve_texture_slot(texture_assets);
    texture_assets_load_texture_then_unload_image(texture_assets, texture_handle, texture_image);
    
    URectSize screen_render_texture_size = { 1000, 1000 };
    SCREEN_TEXTURE_TARGET = create_render_target_texture(&RENDER_TARGETS, screen_render_texture_size, GRAY);
    
    TextureHandle screen_texture_handle = texture_assets_reserve_texture_slot(texture_assets);
    JUST_LOG_DEBUG("screen_texture_handle: %d\n", screen_texture_handle.id);
    {
        RenderTarget* screen_texture_target = get_render_target(&RENDER_TARGETS, SCREEN_TEXTURE_TARGET);
        texture_assets_put_texture(texture_assets, screen_texture_handle, screen_texture_target->target.texture.texture.texture);
        // Texture* screen_texture_slot = texture_assets_get_texture_unchecked(texture_assets, screen_texture_handle);
        // *screen_texture_slot = screen_texture_target->target.texture.texture.texture;
    }

    // INIT
    set_primary_entity_camera2d(
        camera_store,
        (EntityCamera2D) {
            // .id = PRIMARY_CAMERA_ID,
            .sort_index = 0,
            .camera = (Camera2D) {
                .offset = {
                    .x = 1000 / 2.0,
                    .y = 1000 / 2.0,
                },
                .target = {0},
                .rotation = 0,
                .zoom = 1,
            },
            .target = WINDOW_RENDER_TARGET,
            .layers = on_primary_layer(),
        }
    );

    #define CAMERA_2 2
    add_entity_camera2d(
        camera_store,
        (EntityCamera2D) {
            .id = CAMERA_2,
            .sort_index = 10,
            .camera = (Camera2D) {
                .offset = {
                    .x = 0,
                    .y = 0,
                },
                .target = {0},
                .rotation = 0,
                .zoom = 1,
            },
            .target = SCREEN_TEXTURE_TARGET,
            .layers = on_single_layer(2),
        }
    );

    EntityKey screen_texture_entity_key = spawn_game_entity(
        entity_store,
        (GameEntity) {
            .entity = (EntityBase) {
                .kind = EntityKind_Sprite,
                // .data_size = sizeof(GameEntity),
                .visible = false,
                .sort_index = 0,
            },
            .transform = (Transform2D) {
                .anchor = make_anchor(Anchor_Center),
                .position = (Vector2) { 0, 0 },
                .use_source_size = false,
                .size = (Vector2) { 800, 800 },
                .scale = (Vector2) { 1, 1 },
                .rotation = 0,
                // .rway = Rotation_CW,
            },
            .sprite_entity = (SpriteEntity) {
                .sprite = (Sprite) {
                    .texture = screen_texture_handle,
                    .tint = WHITE,
                    .use_custom_source = false,
                    // .source = {},
                    .flip_x = false,
                    .flip_y = true,
                    .debug_render_options = {
                        .render_frame = true,
                        .render_frame_color = RED,
                        .render_frame_thickness = 10,
                    },
                },
            },
        }
    );
    SCREEN_ENTITY = screen_texture_entity_key;
    JUST_LOG_DEBUG("entity_check: {%llu, %llu} ?= {%llu, %llu} -> %s\n",
        screen_texture_entity_key.index, screen_texture_entity_key.generation,
        SCREEN_ENTITY.index, SCREEN_ENTITY.generation,
        ENTITY_KEY_EQUALS(screen_texture_entity_key, SCREEN_ENTITY) ? "true" : "false"
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
            .transform = (Transform2D) {
                .position = (Vector2) { 100, 100 },
            },
            .circle_entity = (CircleEntity) {
                .radius = 50,
                .color = GREEN,
            },
        }
    );
    
    EntityKey entity2_key = spawn_game_entity(
        entity_store,
        (GameEntity) {
            .entity = (EntityBase) {
                .kind = EntityKind_Rectangle,
                .data_size = sizeof(GameEntity),
                .visible = false,
                .sort_index = 10,
            },
            .transform = (Transform2D) {
                .anchor = make_anchor(Anchor_Center),
                .position = (Vector2) { 100, 100 },
                .size = (Vector2) { 100, 100 },
                .scale = (Vector2) { 1, 1 },
            },
            .rectangle_entity = (RectangleEntity) {
                .color = BLUE,
            },
        }
    );

    EntityKey entity3_key = spawn_game_entity(
        entity_store,
        (GameEntity) {
            .entity = (EntityBase) {
                .kind = EntityKind_Sprite,
                // .data_size = sizeof(GameEntity),
                .visible = false,
                .sort_index = 10,
                .use_layer_system = true,
                .layers = on_single_layer(2),
            },
            .transform = (Transform2D) {
                .anchor = make_anchor(Anchor_Top_Left),
                .position = (Vector2) { 100, 100 },
                .use_source_size = false,
                .size = (Vector2) { 490 * 0.5, 970 * 0.5 },
                .scale = (Vector2) { 1, 1 },
                .rotation = 0,
                // .rway = Rotation_CW,
            },
            .sprite_entity = (SpriteEntity) {
                .sprite = (Sprite) {
                    .texture = texture_handle,
                    .tint = WHITE,
                    .use_custom_source = false,
                    // .source = {},
                    .flip_x = false,
                    .flip_y = true,
                    // .debug_render_options = {
                    //     .render_frame = true,
                    //     .render_frame_color = RED,
                    //     .render_frame_thickness = 10,
                    // },
                },
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
        get_entity_key(entity_store, entity);
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
        entity_camera2d_store_prepare_for_render(camera_store, camera_render_lists);

        // RENDER_EXTRACT
        entity_render_extract_for_each_entity_camera2d(texture_assets, camera_render_lists, entity_store, camera_store);

        // RENDER_SORT
        entity_render_sort_for_each_entity_camera2d(camera_render_lists);

        // RENDER
        render_entities_on_each_render_target(texture_assets, &RENDER_TARGETS, camera_render_lists, camera_store);
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
        entity_render_list_extract(render_list, store, NULL);

        // RENDER_SORT
        entity_render_list_sort(render_list);

        // RENDER
        BeginDrawing();
            ClearBackground(WHITE);
            entity_render_list_render(render_list, NULL);
        EndDrawing();
    }

    destroy_entity_render_pair(&ENTITY_RENDER);

    return 0;
}
