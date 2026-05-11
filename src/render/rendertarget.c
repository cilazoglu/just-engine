
#include "memory/dynarray.h"

#include "rendertarget.h"

RenderTargetId create_render_target_window(RenderTargets* render_targets, const char* title, URectSize window_size, Color clear_color) {
    static bool is_single_window_init = false; // RAYLIB
    static usize single_window_target_id = 0;
    
    if (!is_single_window_init) {
        is_single_window_init = true;
        single_window_target_id = render_targets->count;
        
        InitWindow(window_size.width, window_size.height, title);

        RenderTarget target = {
            .type = RENDER_TARGET_WINDOW,
            .target.window = (RenderTargetWindow) {
                .window_id = 0, // NOTE: normally actual window id for drawing into it
                .window_size = window_size,
                .title = title,
                .clear_color = clear_color,
            },
        };
        dynarray_push_back(*render_targets, .targets, target);
    }

    return single_window_target_id;
}

RenderTargetId create_render_target_texture(RenderTargets* render_targets, URectSize texture_size, Color clear_color) {
    usize render_texture_id = render_targets->count;
    
    RenderTexture render_texture = LoadRenderTexture(texture_size.width, texture_size.height);
    RenderTarget target = {
        .type = RENDER_TARGET_TEXTURE,
        .target.texture = (RenderTargetTexture) {
            .texture = render_texture,
            .texture_size = texture_size,
            .clear_color = clear_color,
        },
    };
    dynarray_push_back(*render_targets, .targets, target);

    return render_texture_id;
}

RenderTarget* get_render_target(RenderTargets* render_targets, RenderTargetId id) {
    ASSERT(id < render_targets->count);
    return &render_targets->targets[id];
}

// --

static RenderTargetType RENDER2D_CURRENT_RENDER_TARGET_TYPE = -1;

void render_target_begin_render(RenderTarget* render_target) {
    RENDER2D_CURRENT_RENDER_TARGET_TYPE = render_target->type;

    switch (RENDER2D_CURRENT_RENDER_TARGET_TYPE) {
    case RENDER_TARGET_WINDOW:
        RenderTargetWindow* render_target_window = &render_target->target.window;
        JUST_DEV_MARK();
        BeginDrawing();
        ClearBackground(render_target_window->clear_color);
        break;
    case RENDER_TARGET_TEXTURE:
        RenderTargetTexture* render_target_texture = &render_target->target.texture;
        BeginTextureMode(render_target_texture->texture);
        ClearBackground(render_target_texture->clear_color);
        break;
    default:
        PANIC("Unsupported RenderTargetType.");
    }
}

void render_target_end_render() {
    switch (RENDER2D_CURRENT_RENDER_TARGET_TYPE) {
    case RENDER_TARGET_WINDOW:
        JUST_DEV_MARK();
        EndDrawing();
        JUST_DEV_MARK();
        break;
    case RENDER_TARGET_TEXTURE:
        EndTextureMode();
        break;
    default:
        PANIC("Unsupported RenderTargetType.");
    }

    RENDER2D_CURRENT_RENDER_TARGET_TYPE = -1;
}
