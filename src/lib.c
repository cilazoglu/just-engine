#include <stdio.h>
#include <assert.h>

#include "raylib.h"
#include "raymath.h"

#include "raycimgui.h"

#include "justcstd.h"
#include "base.h"
#include "logging.h"
#include "memory/memory.h"
#include "memory/allocator.h"
#include "assets/asset.h"
#include "events/events.h"
#include "render/render2d.h"

#include "lib.h"


// TODO:
//  - Game Base Thread
//      - runs game logic
//  - Render Base Thread (Main thread)
//      - runs render logic
//      - gpu bus io
//      - draw calls


JustEngineGlobalResources JUST_GLOBAL = LAZY_INIT;
JustEngineGlobalRenderResources JUST_RENDER_GLOBAL = LAZY_INIT;

void just_engine_init(JustEngineInit init) {
    RenderTargets render_targets = {0};
    RenderTargetId main_window_target_id = create_render_target_window(&render_targets, init.window.title, init.window.size, init.window.clear_color);
    
    TextureAssets texture_assets = new_texture_assets();

    RenderTargetId render_screen_target_id = create_render_target_texture(&render_targets, init.render2d.render_screen_size, init.render2d.clear_color);
    TextureHandle render_screen_texture_handle = texture_assets_reserve_texture_slot(&texture_assets);
    {
        RenderTarget* render_target = get_render_target(&render_targets, render_screen_target_id);
        SetTextureFilter(render_target->target.texture.texture.texture, TEXTURE_FILTER_POINT);
        texture_assets_put_texture(&texture_assets, render_screen_texture_handle, render_target->target.texture.texture.texture);
    }

    JUST_GLOBAL = (JustEngineGlobalResources) {
        // --
        .frame_constants = {
            .delta_time = 0.0,
            .window_size = init.window.size,
            .render_screen_size = init.render2d.render_screen_size,
            .window_render_screen_ratio = LATER_INIT,
        },
        // --
        .functions = {
            .entity_store_make_fn = init.functions.entity_store_make_fn,
            .entity_render_list_make_fn = init.functions.entity_render_list_make_fn,
        },
        // --
        .frame_storage = make_bump_allocator_with_size(init.frame_storage.size),
        .threadpool = thread_pool_create(init.threadpool.nthreads, init.threadpool.task_queue_capacity),
        // --
        .file_image_server = LATER_INIT,
        .texture_assets = texture_assets,
        .texture_asset_events = TextureAssetEvent__events_create(),
        // --
        .entity_store = init.functions.entity_store_make_fn(),
        // --
        .render_targets = render_targets,
        .render_target_order = LATER_INIT,
        .known_render_targets = {
            .main_window = main_window_target_id,
            .render_screen = render_screen_target_id,
            .render_screen_texture = render_screen_texture_handle,
        },
        // --
        .camera_store = make_entity_camera2d_store(),
        // --
        .ui_store = ui_element_store_new(),
    };

    JUST_GLOBAL.frame_constants.window_render_screen_ratio = (Vector2) {
        .x = (float32) JUST_GLOBAL.frame_constants.window_size.width / JUST_GLOBAL.frame_constants.render_screen_size.width,
        .y = (float32) JUST_GLOBAL.frame_constants.window_size.height / JUST_GLOBAL.frame_constants.render_screen_size.height,
    };

    JUST_GLOBAL.file_image_server = (FileImageServer) {
        .RES_threadpool = JUST_GLOBAL.threadpool,
        .RES_texture_assets = &JUST_GLOBAL.texture_assets,
        .RES_texture_assets_events = &JUST_GLOBAL.texture_asset_events,
        .asset_folder = init.dir.asset_dir,
    };

    dynarray_push_back(JUST_GLOBAL.render_target_order, .order, JUST_GLOBAL.known_render_targets.render_screen);
    dynarray_push_back(JUST_GLOBAL.render_target_order, .order, JUST_GLOBAL.known_render_targets.main_window);

    allocator_vtable_reserve(JUST_ENGINE_ALLOCATOR_IMPL_COUNT + init.vtable.user_allocator_impl_count);
    just_engine__allocator_vtable_add_entries();

    // --

    JUST_RENDER_GLOBAL = (JustEngineGlobalRenderResources) {
        .render_list = LAZY_INIT,
        .crender_lists = LAZY_INIT,
        .EXTRACT_RES = init.render2d.EXTRACT_RES,
        .RENDER_RES = init.render2d.RENDER_RES,
    };
}

void just_engine_deinit(JustEngineDeinit deinit) {
    thread_pool_shutdown(&JUST_GLOBAL.threadpool, deinit.threadpool.shutdown);
}

static JustChapter* find_chapter(JustChapters* chapters, int32 chapter_id) {
    for (usize i = 0; i < chapters->count; i++) {
        JustChapter* ch = chapters->chapters[i];
        if (ch->chapter_id == chapter_id) {
            return ch;
        }
    }
    return NULL;
}

void just_engine_run(JustChapters chapters, JustEngineInit init, JustEngineDeinit* deinit) {
    #pragma region INITIALIZE

    just_engine_init(init); // InitWindow

    SetTargetFPS(init.execution.target_fps);
    SetWindowMonitor(GetMonitorCount()-1);

	rligSetup(true);

    if (init.use_network_subsystem) {
        configure_network_system(init.network.config);
        start_network_thread();
    }

    if (init.use_http_client_subsystem) {
        just_http_global_init_default();
    }

    #pragma endregion INITIALIZE
    
    #pragma region RUN

    JustChapter* current_chapter = find_chapter(&chapters, chapters.initial_chapter);
    if (current_chapter->init_fn) {
        current_chapter->init_fn();
    }

    while (true) {
        just_app_run_once(&current_chapter->app);
        if (JUST_GLOBAL.execution.should_close) {
            break;
        }
        if (current_chapter->end) {
            JustChapter* next_chapter = find_chapter(&chapters, current_chapter->transition_id);

            if (current_chapter->deinit_fn) {
                current_chapter->deinit_fn();
            }
            current_chapter->end = false;
            current_chapter->transition_id = 0;

            if (next_chapter->init_fn) {
                next_chapter->init_fn();
            }
            current_chapter = next_chapter;
        }
    }

    #pragma endregion RUN

    #pragma region DEINITIALIZE

    if (deinit != NULL) {
        just_engine_deinit(*deinit);
    }

    #pragma endregion DEINITIALIZE
}

void just_engine_set_render_target_order(RenderTargetId* order, usize count) {
    dynarray_clear(JUST_GLOBAL.render_target_order);
    dynarray_reserve(JUST_GLOBAL.render_target_order, .order, count);
    std_memcpy(JUST_GLOBAL.render_target_order.order, order, sizeof(order[0]) * count);
    JUST_GLOBAL.render_target_order.count = count;
}

// ---------------------------

void SYSTEM_FRAME_BEGIN_set_delta_time(
    JustEngineFrameConstants* RES_frame_constants
) {
    RES_frame_constants->delta_time = GetFrameTime();
}

void SYSTEM_POST_UPDATE_camera_visibility(
    EntityCamera2DStore* RES_camera_store,
    EntityStore* RES_entity_store
) {
    // TODO
}

void SYSTEM_POST_UPDATE_check_mutated_images(
    TextureAssets* RES_texture_assets,
    Events_TextureAssetEvent* RES_texture_asset_events
) {
    TextureAssetEvent event;
    for (uint32 i = 0; i < TEXTURE_SLOTS; i++) {
        if (RES_texture_assets->image_changed[i]) {
            RES_texture_assets->image_changed[i] = false;
            event = (TextureAssetEvent) {
                .handle = new_texture_handle(i),
                .type = AssetEvent_ImageChanged,
                .consumed = false,
            };
            TextureAssetEvent__events_send_single(RES_texture_asset_events, event);
        }
    }
}

void SYSTEM_RENDER_EXTRACT_load_textures_for_loaded_or_changed_images(
    TextureAssets* RES_texture_assets,
    Events_TextureAssetEvent* RES_texture_asset_events
) {
    static uint32 LOCAL_image_loaded_events_offset = 0;

    EventsIter_TextureAssetEvent events_iter = TextureAssetEvent__events_begin_iter(RES_texture_asset_events, LOCAL_image_loaded_events_offset);

    while (TextureAssetEvent__events_iter_has_next(&events_iter)) {
        TextureAssetEvent event = TextureAssetEvent__events_iter_read_next(&events_iter);
        // TODO: Push Texture Events
        switch (event.type) {
        case AssetEvent_ImageChanged:
            texture_assets_update_texture_unchecked(RES_texture_assets, event.handle);
            break;
        case AssetEvent_ImageLoaded:
            texture_assets_load_texture_uncheched(RES_texture_assets, event.handle);
            break;
        }
    }

    LOCAL_image_loaded_events_offset = TextureAssetEvent__events_iter_end(&events_iter);
}

void SYSTEM_FRAME_END_swap_event_buffers(
    Events_TextureAssetEvent* RES_texture_asset_events
) {
    TextureAssetEvent__events_swap_buffers(RES_texture_asset_events);
}

void SYSTEM_FRAME_END_reset_temporary_storage(
    BumpAllocator* RES_temporary_storage
) {
    reset_bump_allocator(RES_temporary_storage);
}

// ---------------------------

// -- FRAME_BEGIN --

void JUST_SYSTEM_FRAME_BEGIN_set_delta_time() {
    SYSTEM_FRAME_BEGIN_set_delta_time(
        &JUST_GLOBAL.frame_constants
    );
}

void JUST_SYSTEM_FRAME_BEGIN_begin_imgui() {
    rligBegin();
}

// -- INPUT --

void JUST_SYSTEM_INPUT_handle_input_for_ui_store() {
    SYSTEM_INPUT_handle_input_for_ui_store(
        &JUST_GLOBAL.ui_store
    );
}

// -- PREPARE --
// -- -- PRE_PREPARE --
// -- -- PREPARE --
// -- -- POST_PREPARE --

// -- UPDATE --
// -- -- PRE_UPDATE --
// -- -- UPDATE --

void JUST_SYSTEM_UPDATE_update_ui_elements() {
    SYSTEM_UPDATE_update_ui_elements(
        &JUST_GLOBAL.ui_store,
        JUST_GLOBAL.frame_constants.delta_time
    );
}

// -- -- POST_UPDATE --

void JUST_SYSTEM_POST_UPDATE_check_mutated_images() {
    SYSTEM_POST_UPDATE_check_mutated_images(
        &JUST_GLOBAL.texture_assets,
        &JUST_GLOBAL.texture_asset_events
    );
}

void JUST_SYSTEM_POST_UPDATE_camera_visibility() {
    SYSTEM_POST_UPDATE_camera_visibility(
        &JUST_GLOBAL.camera_store,
        &JUST_GLOBAL.entity_store
    );
}

// -- RENDER --
// -- -- PREPARE --

void JUST_SYSTEM_RENDER_PREPARE_render2d() {
    render2d_prepare_camera_render_lists(
        &JUST_GLOBAL.camera_store,
        &JUST_RENDER_GLOBAL.crender_lists,
        JUST_GLOBAL.functions.entity_render_list_make_fn
    );
}

// -- -- QUEUE --
// -- -- EXTRACT --

void JUST_SYSTEM_RENDER_EXTRACT_load_textures_for_loaded_or_changed_images() {
    SYSTEM_RENDER_EXTRACT_load_textures_for_loaded_or_changed_images(
        &JUST_GLOBAL.texture_assets,
        &JUST_GLOBAL.texture_asset_events
    );
}

void JUST_SYSTEM_RENDER_EXTRACT_render2d() {
    render2d_extract_for_each_camera2d(
        JUST_RENDER_GLOBAL.EXTRACT_RES,
        &JUST_RENDER_GLOBAL.crender_lists,
        &JUST_GLOBAL.entity_store,
        &JUST_GLOBAL.camera_store
    );
}

// void JUST_SYSTEM_EXTRACT_RENDER_cull_and_sort_sprites() {
//     SYSTEM_EXTRACT_RENDER_cull_and_sort_sprites(
//         &JUST_GLOBAL.camera_store,
//         &JUST_GLOBAL.sprite_store,
//         &JUST_RENDER_GLOBAL.prepared_render_sprites
//     );
// }

// -- -- SORT --

void JUST_SYSTEM_RENDER_SORT_render2d() {
    render2d_sort_for_each_camera2d(
        &JUST_RENDER_GLOBAL.crender_lists
    );
}

// -- -- RENDER --

void JUST_SYSTEM_RENDER_RENDER_render2d() {
    render2d_render_entities_on_each_render_target_except_window(
        JUST_RENDER_GLOBAL.RENDER_RES,
        &JUST_GLOBAL.render_targets,
        &JUST_RENDER_GLOBAL.crender_lists,
        &JUST_GLOBAL.camera_store,
        JUST_GLOBAL.render_target_order.order,
        JUST_GLOBAL.render_target_order.count
    );
}

// -- -- RENDER_SCREEN --

void JUST_SYSTEM_RENDER_RENDER_SCREEN_begin_drawing() {
    RenderTargets* render_targets = &JUST_GLOBAL.render_targets;
    RenderTargetId main_window_render_target = JUST_GLOBAL.known_render_targets.main_window;

    RenderTarget* render_target = get_render_target(render_targets, main_window_render_target);
    render_target_begin_render(render_target);
}

void JUST_SYSTEM_RENDER_RENDER_SCREEN_render2d() {
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

void JUST_SYSTEM_RENDER_RENDER_SCREEN_draw_ui_elements() {
    SYSTEM_RENDER_draw_ui_elements(
        &JUST_GLOBAL.ui_store
    );
}

void JUST_SYSTEM_RENDER_RENDER_SCREEN_draw_imgui() {
    rligEnd();
}

void JUST_SYSTEM_RENDER_RENDER_SCREEN_end_drawing() {
    render_target_end_render();
}

// void JUST_SYSTEM_RENDER_begin_drawing() {
//     // pass
// }

// void JUST_SYSTEM_RENDER_render2d() {
//     SYSTEM_RENDER_render2d_render_sprites(
//         &JUST_GLOBAL.texture_assets,
//         &JUST_GLOBAL.sprite_store,
//         &JUST_GLOBAL.camera_store,
//         &JUST_RENDER_GLOBAL.prepared_render_sprites
//     );
// }

// void JUST_SYSTEM_RENDER_end_drawing() {
//     // pass
// }

// void JUST_SYSTEM_RENDER_SCREEN_begin_drawing() {
//     BeginDrawing();
//         ClearBackground(JUST_GLOBAL.frame_constants.window_clear_color);
//         Texture texture = JUST_GLOBAL.screen_render_target.texture.texture;
//         URectSize screen_size = JUST_GLOBAL.frame_constants.window_size;
//         DrawTexturePro(
//             texture,
//             (Rectangle) { 0, 0, (float)texture.width, (float)-texture.height },
//             (Rectangle) { 0, 0, (float)screen_size.width, (float)screen_size.height },
//             (Vector2) { 0, 0 },
//             0,
//             WHITE
//         );
// }

// void JUST_SYSTEM_RENDER_SCREEN_draw_ui_elements() {
//     SYSTEM_RENDER_draw_ui_elements(
//         &JUST_GLOBAL.ui_store
//     );
// }

// void JUST_SYSTEM_RENDER_SCREEN_draw_imgui() {
//     rligEnd();
// }

// void JUST_SYSTEM_RENDER_SCREEN_end_drawing() {
//     EndDrawing();
// }

// -- FRAME_END --

void JUST_SYSTEM_FRAME_END_swap_event_buffers() {
    SYSTEM_FRAME_END_swap_event_buffers(
        &JUST_GLOBAL.texture_asset_events
    );
}

void JUST_SYSTEM_FRAME_END_reset_temporary_storage() {
    SYSTEM_FRAME_END_reset_temporary_storage(
        &JUST_GLOBAL.frame_storage
    );
}

// ---------------------------
#if 0
void JUST_ENGINE_RUN_STAGE(JustEngineSystemStage stage) {
    switch (stage) {
        case STAGE__INPUT:
        JUST_LOG_TRACE("-- JustEngine -- Stage: STAGE__INPUT --\n");
        JUST_SYSTEM_INPUT_handle_input_for_ui_store();
        break;

    case STAGE__PREPARE__PRE_PREPARE:
        JUST_LOG_TRACE("-- JustEngine -- Stage: STAGE__PREPARE__PRE_PREPARE --\n");
        JUST_SYSTEM_PRE_PREPARE_set_delta_time();
        break;
    case STAGE__PREPARE__PREPARE:
        JUST_LOG_TRACE("-- JustEngine -- Stage: STAGE__PREPARE__PREPARE --\n");
        break;
    case STAGE__PREPARE__POST_PREPARE:
        JUST_LOG_TRACE("-- JustEngine -- Stage: STAGE__PREPARE__POST_PREPARE --\n");
        break;

    case STAGE__UPDATE__PRE_UPDATE:
        JUST_LOG_TRACE("-- JustEngine -- Stage: STAGE__UPDATE__PRE_UPDATE --\n");
        break;
    case STAGE__UPDATE__UPDATE:
        JUST_LOG_TRACE("-- JustEngine -- Stage: STAGE__UPDATE__UPDATE --\n");
        JUST_SYSTEM_UPDATE_update_ui_elements();
        break;
    case STAGE__UPDATE__POST_UPDATE:
        JUST_LOG_TRACE("-- JustEngine -- Stage: STAGE__UPDATE__POST_UPDATE --\n");
        JUST_SYSTEM_POST_UPDATE_check_mutated_images();
        JUST_SYSTEM_POST_UPDATE_camera_visibility();
        break;
        
    case STAGE__RENDER__QUEUE_RENDER:
        JUST_LOG_TRACE("-- JustEngine -- Stage: STAGE__RENDER__QUEUE_RENDER --\n");
        break;
    case STAGE__RENDER__EXTRACT_RENDER:
        JUST_LOG_TRACE("-- JustEngine -- Stage: STAGE__RENDER__EXTRACT_RENDER --\n");
        JUST_SYSTEM_EXTRACT_RENDER_load_textures_for_loaded_or_changed_images();
        JUST_SYSTEM_EXTRACT_RENDER_cull_and_sort_sprites();
        break;
    case STAGE__RENDER__RENDER:
        JUST_LOG_TRACE("-- JustEngine -- Stage: STAGE__RENDER__RENDER --\n");
        JUST_SYSTEM_RENDER_sorted_sprites();
        JUST_SYSTEM_RENDER_draw_ui_elements();
        break;
    
    case STAGE__FRAME_BOUNDARY:
        JUST_LOG_TRACE("-- JustEngine -- Stage: STAGE__FRAME_BOUNDARY --\n");
        JUST_SYSTEM_FRAME_BOUNDARY_swap_event_buffers();
        JUST_SYSTEM_FRAME_BOUNDARY_reset_temporary_storage();
        break;
    }
}
#endif
// ---------------------------

void APP_BUILDER_ADD__JUST_ENGINE_CORE_SYSTEMS(JustAppBuilder* app_builder) {
    int32 STAGE; 
    
    // =====
    STAGE = CORE_STAGE__FRAME_BEGIN;
    {
        just_app_builder_add_system_with(app_builder, STAGE, fn_into_system(JUST_SYSTEM_FRAME_BEGIN_set_delta_time), (SystemConstraint) { .run_first = true });
        just_app_builder_add_system(app_builder, STAGE, fn_into_system(JUST_SYSTEM_FRAME_BEGIN_begin_imgui));
    }

    // =====
    STAGE = CORE_STAGE__INPUT;
    {
        just_app_builder_add_system(app_builder, STAGE, fn_into_system(JUST_SYSTEM_INPUT_handle_input_for_ui_store));
    }

    // =====
    STAGE = CORE_STAGE__PREPARE__PRE_PREPARE;

    STAGE = CORE_STAGE__PREPARE__PREPARE;

    STAGE = CORE_STAGE__PREPARE__POST_PREPARE;

    // =====
    STAGE = CORE_STAGE__UPDATE__PRE_UPDATE;

    STAGE = CORE_STAGE__UPDATE__UPDATE;
    {
        just_app_builder_add_system(app_builder, STAGE, fn_into_system(JUST_SYSTEM_UPDATE_update_ui_elements));
    }

    STAGE = CORE_STAGE__UPDATE__POST_UPDATE;
    {
        just_app_builder_add_system(app_builder, STAGE, fn_into_system(JUST_SYSTEM_POST_UPDATE_check_mutated_images));
        just_app_builder_add_system(app_builder, STAGE, fn_into_system(JUST_SYSTEM_POST_UPDATE_camera_visibility));
    }
    
    // =====
    STAGE = CORE_STAGE__RENDER__PREPARE;
    {
        just_app_builder_add_system(app_builder, STAGE, fn_into_system(JUST_SYSTEM_RENDER_PREPARE_render2d));
    }
    
    STAGE = CORE_STAGE__RENDER__QUEUE;

    STAGE = CORE_STAGE__RENDER__EXTRACT;
    {
        just_app_builder_add_system(app_builder, STAGE, fn_into_system(JUST_SYSTEM_RENDER_EXTRACT_load_textures_for_loaded_or_changed_images));
        // just_app_builder_add_system(app_builder, STAGE, fn_into_system(JUST_SYSTEM_EXTRACT_RENDER_cull_and_sort_sprites));
        just_app_builder_add_system(app_builder, STAGE, fn_into_system(JUST_SYSTEM_RENDER_EXTRACT_render2d));
    }
    
    STAGE = CORE_STAGE__RENDER__SORT;
    {
        just_app_builder_add_system(app_builder, STAGE, fn_into_system(JUST_SYSTEM_RENDER_SORT_render2d));
    }

    STAGE = CORE_STAGE__RENDER__RENDER;
    {
        just_app_builder_add_system(app_builder, STAGE, fn_into_system(JUST_SYSTEM_RENDER_RENDER_render2d));
        // just_app_builder_add_system_with(app_builder, STAGE, fn_into_system(JUST_SYSTEM_RENDER_begin_drawing), (SystemConstraint) { .run_first = true });
        // just_app_builder_add_system_with(app_builder, STAGE,
        //     fn_into_system(JUST_SYSTEM_RENDER_render2d),
        //     (SystemConstraint) {
        //         .run_after = {
        //             .count = 1,
        //             .systems = { fn_into_system(JUST_SYSTEM_RENDER_begin_drawing) },
        //         },
        //     }
        // );
        // just_app_builder_add_system_with(app_builder, STAGE, fn_into_system(JUST_SYSTEM_RENDER_end_drawing), (SystemConstraint) { .run_last = true });
    }

    STAGE = CORE_STAGE__RENDER__RENDER_SCREEN;
    {
        just_app_builder_add_system_with(app_builder, STAGE, fn_into_system(JUST_SYSTEM_RENDER_RENDER_SCREEN_begin_drawing), (SystemConstraint) { .run_first = true });
        just_app_builder_add_system_with(app_builder, STAGE,
            fn_into_system(JUST_SYSTEM_RENDER_RENDER_SCREEN_render2d),
            (SystemConstraint) {
                .run_after = {
                    .count = 1,
                    .systems = { fn_into_system(JUST_SYSTEM_RENDER_RENDER_SCREEN_begin_drawing) },
                },
            }
        );
        just_app_builder_add_system_with(app_builder, STAGE,
            fn_into_system(JUST_SYSTEM_RENDER_RENDER_SCREEN_draw_ui_elements),
            (SystemConstraint) {
                .run_after = {
                    .count = 1,
                    .systems = { fn_into_system(JUST_SYSTEM_RENDER_RENDER_SCREEN_render2d) },
                },
            }
        );
        just_app_builder_add_system_with(app_builder, STAGE,
            fn_into_system(JUST_SYSTEM_RENDER_RENDER_SCREEN_draw_imgui),
            (SystemConstraint) {
                .run_after = {
                    .count = 1,
                    .systems = { fn_into_system(JUST_SYSTEM_RENDER_RENDER_SCREEN_draw_ui_elements) },
                },
            }
        );
        just_app_builder_add_system_with(app_builder, STAGE, fn_into_system(JUST_SYSTEM_RENDER_RENDER_SCREEN_end_drawing), (SystemConstraint) { .run_last = true });
    }
    
    // =====
    STAGE = CORE_STAGE__FRAME_END;
    {
        just_app_builder_add_system(app_builder, STAGE, fn_into_system(JUST_SYSTEM_FRAME_END_swap_event_buffers));
        just_app_builder_add_system(app_builder, STAGE, fn_into_system(JUST_SYSTEM_FRAME_END_reset_temporary_storage));
    }
}

void APP_ADD__JUST_ENGINE_CORE_SYSTEMS() {
    APP_BUILDER_ADD__JUST_ENGINE_CORE_SYSTEMS(GLOBAL_APP_BUILDER());
}
