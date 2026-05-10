#pragma once

#include "base.h"

typedef enum {
    RENDER_TARGET_WINDOW = 0,
    RENDER_TARGET_TEXTURE,
} RenderTargetType;

typedef usize WindowId;
typedef usize RenderTargetId;

// Limited to single instance in raylib which supports single window
typedef struct {
    WindowId window_id;
    const char* title;
    URectSize window_size;
    Color clear_color;
} RenderTargetWindow;

typedef struct {
    RenderTexture texture;
    URectSize texture_size;
    Color clear_color;
} RenderTargetTexture;

typedef struct {
    RenderTargetType type;
    union {
        RenderTargetWindow window;
        RenderTargetTexture texture;
    } target;
} RenderTarget;

typedef struct {
    usize count;
    usize capacity;
    // RenderTargetWindow* target_windows;
    // RenderTargetTexture* target_textures;
    RenderTarget* targets;
} RenderTargets;

RenderTargetId create_render_target_window(RenderTargets* render_targets, const char* title, URectSize window_size, Color clear_color);
RenderTargetId create_render_target_texture(RenderTargets* render_targets, URectSize texture_size, Color clear_color);
RenderTarget* get_render_target(RenderTargets* render_targets, RenderTargetId id);

// --

void render_target_begin_render(RenderTarget* render_target);
void render_target_end_render();
