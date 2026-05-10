
#include "camera-entity.h"

void entity_render_list_sort_for_each_camera(CameraRenderLists* crender_lists) {
    for (usize i = 0; i < crender_lists->count; i++) {
        CameraRenderList* crender_list = &crender_lists->items[i];
        entity_render_list_sort(&crender_list->render_list);
    }
}
