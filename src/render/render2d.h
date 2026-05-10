#pragma once

#include "assets/asset.h"

#include "camera.h"
#include "entity.h"
#include "camera-entity.h"

#include "camera2d.h"

void render2d_prepare_camera_render_lists(
    EntityCamera2DStore* camera_store,
    CameraRenderLists* crender_lists,
    EntityRenderList_MakeFn entity_render_list_make_fn
);

void entity_render_list_extract_with_camera2d(
    void* EXTRACT_RES,
    EntityRenderList* render_list,
    EntityStore* store,
    CameraId camera_id,
    EntityCamera2D* entity_camera
);

void render2d_extract_for_each_camera2d(
    void* EXTRACT_RES,
    CameraRenderLists* crender_lists,
    EntityStore* store,
    EntityCamera2DStore* camera_store
);

void render2d_sort_for_each_camera2d(
    CameraRenderLists* crender_lists
);

void entity_render_for_each_camera2d(
    void* RENDER_RES,
    CameraRenderLists* crender_lists,
    EntityCamera2DStore* camera_store,
    RenderTargetId active_render_target
);

void render2d_render_entities_on_each_render_target_except_window(
    void* RENDER_RES,
    RenderTargets* render_targets,
    CameraRenderLists* crender_lists,
    EntityCamera2DStore* camera_store,
    // --
    RenderTargetId* render_target_order,
    usize render_target_order_count
);

void render2d_render_entities_on_main_window_render_target(
    void* RENDER_RES,
    RenderTargets* render_targets,
    CameraRenderLists* crender_lists,
    EntityCamera2DStore* camera_store,
    RenderTargetId main_window_render_target
);
