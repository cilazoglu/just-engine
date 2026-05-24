#include "justengine.h"

typedef struct {
    EntityBase entity_base;
    Vector2 pos;
} TestEntity;

typedef struct {
    RenderEntityBase render_entity_base;
    Rectangle render_rect;
} TestRenderEntity;

void extract_entity(void* EXTRACT_RES, EntityBase* entity, RenderEntityBase* set_render_entity) {
    TestEntity* test_entity = (TestEntity*)entity;
    TestRenderEntity* set_test_render_entity = (TestRenderEntity*)set_render_entity;
    set_test_render_entity->render_rect = (Rectangle) {
        .x = test_entity->pos.x,
        .y = test_entity->pos.y,
        .width = 100,
        .height = 100,
    };
}

void render_entity(void* RENDER_RES, RenderEntityBase* render_entity) {
    TestRenderEntity* test_render_entity = (TestRenderEntity*)render_entity;

    EntityKey entity_key = test_render_entity->render_entity_base.entity_key;
    Rectangle rect = test_render_entity->render_rect;
    JUST_LOG_INFO(
        "render: "
        "entity_key = { .index = %llu, .generation = %llu }, "
        "rect = { .x = %0.2f, .y = %0.2f, .width = %0.2f, .height = %0.2f }\n",
        entity_key.index, entity_key.generation,
        rect.x, rect.y, rect.width, rect.height
    );
}

typedef struct {
    usize count;
    usize capacity;
    EntityKey* entities;
} EntityList;

int main() {
    EntityStore STORE = make_uniform_entity_store(TestEntity, 16);
    EntityStore* store = &STORE;
    EntityList list = {0};

    for (usize _c = 0; _c < 10; _c++) {
        for (usize add_i = 0; add_i < 100; add_i++) {
            TestEntity entity = {
                .pos = Vector2_From(add_i),
            };
            EntityKey key = spawn_entity(store, entity);
            EntityKey data_key = store->indices[key.index];
            if (ENTITY_KEY_IS_INVALID(key)) {
                ASSERT(false);
            }
            if (ENTITY_KEY_IS_INVALID(data_key)) {
                ASSERT(false);
            }
            dynarray_push_back(list, .entities, key);
        }
        for (usize del_i = 0; del_i < 50; del_i++) {
            usize i = list.count-1 - del_i;
            EntityKey key = list.entities[i];
            despawn_entity(store, key);
            EntityKey data_key = store->indices[key.index];
            if (ENTITY_KEY_IS_INVALID(data_key)) {
                ASSERT(false);
            }
        }
        list.count -= 50;
    }

    return 0;
}

