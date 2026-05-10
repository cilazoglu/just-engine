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

void render2d_extract_for_each_camera2d(
    void* EXTRACT_RES,
    CameraRenderLists* crender_lists,
    EntityStore* store,
    EntityCamera2DStore* camera_store
);

void render2d_extract_for_each_camera2d(
    void* EXTRACT_RES,
    CameraRenderLists* crender_lists,
    EntityStore* store,
    EntityCamera2DStore* camera_store
);

void render2d_sort_for_each_camera2d(
    CameraRenderLists* crender_lists
) {
    entity_render_list_sort_for_each_camera(crender_lists);
};

void render2d_render_entities_on_each_render_target(
    void* RENDER_RES,
    RenderTargets* render_targets,
    CameraRenderLists* crender_lists,
    EntityCamera2DStore* camera_store,
    // --
    RenderTargetId* render_target_order,
    usize render_target_order_count
);
