#pragma once

#include "base.h"
#include "assets/asset.h"
#include "math/transform.h"
#include "physics/collision.h"

typedef struct {
    bool render_anchor;
    uint32 render_anchor_radius;
    Color render_anchor_color;
    bool render_frame;
    uint32 render_frame_thickness;
    Color render_frame_color;
} SpriteDebugRenderOptions;

typedef struct {
    TextureHandle texture;
    Color tint;
    bool use_custom_size;
    Vector2 size;
    bool use_custom_source;
    Rectangle source;
    bool flip_x;
    bool flip_y;
    SpriteDebugRenderOptions debug_render_options;
} Sprite;

typedef struct {
    Texture texture;
    Color tint;
    Rectangle source;
    Rectangle destination;
    // Vector2 position;
    // Vector2 size;
    Vector2 origin;
    float32 rotation;
    SpriteDebugRenderOptions sprite_debug_render_options;
} SpriteRender;

typedef struct {
    TextureAssets* texture_assets;
} SpriteExtractRes;

SpriteRender extract_sprite_entity(SpriteExtractRes RES, Transform2D* transform, Sprite* sprite);
void render_sprite_entity(SpriteRender* sprite_render);

static inline Vector2 sprite_get_size(Sprite* sprite, TextureAssets* texture_assets) {
    if (sprite->use_custom_size) return sprite->size;
    if (sprite->use_custom_source) return rectangle_size(sprite->source).as_vec;
    if (texture_assets != NULL) {
        Texture* texture = texture_assets_get_texture_unchecked(texture_assets, sprite->texture);
        return (Vector2) { .x = texture->width, .y = texture->height };
    }
    PANIC("Sprite size is broken\n");
}

static inline SpaceShift sprite_get_space_shift(Transform2D* transform, Sprite* sprite, TextureAssets* texture_assets) {
    Vector2 sprite_size = sprite_get_size(sprite, texture_assets);
    return (SpaceShift) {
        .translation = Vector2Subtract(
            transform->position,
            Vector2Multiply(
                transform->scale,
                Vector2Multiply(sprite_size, transform->anchor.origin)
            )
        ),
        .scale = transform->scale,
    };
}
