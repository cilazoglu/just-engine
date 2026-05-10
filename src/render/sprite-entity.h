#pragma once

#include "base.h"
#include "assets/asset.h"
#include "math/transform.h"

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
} SpriteEntity;

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
} SpriteEntityRender;

typedef struct {
    TextureAssets* texture_assets;
} SpriteExtractRes;

SpriteEntityRender extract_sprite_entity(SpriteExtractRes RES, Transform2D* transform, SpriteEntity* sprite);
void render_sprite_entity(SpriteEntityRender* sprite_render);
