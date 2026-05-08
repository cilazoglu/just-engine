
#include "justengine.h"

typedef enum {
    EntityKind_Circle,
    EntityKind_Rectangle,
} EntityKind;

typedef struct {
    Color color;
} CircleEntity;

typedef struct {
    Color color;
    bool render_frame;
    Color frame_color;
} RectangleEntity;

typedef struct {
    EntityBase entity;
    union {
        CircleEntity circle_entity;
        RectangleEntity rectangle_entity;
    };
} GameEntity;

typedef struct {
    RenderEntityBase render_entity;
    Color color;
} GameRenderEntity;

EntityKey spawn_game_entity(EntityStore* store, GameEntity entity) {
    return spawn_entity(store, entity);
}

void extract_entity(EntityBase* entity, RenderEntityBase* set_render_entity) {
    GameEntity* game_entity = (void*)entity;
    GameRenderEntity* set_game_render_entity = (void*)set_render_entity;

    *set_game_render_entity = (GameRenderEntity) {
        .render_entity = (RenderEntityBase) {
            .kind = game_entity->entity.kind,
            .data_size = game_entity->entity.data_size,
            .sort_index = game_entity->entity.sort_index,
        },
        .color = game_entity->entity.kind == EntityKind_Circle ? game_entity->circle_entity.color : game_entity->rectangle_entity.color
    };
}

void render_entity(RenderEntityBase* render_entity) {
    GameRenderEntity* game_render_entity = (void*)render_entity;

    Color color = game_render_entity->color;
    switch (game_render_entity->render_entity.kind) {
    case EntityKind_Circle:
        DrawCircle(100, 100, 50, color);
        break;
    case EntityKind_Rectangle:
        DrawRectangle(100, 100, 100, 100, color);
        break;
    }
}

int main() {
    EntityRenderPair ENTITY_RENDER = make_uniform_entity_render_pair(
        GameEntity, GameRenderEntity, 1, 1, extract_entity, render_entity
    );
    EntityStore* store = &ENTITY_RENDER.store;
    EntityRenderList* render_list = &ENTITY_RENDER.render_list;
    
    InitWindow(1000, 1000, "Entity Render System Test");
    SetTargetFPS(60);

    // INIT
    EntityKey entity1_key = spawn_game_entity(
        store,
        (GameEntity) {
            .entity = (EntityBase) {
                .kind = EntityKind_Circle,
                // .data_size = sizeof(GameEntity),
                .visible = false,
                .sort_index = 20,
            },
            // .transform = (Transform2D) {
            //     .anchor = make_anchor(Anchor_Center),
            //     .position = (Vector2) { 100, 100 },
            //     .size = (Vector2) { 10, 10 },
            //     .scale = (Vector2) { 1, 1 },
            // },
            .circle_entity = (CircleEntity) {
                .color = GREEN,
            },
        }
    );
    
    EntityKey entity2_key = spawn_game_entity(
        store,
        (GameEntity) {
            .entity = (EntityBase) {
                .kind = EntityKind_Rectangle,
                // .data_size = sizeof(GameEntity),
                .visible = false,
                .sort_index = 10,
            },
            // .transform = (Transform2D) {
            //     .anchor = make_anchor(Anchor_Center),
            //     .position = (Vector2) { 100, 100 },
            //     .size = (Vector2) { 10, 10 },
            //     .scale = (Vector2) { 1, 1 },
            // },
            .rectangle_entity = (RectangleEntity) {
                .color = BLUE,
                .render_frame = true,
                .frame_color = MAGENTA,
            },
        }
    );

    // TEST
    {
        EntityBase* entity1 = get_entity(store, entity1_key);
        EntityKey entity1_key_2 = get_entity_key(store, entity1);
        ASSERT(!ENTITY_KEY_IS_INVALID(entity1_key_2));
        ASSERT(entity1_key.index == entity1_key_2.index && entity1_key.generation == entity1_key_2.generation);
    }

    EntityIter entity_iter = entity_begin_iter(store->data, store->count, store->data_layout.size);
    EntityBase* entity;
    while ((entity = next_entity(&entity_iter)) != NULL) {
        // entity->visible = true;
    }

    while (!WindowShouldClose()) {
        // INPUT
        bool toggle_visible_entity1 = IsKeyPressed(KEY_A);
        bool toggle_visible_entity2 = IsKeyPressed(KEY_S);
        bool swap_sort_indices = IsKeyPressed(KEY_SPACE);

        // UPDATE
        if (toggle_visible_entity1) {
            EntityBase* entity1 = get_entity(store, entity1_key);
            entity1->visible ^= 1;
        }
        if (toggle_visible_entity2) {
            EntityBase* entity2 = get_entity(store, entity2_key);
            entity2->visible ^= 1;
        }
        if (swap_sort_indices) {
            EntityBase* entity1 = get_entity(store, entity1_key);
            EntityBase* entity2 = get_entity(store, entity2_key);
            uint8 temp = entity1->sort_index;
            entity1->sort_index = entity2->sort_index;
            entity2->sort_index = temp;
        }

        // RENDER_EXTRACT
        entity_render_list_extract(render_list, store);

        // RENDER_SORT
        entity_render_list_sort(render_list);

        // RENDER
        BeginDrawing();
            ClearBackground(WHITE);
            entity_render_list_render(render_list);
        EndDrawing();
    }

    destroy_entity_render_pair(&ENTITY_RENDER);

    return 0;
}
