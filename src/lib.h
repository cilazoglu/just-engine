#include "base.h"
#include "memory/memory.h"
#include "thread/threadpool.h"
#include "assets/asset.h"
#include "assets/assetserver.h"
#include "network/net2.h"
#include "network/httpclient.h"
#include "render/camera.h"
#include "render/camera2d.h"
#include "render/rendertarget.h"
#include "render/entity.h"
#include "ui/justui.h"
#include "execution/execution.h"

typedef struct {
    // --------
    struct {
        URectSize size; // 1920x1080
        const char* title;
        Color clear_color;
    } window;
    // --------
    struct {
        uint32 target_fps;
    } execution;
    // --------
    struct {
        EntityStore_MakeFn entity_store_make_fn;
        EntityRenderList_MakeFn entity_render_list_make_fn;
    } functions;
    // --------
    struct {
        usize size;
    } frame_storage;
    // --------
    struct {
        uint32 nthreads;
        uint32 task_queue_capacity;
    } threadpool;
    // --------
    struct {
        const char* asset_dir;
    } dir;
    // --------
    struct {
        URectSize render_screen_size; // 640x360
        Color clear_color;
        void* EXTRACT_RES;
        void* RENDER_RES;
    } render2d;
    // --------
    bool use_network_subsystem;
    struct {
        NetworkConfig config;
    } network;
    // --------
    bool use_http_client_subsystem;
    // --------
    struct {
        usize user_allocator_impl_count;
    } vtable;
    // --------
} JustEngineInit;

typedef struct {
    // --------
    struct {
        ThreadPoolShutdown shutdown;
    } threadpool;
    // --------
} JustEngineDeinit;

typedef struct {
    float32 delta_time;
    URectSize window_size;
    URectSize render_screen_size;
    // calculated, should match
    Vector2 window_render_screen_ratio; // window / screen_texture
} JustEngineFrameConstants;

typedef struct {
    // --------
    JustEngineFrameConstants frame_constants;
    // --------
    struct {
        bool should_close;
    } execution;
    // --------
    struct {
        EntityStore_MakeFn entity_store_make_fn;
        EntityRenderList_MakeFn entity_render_list_make_fn;
    } functions;
    // --------
    BumpAllocator frame_storage;
    ThreadPool* threadpool;
    // -- Image/Texture
    FileImageServer file_image_server;
    TextureAssets texture_assets;
    Events_TextureAssetEvent texture_asset_events;
    // -- Entity System
    EntityStore entity_store;
    // -- Render
    RenderTargets render_targets;
    struct {
        usize count;
        usize capacity;
        RenderTargetId* order;
    } render_target_order;
    RenderTargetId main_window_render_target;
    RenderTargetId render_screen_target;
    // -- Render2D
    EntityCamera2DStore camera_store;
    // -- UI
    UIElementStore ui_store;
    // --------
} JustEngineGlobalResources;

typedef struct {
    // --------
    // -- Render
    EntityRenderList render_list;
    // -- 
    void* EXTRACT_RES;
    void* RENDER_RES;
    CameraRenderLists crender_lists;
    // --------
} JustEngineGlobalRenderResources;

extern JustEngineGlobalResources JUST_GLOBAL;
extern JustEngineGlobalRenderResources JUST_RENDER_GLOBAL;

void just_engine_init(JustEngineInit init);
void just_engine_deinit(JustEngineDeinit deinit);
void just_engine_run(JustChapters chapters, JustEngineInit init, JustEngineDeinit* deinit);

#define just_engine_mark_exit() (JUST_GLOBAL.execution.should_close = true)

void just_engine_set_render_target_order(RenderTargetId* order, usize count);

// ---------------------------

/**
 * -- STAGES --
 * 
 * INITIALIZE
 * ----------
 * 
 * FRAME_BEGIN
 * 
 * INPUT
 * 
 * PREPARE
 *      PRE_PREPARE
 *      PREAPRE
 *      POST_PREPARE
 * 
 * UPDATE
 *      PRE_UPDATE
 *      UPDATE
 *      POST_UPDATE
 * 
 * RENDER
 *      PREPARE
 *      QUEUE
 *      EXTRACT
 *      RENDER
 * 
 * FRAME_END
 * 
 */

// ---------------------------

void SYSTEM_FRAME_BEGIN_set_delta_time(
    JustEngineFrameConstants* RES_frame_constants
);

void SYSTEM_POST_UPDATE_camera_visibility(
    EntityCamera2DStore* RES_camera_store,
    EntityStore* RES_entity_store
);

void SYSTEM_POST_UPDATE_check_mutated_images(
    TextureAssets* RES_texture_assets,
    Events_TextureAssetEvent* RES_texture_asset_events
);

void SYSTEM_RENDER_EXTRACT_load_textures_for_loaded_or_changed_images(
    TextureAssets* RES_texture_assets,
    Events_TextureAssetEvent* RES_texture_asset_events
);

void SYSTEM_FRAME_END_swap_event_buffers(
    Events_TextureAssetEvent* RES_texture_asset_events
);

void SYSTEM_FRAME_END_reset_temporary_storage(
    TemporaryStorage* RES_temporary_storage
);

// ---------------------------

// -- FRAME_BEGIN --

void JUST_SYSTEM_FRAME_BEGIN_set_delta_time();
void JUST_SYSTEM_FRAME_BEGIN_begin_imgui();

// -- INPUT --

void JUST_SYSTEM_INPUT_handle_input_for_ui_store();

// -- PREPARE --
// -- -- PRE_PREPARE --
// -- -- PREPARE --
// -- -- POST_PREPARE --

// -- UPDATE --
// -- -- PRE_UPDATE --
// -- -- UPDATE --

void JUST_SYSTEM_UPDATE_update_ui_elements();

// -- -- POST_UPDATE --

void JUST_SYSTEM_POST_UPDATE_check_mutated_images();
void JUST_SYSTEM_POST_UPDATE_camera_visibility();

// -- RENDER --
// -- -- PREPARE --

void JUST_SYSTEM_RENDER_PREPARE_render2d();

// -- -- QUEUE --
// -- -- EXTRACT --

void JUST_SYSTEM_RENDER_EXTRACT_load_textures_for_loaded_or_changed_images();
// void JUST_SYSTEM_EXTRACT_RENDER_cull_and_sort_sprites();
void JUST_SYSTEM_RENDER_EXTRACT_render2d();

// -- -- RENDER --

void JUST_SYSTEM_RENDER_RENDER_render2d();

// void JUST_SYSTEM_RENDER_begin_drawing();
// void JUST_SYSTEM_RENDER_render2d();
// void JUST_SYSTEM_RENDER_end_drawing();

// -- -- RENDER_SCREEN --

void JUST_SYSTEM_RENDER_RENDER_SCREEN_begin_drawing();
void JUST_SYSTEM_RENDER_RENDER_SCREEN_render2d();
void JUST_SYSTEM_RENDER_RENDER_SCREEN_draw_ui_elements();
void JUST_SYSTEM_RENDER_RENDER_SCREEN_draw_imgui();
void JUST_SYSTEM_RENDER_RENDER_SCREEN_end_drawing();

// void JUST_SYSTEM_RENDER_SCREEN_begin_drawing();
// void JUST_SYSTEM_RENDER_SCREEN_draw_ui_elements();
// void JUST_SYSTEM_RENDER_SCREEN_draw_imgui();
// void JUST_SYSTEM_RENDER_SCREEN_end_drawing();

// -- FRAME_END --

void JUST_SYSTEM_FRAME_END_swap_event_buffers();
void JUST_SYSTEM_FRAME_END_reset_temporary_storage();

// ---------------------------

void APP_BUILDER_ADD__JUST_ENGINE_CORE_SYSTEMS(JustAppBuilder* app_builder);
void APP_ADD__JUST_ENGINE_CORE_SYSTEMS();
