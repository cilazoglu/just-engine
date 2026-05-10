#pragma once

#include "camera.h"
#include "entity.h"

typedef struct {
    CameraId camera_id;
    EntityRenderList render_list;
} CameraRenderList;

typedef struct {
    usize count; // == camera_count
    usize capacity;
    CameraRenderList* items;
} CameraRenderLists;

void entity_render_list_sort_for_each_camera(CameraRenderLists* crender_lists);
