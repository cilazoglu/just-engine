
#include "sprite-entity.h"

SpriteEntityRender extract_sprite_entity(SpriteExtractRes RES, Transform2D* transform, SpriteEntity* sprite) {
    Rectangle source;
    Rectangle source_flipped;
    Vector2 size;
    Rectangle destination;
    Vector2 origin;
    float32 rotation;

    Texture texture = *texture_assets_get_texture_or_default(RES.texture_assets, sprite->texture);

    source = sprite->use_custom_source
        ? sprite->source
        : (Rectangle) {0, 0, texture.width, texture.height};
    source_flipped = source;
    source_flipped.width *= sprite->flip_x ? -1 : 1;
    source_flipped.height *= sprite->flip_y ? -1 : 1;
    size = sprite->use_custom_size
        ? sprite->size
        : (Vector2) {source.width, source.height};
    size = Vector2Multiply(size, transform->scale);
    destination = (Rectangle) {
        .x = transform->position.x,
        .y = transform->position.y,
        .width = size.x,
        .height = size.y,
    };
    origin = Vector2Multiply(transform->anchor.origin, size);
    rotation = transform->rotation * transform->rway;

    return (SpriteEntityRender) {
        .texture = texture,
        .tint = sprite->tint,
        .source = source_flipped,
        .destination = destination,
        // .position = transform->position,
        // .size = size,
        .origin = origin,
        .rotation = rotation,
        .sprite_debug_render_options = sprite->debug_render_options,
    };
}

void render_sprite_entity(SpriteEntityRender* sprite_render) {
    DrawTexturePro(
        sprite_render->texture,
        sprite_render->source,
        sprite_render->destination,
        sprite_render->origin,
        sprite_render->rotation,
        sprite_render->tint
    );

    SpriteDebugRenderOptions debug = sprite_render->sprite_debug_render_options;
    if (debug.render_anchor) {
        Vector2 pos = { sprite_render->destination.x, sprite_render->destination.y };
        DrawCircleV(pos, debug.render_anchor_radius, debug.render_anchor_color);
    }
    if (debug.render_frame) {
        Vector2 pos = { sprite_render->destination.x, sprite_render->destination.y };
        pos = Vector2Subtract(pos, sprite_render->origin);
        Rectangle dest = {
            .x = pos.x,
            .y = pos.y,
            .width = sprite_render->destination.width,
            .height = sprite_render->destination.height,
        };
        DrawRectangleLinesEx(dest, debug.render_frame_thickness, debug.render_frame_color);
    }
}
