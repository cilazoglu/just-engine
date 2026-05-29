#pragma once

#include "base.h"
#include "rendertarget.h"
#include "camera.h"

typedef struct {
    CameraId id;
    uint8 sort_index;
    bool disabled;
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

EntityCamera2DStore make_entity_camera2d_store();
void destroy_entity_camera2d_store(EntityCamera2DStore* store);

EntityCamera2D* get_primary_entity_camera2d(EntityCamera2DStore* store);
void set_primary_entity_camera2d(EntityCamera2DStore* store, EntityCamera2D camera);

EntityCamera2D* get_entity_camera2d(EntityCamera2DStore* store, CameraId camera_id);
void add_entity_camera2d(EntityCamera2DStore* store, EntityCamera2D camera);

void sort_entity_camera2d_store(EntityCamera2DStore* store);

// --

void camera2d_begin_render(EntityCamera2D* entity_camera);
void camera2d_end_render();
