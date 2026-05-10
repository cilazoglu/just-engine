
#include "memory/dynarray.h"

#include "render2d.h"

void render2d_prepare_camera_render_lists(
    EntityCamera2DStore* camera_store,
    CameraRenderLists* crender_lists,
    EntityRenderList_MakeFn entity_render_list_make_fn
) {
    if (crender_lists->count < camera_store->count) {
        usize reserve_count = camera_store->count - crender_lists->count;
        dynarray_reserve(*crender_lists, .items, reserve_count);

        for (usize i = 0; i < camera_store->count; i++) {
            crender_lists->items[i].camera_id = camera_store->cameras[i].id;
        }
        for (usize i = crender_lists->count; i < camera_store->count; i++) {
            // crender_lists->items[i].render_list = make_uniform_entity_render_list(GameRenderEntity, 10, extract_entity, render_entity);
            crender_lists->items[i].render_list = entity_render_list_make_fn();
        }
        crender_lists->count = camera_store->count;
    }
}

void entity_render_list_extract_with_camera2d(
    void* EXTRACT_RES,
    EntityRenderList* render_list,
    EntityStore* store,
    CameraId camera_id,
    EntityCamera2D* entity_camera
) {
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
    entity_render_list_extract_base(render_list, store, EXTRACT_RES);
}

void render2d_extract_for_each_camera2d(
    void* EXTRACT_RES,
    CameraRenderLists* crender_lists,
    EntityStore* store,
    EntityCamera2DStore* camera_store
) {
    for (usize i = 0; i < crender_lists->count; i++) {
        CameraRenderList* crender_list = &crender_lists->items[i];
        CameraId camera_id = crender_list->camera_id;
        EntityCamera2D* camera = get_entity_camera2d(camera_store, camera_id);
        entity_render_list_extract_with_camera2d(EXTRACT_RES, &crender_list->render_list, store, camera_id, camera);
    }
}

void render2d_sort_for_each_camera2d(
    CameraRenderLists* crender_lists
) {
    entity_render_list_sort_for_each_camera(crender_lists);
}

void entity_render_for_each_camera2d(
    void* RENDER_RES,
    CameraRenderLists* crender_lists,
    EntityCamera2DStore* camera_store,
    RenderTargetId active_render_target
) {
    for (usize i = 0; i < crender_lists->count; i++) {
        CameraRenderList* crender_list = &crender_lists->items[i];
        EntityCamera2D* camera = get_entity_camera2d(camera_store, crender_list->camera_id);
        if (camera->target == active_render_target) {
            camera2d_begin_render(camera);
                entity_render_list_render(&crender_list->render_list, RENDER_RES);
            camera2d_end_render();
        }
    }
}

void render2d_render_entities_on_each_render_target_except_window(
    void* RENDER_RES,
    RenderTargets* render_targets,
    CameraRenderLists* crender_lists,
    EntityCamera2DStore* camera_store,
    // --
    RenderTargetId* render_target_order,
    usize render_target_order_count
) {
    for (usize i = 0; i < render_target_order_count; i++) {
        RenderTargetId active_render_target = render_target_order[i];
        RenderTarget* render_target = get_render_target(render_targets, active_render_target);
        if (render_target->type == RENDER_TARGET_WINDOW) {
            // NOTE: handle window drawing separately on raylib
            continue;
        }
        render_target_begin_render(render_target);
            entity_render_for_each_camera2d(
                RENDER_RES,
                crender_lists,
                camera_store,
                active_render_target
            );
        render_target_end_render();
    }
}

void render2d_render_entities_on_main_window_render_target(
    void* RENDER_RES,
    RenderTargets* render_targets,
    CameraRenderLists* crender_lists,
    EntityCamera2DStore* camera_store,
    RenderTargetId main_window_render_target
) {
    RenderTarget* render_target = get_render_target(render_targets, main_window_render_target);
    render_target_begin_render(render_target);
        entity_render_for_each_camera2d(
            RENDER_RES,
            crender_lists,
            camera_store,
            main_window_render_target
        );
    render_target_end_render();
}
