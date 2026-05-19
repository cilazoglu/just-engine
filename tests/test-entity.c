
#include "justengine.h"

typedef enum {
    EntityKind_Circle,
    EntityKind_Rectangle,
    EntityKind_Sprite,
} EntityKind;

typedef struct {
    float32 radius;
    Color color;
} CircleEntity;

typedef struct {
    Color color;
} RectangleEntity;

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
    float32 radius;
    // --
    Color color;
    Vector2 position;
    Vector2 size;
    // --
    SpriteEntityRender sprite_render;
} GameRenderEntity;

typedef struct {
    TextureAssets* texture_assets;
} GameEntityExtractRes;

typedef struct {
    TextureAssets* texture_assets;
} GameEntityRenderRes;

GameEntityExtractRes GAME_ENTITY_EXTRACT_RES = STARTUP_INIT;
GameEntityRenderRes GAME_ENTITY_RENDER_RES = STARTUP_INIT;

EntityKey spawn_game_entity(EntityStore* store, GameEntity entity) {
    // return invalid_entity_key();
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
            .size = game_entity->transform.scale,
        };
    } break;
    case EntityKind_Sprite: {
        SpriteExtractRes res = {
            .texture_assets = RES->texture_assets,
        };
        SpriteEntityRender sprite_render = extract_sprite_entity(res, &game_entity->transform, &game_entity->sprite_entity);

        *set_game_render_entity = (GameRenderEntity) {
            .render_entity = (RenderEntityBase) {
                // .data_size = sizeof(GameRenderEntity),
            },
            .sprite_render = sprite_render,
        };
    } break;
    }
}

bool SW = false;

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
        render_sprite_entity(&game_render_entity->sprite_render);
    } break;
    }
}

EntityStore game_make_entity_store() {
    return make_uniform_entity_store(GameEntity, 1);
}

EntityRenderList game_make_entity_render_list() {
    return make_uniform_entity_render_list(GameRenderEntity, 1, extract_entity, render_entity);
}

EntityKey entity1_key;
EntityKey entity2_key;

void SYSTEM_UPDATE_0() {
    if (WindowShouldClose()) just_engine_mark_exit();

    // INPUT
    bool toggle_visible_entity1 = IsKeyPressed(KEY_A);
    bool toggle_visible_entity2 = IsKeyPressed(KEY_S);
    bool swap_sort_indices = IsKeyPressed(KEY_SPACE);

    SW = IsKeyDown(KEY_W);

    // UPDATE
    if (toggle_visible_entity1) {
        EntityBase* entity1 = get_entity(&JUST_GLOBAL.entity_store, entity1_key);
        entity1->visible ^= 1;
    }
    if (toggle_visible_entity2) {
        EntityBase* entity2 = get_entity(&JUST_GLOBAL.entity_store, entity2_key);
        entity2->visible ^= 1;
    }
    if (swap_sort_indices) {
        EntityBase* entity1 = get_entity(&JUST_GLOBAL.entity_store, entity1_key);
        EntityBase* entity2 = get_entity(&JUST_GLOBAL.entity_store, entity2_key);
        uint8 temp = entity1->sort_index;
        entity1->sort_index = entity2->sort_index;
        entity2->sort_index = temp;
    }
}

void SYSTEM_POST_UPDATE_0() {
    // prepare
    {
        render2d_prepare_camera_render_lists(
            &JUST_GLOBAL.camera_store,
            &JUST_RENDER_GLOBAL.crender_lists,
            JUST_GLOBAL.functions.entity_render_list_make_fn
        );
    }

    // extract
    {
        render2d_extract_for_each_camera2d(
            JUST_RENDER_GLOBAL.EXTRACT_RES,
            &JUST_RENDER_GLOBAL.crender_lists,
            &JUST_GLOBAL.entity_store,
            &JUST_GLOBAL.camera_store
        );
    }

    // sort
    {
        render2d_sort_for_each_camera2d(
            &JUST_RENDER_GLOBAL.crender_lists
        );
    }

    // render
    {
        render2d_render_entities_on_each_render_target_except_window(
            JUST_RENDER_GLOBAL.RENDER_RES,
            &JUST_GLOBAL.render_targets,
            &JUST_RENDER_GLOBAL.crender_lists,
            &JUST_GLOBAL.camera_store,
            JUST_GLOBAL.render_target_order.order,
            JUST_GLOBAL.render_target_order.count
        );
    }

    // render_screen_begin
    {
        RenderTargets* render_targets = &JUST_GLOBAL.render_targets;
        RenderTargetId main_window_render_target = JUST_GLOBAL.known_render_targets.main_window;

        RenderTarget* render_target = get_render_target(render_targets, main_window_render_target);
        render_target_begin_render(render_target);
    }

    // render_screen
    {
        void* RENDER_RES = JUST_RENDER_GLOBAL.RENDER_RES;
        RenderTargets* render_targets = &JUST_GLOBAL.render_targets;
        CameraRenderLists* crender_lists = &JUST_RENDER_GLOBAL.crender_lists;
        EntityCamera2DStore* camera_store = &JUST_GLOBAL.camera_store;
        RenderTargetId main_window_render_target = JUST_GLOBAL.known_render_targets.main_window;

        entity_render_for_each_camera2d(
            RENDER_RES,
            crender_lists,
            camera_store,
            main_window_render_target
        );
    }

    // render_screen_end
    {
        render_target_end_render();
    }
}

void INIT() {
    GAME_ENTITY_EXTRACT_RES = (GameEntityExtractRes) {
        .texture_assets = &JUST_GLOBAL.texture_assets,
    };
    GAME_ENTITY_RENDER_RES = (GameEntityRenderRes) {
        .texture_assets = &JUST_GLOBAL.texture_assets,
    };

    TextureHandle test_texture_handle = texture_assets_reserve_texture_slot(&JUST_GLOBAL.texture_assets);
    {
        Texture texture = LoadTexture("C:\\dev\\c\\just-engine\\test-assets\\fire.png");
        texture_assets_put_texture(&JUST_GLOBAL.texture_assets, test_texture_handle, texture);
    }

    // INIT
    set_primary_entity_camera2d(
        &JUST_GLOBAL.camera_store,
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
            .target = JUST_GLOBAL.known_render_targets.main_window,
            .layers = on_primary_layer(),
        }
    );

    #define CAMERA_2 2
    add_entity_camera2d(
        &JUST_GLOBAL.camera_store,
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
            .target = JUST_GLOBAL.known_render_targets.render_screen,
            .layers = on_single_layer(2),
        }
    );

    EntityKey screen_texture_entity_key = spawn_game_entity(
        &JUST_GLOBAL.entity_store,
        (GameEntity) {
            .entity = (EntityBase) {
                .kind = EntityKind_Sprite,
                // .data_size = sizeof(GameEntity),
                .visible = true,
                .sort_index = 0,
            },
            .transform = (Transform2D) {
                .anchor = make_anchor(Anchor_Center),
                .position = (Vector2) { 0, 0 },
                .scale = (Vector2) { 1, 1 },
                .rotation = 0,
                // .rway = Rotation_CW,
            },
            .sprite_entity = (SpriteEntity) {
                .texture = JUST_GLOBAL.known_render_targets.render_screen_texture,
                .tint = WHITE,
                .use_custom_size = true,
                .size = (Vector2) { 800, 800 },
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
        }
    );

    entity1_key = spawn_game_entity(
        &JUST_GLOBAL.entity_store,
        (GameEntity) {
            .entity = (EntityBase) {
                .kind = EntityKind_Circle,
                // .data_size = sizeof(GameEntity),
                .visible = true,
                .sort_index = 20,
                .use_layer_system = false,
                .layers = on_single_layer(2),
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
    
    entity2_key = spawn_game_entity(
        &JUST_GLOBAL.entity_store,
        (GameEntity) {
            .entity = (EntityBase) {
                .kind = EntityKind_Rectangle,
                .data_size = sizeof(GameEntity),
                .visible = true,
                .sort_index = 10,
            },
            .transform = (Transform2D) {
                .anchor = make_anchor(Anchor_Center),
                .position = (Vector2) { 100, 100 },
                .scale = (Vector2) { 100, 100 },
            },
            .rectangle_entity = (RectangleEntity) {
                .color = BLUE,
            },
        }
    );

    EntityKey entity3_key = spawn_game_entity(
        &JUST_GLOBAL.entity_store,
        (GameEntity) {
            .entity = (EntityBase) {
                .kind = EntityKind_Sprite,
                // .data_size = sizeof(GameEntity),
                .visible = true,
                .sort_index = 11,
                .use_layer_system = true,
                .layers = on_single_layer(2),
            },
            .transform = (Transform2D) {
                .anchor = make_anchor(Anchor_Top_Left),
                .position = (Vector2) { 100, 100 },
                .scale = (Vector2) { 1, 1 },
                .rotation = 0,
                // .rway = Rotation_CW,
            },
            .sprite_entity = (SpriteEntity) {
                .texture = test_texture_handle, // new_texture_handle(DEFAULT_TEXTURE_HANDLE_ID), // test_texture_handle,
                .tint = WHITE,
                .use_custom_size = true,
                .size = (Vector2) { 490 * 0.5, 970 * 0.5 },
                .use_custom_source = false,
                // .source = {},
                .flip_x = false,
                .flip_y = true,
                .debug_render_options = {0},
                // .debug_render_options = {
                //     .render_frame = true,
                //     .render_frame_color = RED,
                //     .render_frame_thickness = 10,
                // },
            },
        }
    );

    EntityStore* entity_store = &JUST_GLOBAL.entity_store;

    // TEST
    // {
    //     EntityBase* entity1 = get_entity(entity_store, entity1_key);
    //     EntityKey entity1_key_2 = get_entity_key(entity_store, entity1);
    //     ASSERT(!ENTITY_KEY_IS_INVALID(entity1_key_2));
    //     ASSERT(entity1_key.index == entity1_key_2.index && entity1_key.generation == entity1_key_2.generation);
    // }

    // EntityIter entity_iter = entity_begin_iter(entity_store->data, entity_store->count, entity_store->data_layout.size);
    // EntityBase* entity;
    // while ((entity = next_entity(&entity_iter)) != NULL) {
    //     entity->visible = true;
    //     get_entity_key(entity_store, entity);
    // }
}

int32 CHAPTER__THIS = 0;
JustChapter CHAPTER = {0};

JustChapter* CHAPTER_this() {
    JustAppBuilder app_builder = {0};
 
    APP_BUILDER_ADD__JUST_ENGINE_CORE_SYSTEMS(&app_builder);

    just_app_builder_add_system(&app_builder,
        CORE_STAGE__UPDATE__UPDATE, fn_into_system(SYSTEM_UPDATE_0));
    // just_app_builder_add_system(&app_builder,
    //     CORE_STAGE__UPDATE__POST_UPDATE, fn_into_system(SYSTEM_POST_UPDATE_0));

    CHAPTER = (JustChapter) {
        .chapter_id = CHAPTER__THIS,
        .init_fn = INIT,
        .deinit_fn = NULL,
        .app = just_app_builder_build_app(&app_builder),
    };
    return &CHAPTER;
}

int main() {
    SetTraceLogLevel(LOG_ALL);
    SetTraceLogLevel(LOG_DEBUG);

    JustChapterPtr game_chapters[] = {
        CHAPTER_this(),
    };
    usize n_chapters = ARRAY_LENGTH(game_chapters);

    JustChapters chapters = {
        .initial_chapter = CHAPTER__THIS,
    };
    dynarray_reserve(chapters, .chapters, n_chapters);
    for (usize i = 0; i < n_chapters; i++) {
        dynarray_push_back(chapters, .chapters, game_chapters[i]);
    }
    
    just_engine_run(
        chapters,
        (JustEngineInit) {
            // --------
            .window = {
                .size = { 1000, 1000 },
                .title = "test",
                .clear_color = GRAY,
            },
            // --------
            .execution = {
                .target_fps = 60,
            },
            // --------
            .functions = {
                .entity_store_make_fn = game_make_entity_store,
                .entity_render_list_make_fn = game_make_entity_render_list,
            },
            // --------
            .frame_storage = {
                .size = 1024 * 10,
            },
            // --------
            .threadpool = {
                .nthreads = 4,
                .task_queue_capacity = 100,
            },
            // --------
            .dir = {
                .asset_dir = "",
            },
            // --------
            .render2d = {
                .render_screen_size = { 1000, 1000 },
                .clear_color = WHITE,
                .EXTRACT_RES = &GAME_ENTITY_EXTRACT_RES,
                .RENDER_RES = &GAME_ENTITY_RENDER_RES,
            },
            // --------
            .use_network_subsystem = false,
            // --------
            .use_http_client_subsystem = false,
            // --------
            .vtable = {
                .user_allocator_impl_count = 0,
            },
            // --------
        },
        NULL
    );

    return 0;
}
