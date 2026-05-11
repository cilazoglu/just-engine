
#include "memory/dynarray.h"

#include "camera2d.h"

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

void camera2d_begin_render(EntityCamera2D* entity_camera) {
    BeginMode2D(entity_camera->camera);
    JUST_DEV_MARK();
}

void camera2d_end_render() {
    EndMode2D();
    JUST_DEV_MARK();
}
