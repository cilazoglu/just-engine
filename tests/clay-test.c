#include "justengine.h"
#include "introspect_gen__justengine.h"

#define FONT_ID_BODY_24 0
#define FONT_ID_BODY_16 1

#define CLAY_GRAY               (Clay_Color) { 43, 41, 51, 255 }
#define CLAY_CONTENT_BG_GRAY    (Clay_Color) { 90, 90, 90, 255 }

uint32 test = 0;
usize frame_number = 0;
usize create_frame_number = 0;

void button_on_hover(
    Clay_ElementId elementId,
    Clay_PointerData pointerData,
    intptr_t userData
) {
    char* state = "";
    switch (pointerData.state) {
    case CLAY_POINTER_DATA_PRESSED_THIS_FRAME:
        state = "CLAY_POINTER_DATA_PRESSED_THIS_FRAME";
        break;
    case CLAY_POINTER_DATA_PRESSED:
        state = "CLAY_POINTER_DATA_PRESSED";
        break;
    case CLAY_POINTER_DATA_RELEASED_THIS_FRAME:
        state = "CLAY_POINTER_DATA_RELEASED_THIS_FRAME";
        printf("create_frame_number: %llu, frame_number: %llu ", create_frame_number, frame_number);
        break;
    case CLAY_POINTER_DATA_RELEASED:
        state = "CLAY_POINTER_DATA_RELEASED";
        break;
    }

    String element_id_str = clay_string_to_string(elementId.stringId);
    printf("element: %s, state: %s\n", element_id_str.cstr, state);
    test = 1;
}

void button_on_click(
    Clay_ElementId element_id,
    JustClay_PointerState pointer_state,
    void* user_data
) {
    if (pointer_state.just_clicked) {
        JUST_LOG_ERROR("Clicked: %s\n", element_id.stringId.chars);
    }
    else {
        // just_pretty_print(JustClay_PointerState)(NULL, &pointer_state);
    }
}

void RenderTabSelectButton(Clay_String text) {
    CLAY((Clay_ElementDeclaration) {
        .id = CLAY_SID(text),
        .layout = {
            // .padding = CLAY_PADDING_ALL(16),
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_GROW(0),
            },
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
                .y = CLAY_ALIGN_Y_CENTER,
            },
        },
        .backgroundColor = Clay_Hovered() ? printf("Hovered: %u\n", test) ? RAYLIB_COLOR_TO_CLAY_COLOR(GREEN) : RAYLIB_COLOR_TO_CLAY_COLOR(GREEN) : RAYLIB_COLOR_TO_CLAY_COLOR(RAYWHITE),
        .cornerRadius = CLAY_CORNER_RADIUS(5),
    }) {
        if (text.chars[0] == 'S') { // "Sound"
            create_frame_number = frame_number;
        }
        Clay_OnHover(button_on_hover, (uintptr_t) NULL);
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_24,
            .fontSize = 24,
            .textColor = RAYLIB_COLOR_TO_CLAY_COLOR(BLACK),
            .textAlignment = CLAY_TEXT_ALIGN_CENTER,
        }));
    };
}

bool JUST_Clay_Hovered(void);
void Just_Clay_OnHover(void (*onHoverFunction)(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData), intptr_t userData);

void JustRenderTabSelectButton(Clay_String text) {
    bool sw = text.chars[0] == 'S'; // Sound
    // Clay_ElementId id = CLAY_SID(text);
    // print_string(clay_string_to_string(text));
    // printf(": %u\n", id.id);
    JCLAY((Clay_ElementDeclaration) {
        .id = JUSTCLAY_INTERRACT(CLAY_SID(text)),
        .layout = {
            // .padding = CLAY_PADDING_ALL(16),
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_GROW(0),
            },
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
                .y = CLAY_ALIGN_Y_CENTER,
            },
        },
        .backgroundColor = JustClay_This_OnHover() ? (RAYLIB_COLOR_TO_CLAY_COLOR(GREEN)) : (RAYLIB_COLOR_TO_CLAY_COLOR(RAYWHITE)),
        //.backgroundColor = Clay_Hovered() ? printf("Hovered: %u\n", test) ? RAYLIB_COLOR_TO_CLAY_COLOR(GREEN) : RAYLIB_COLOR_TO_CLAY_COLOR(GREEN) : RAYLIB_COLOR_TO_CLAY_COLOR(RAYWHITE),
        .cornerRadius = CLAY_CORNER_RADIUS(5),
        .image = (Clay_ImageElementConfig) {
            .imageData = NULL,
        },
    }) {
        // JUST_Clay_Hovered() +
        Clay_Context* context = Clay_GetCurrentContext();
        // Just_Clay_OnHover(button_on_hover, NULL);
        JustClay_InterractWithPointer(button_on_click, NULL);
        bool on_hover = JustClay_OnHover();
        CLAY_TEXT(text, CLAY_TEXT_CONFIG((Clay_TextElementConfig) {
            .fontId = FONT_ID_BODY_24,
            .fontSize = 24,
            .textColor = on_hover ? RAYLIB_COLOR_TO_CLAY_COLOR(BLACK) : RAYLIB_COLOR_TO_CLAY_COLOR(WHITE),
            .textAlignment = CLAY_TEXT_ALIGN_CENTER,
        }));
    };
}

typedef struct {
    Clay_Id id;
    char* name;
} ClayButton;

typedef enum {
    MAIN_MENU__OPTION__PLAY = 0,
    MAIN_MENU__OPTION__EDIT = 1,
    MAIN_MENU__OPTION__EXIT = 2,
    COUNT__MAIN_MENU__OPTION,
} MainMenuOptions;

static ClayButton MainMenuOptions_Buttons[] = {
    [MAIN_MENU__OPTION__PLAY] = {
        .id = CLAY_NULLID,
        .name = "Play",
    },
    [MAIN_MENU__OPTION__EDIT] = {
        .id = CLAY_NULLID,
        .name = "Edit",
    },
    [MAIN_MENU__OPTION__EXIT] = {
        .id = CLAY_NULLID,
        .name = "Exit",
    },
};

void handle_main_menu_buttons(bool* set_should_close) {
    {
        Clay_Id id = MainMenuOptions_Buttons[MAIN_MENU__OPTION__PLAY].id;
        if (id != CLAY_NULLID) {
            JustClay_ElementState play_button = JustClay_GetElementState(id);
            if (play_button.pointer.just_clicked) {
                // PLAY
            }
        }
    }
    {
        Clay_Id id = MainMenuOptions_Buttons[MAIN_MENU__OPTION__EDIT].id;
        if (id != CLAY_NULLID) {
            JustClay_ElementState edit_button = JustClay_GetElementState(id);
            if (edit_button.pointer.just_clicked) {
                // EDIT
            }
        }
    }
    {
        Clay_Id id = MainMenuOptions_Buttons[MAIN_MENU__OPTION__EXIT].id;
        if (id != CLAY_NULLID) {
            JustClay_ElementState exit_button = JustClay_GetElementState(id);
            if (exit_button.pointer.just_clicked) {
                // EXIT
                *set_should_close = true;
            }
        }
    }
}

void MainMenuButton(MainMenuOptions option_i) {
    char* option_name = MainMenuOptions_Buttons[option_i].name;
    Clay_String option_str = string_to_clay_string(string_from_cstr(option_name));

    JCLAY((Clay_ElementDeclaration) {
        .id = JUSTCLAY_INTERRACT(CLAY_SID_LOCAL(option_str)),
        .layout = {
            .sizing = {
                .width = CLAY_SIZING_PERCENT(0.8),
                .height = CLAY_SIZING_GROW(0),
            },
            // .sizing = {
            //     .width = CLAY_SIZING_PERCENT(0.8),
            //     .height = CLAY_SIZING_PERCENT(),
            // },
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
                .y = CLAY_ALIGN_Y_CENTER,
            },
        },
        .backgroundColor = JustClay_This_OnHover() ? (RAYLIB_COLOR_TO_CLAY_COLOR(GREEN)) : (RAYLIB_COLOR_TO_CLAY_COLOR(RAYWHITE)),
        // .cornerRadius = CLAY_CORNER_RADIUS(5),
    }) {
        MainMenuOptions_Buttons[option_i].id = JustClay_InterractWithPointer(button_on_click, NULL);
        bool on_hover = JustClay_OnHover();
        CLAY_TEXT(option_str, CLAY_TEXT_CONFIG((Clay_TextElementConfig) {
            .fontId = FONT_ID_BODY_24,
            .fontSize = 24,
            .textColor = on_hover ? RAYLIB_COLOR_TO_CLAY_COLOR(RAYWHITE) : RAYLIB_COLOR_TO_CLAY_COLOR(BLACK),
            .textAlignment = CLAY_TEXT_ALIGN_CENTER,
        }));
    };
}

Clay_RenderCommandArray main_menu_ui() {
    Clay_BeginLayout();

    CLAY((Clay_ElementDeclaration) {
        .id = CLAY_ID("OuterContainer"),
        .backgroundColor = CLAY_GRAY,
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_GROW(0),
            },
            // .padding = CLAY_PADDING_ALL(500),
            .padding = {
                .top = 200,
                .bottom = 200,
                .left = 800,
                .right = 800,
            },
            .childGap = 0,
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
                .y = CLAY_ALIGN_Y_CENTER,
            },
        },
        // .border = test_border_black,
    }) {
        CLAY((Clay_ElementDeclaration) {
            .id = CLAY_ID("TabsWindow"),
            .backgroundColor = CLAY_CONTENT_BG_GRAY,
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {
                    .width = CLAY_SIZING_PERCENT(1.0),
                    .height = CLAY_SIZING_PERCENT(1.0),
                },
                .padding = CLAY_PADDING_ALL(10),
                .childGap = 10,
                .childAlignment = {
                    .x = CLAY_ALIGN_X_CENTER,
                    .y = CLAY_ALIGN_Y_CENTER,
                },
            },
            // .border = test_border_red,
        }) {
            JCLAY((Clay_ElementDeclaration) {
                .id = CLAY_SID_LOCAL(CLAY_STRING("image")),
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_GROW(0),
                        .height = CLAY_SIZING_PERCENT(0.5),
                    },
                    .childAlignment = {
                        .x = CLAY_ALIGN_X_CENTER,
                        .y = CLAY_ALIGN_Y_CENTER,
                    },
                },
                .backgroundColor = RAYLIB_COLOR_TO_CLAY_COLOR(RAYWHITE),
            }) {
                CLAY_TEXT(CLAY_STRING("[IMAGE]"), CLAY_TEXT_CONFIG((Clay_TextElementConfig) {
                    .fontId = FONT_ID_BODY_24,
                    .fontSize = 24,
                    .textColor = RAYLIB_COLOR_TO_CLAY_COLOR(BLACK),
                    .textAlignment = CLAY_TEXT_ALIGN_CENTER,
                }));
            };

            for (int32 main_menu_option_i = 0; main_menu_option_i < COUNT__MAIN_MENU__OPTION; main_menu_option_i++) {
                MainMenuButton(main_menu_option_i);
            }
        }
    }

    return Clay_EndLayout();
}

Clay_RenderCommandArray settings_page_ui() {
    Clay_BeginLayout();

    Clay_BorderElementConfig test_border_black = {
        .color = RAYLIB_COLOR_TO_CLAY_COLOR(BLACK),
        .width = CLAY_BORDER_ALL(3),
    };
    Clay_BorderElementConfig test_border_red = {
        .color = RAYLIB_COLOR_TO_CLAY_COLOR(RED),
        .width = CLAY_BORDER_ALL(3),
    };

    CLAY((Clay_ElementDeclaration) {
        .id = CLAY_ID("OuterContainer"),
        .backgroundColor = CLAY_GRAY,
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_GROW(0),
            },
            .padding = CLAY_PADDING_ALL(16),
            .childGap = 16,
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
                .y = CLAY_ALIGN_Y_CENTER,
            },
        },
        // .border = test_border_black,
    }) {
        CLAY((Clay_ElementDeclaration) {
            .id = CLAY_ID("TabsWindow"),
            .backgroundColor = CLAY_CONTENT_BG_GRAY,
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {
                    .width = CLAY_SIZING_PERCENT(0.6),
                    .height = CLAY_SIZING_PERCENT(0.6),
                },
                .padding = CLAY_PADDING_ALL(8),
                .childGap = 4,
            },
            // .border = test_border_red,
        }) {
            CLAY((Clay_ElementDeclaration) {
                .id = CLAY_ID("TabSelection_1"),
                .backgroundColor = CLAY_CONTENT_BG_GRAY,
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = {
                        .width = CLAY_SIZING_GROW(0),
                        .height = CLAY_SIZING_PERCENT(0.08),
                    },
                    // .padding = CLAY_PADDING_ALL(8),
                    .childGap = 8,
                },
                // .border = test_border_black,
            }) {
                RenderTabSelectButton(CLAY_STRING("General"));
                RenderTabSelectButton(CLAY_STRING("Graphics"));
                RenderTabSelectButton(CLAY_STRING("Sound"));
            };
            CLAY((Clay_ElementDeclaration) {
                .id = CLAY_ID("TabSelection_2"),
                .backgroundColor = CLAY_CONTENT_BG_GRAY,
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = {
                        .width = CLAY_SIZING_GROW(0),
                        .height = CLAY_SIZING_PERCENT(0.08),
                    },
                    // .padding = CLAY_PADDING_ALL(8),
                    .childGap = 8,
                },
                // .border = test_border_black,
            }) {
                JustRenderTabSelectButton(CLAY_STRING("General 2"));
                JustRenderTabSelectButton(CLAY_STRING("Graphics 2"));
                JustRenderTabSelectButton(CLAY_STRING("Sound 2"));
            };
        };
    }

    return Clay_EndLayout();
}

int main() {
    SET_LOG_LEVEL(LOG_LEVEL_ERROR);
    // SET_LOG_LEVEL(LOG_LEVEL_WARN);
    // SET_LOG_LEVEL(LOG_LEVEL_TRACE);

    InitWindow(1980, 1200, "Clay");
    SetTargetFPS(60);
    
    TextureAssets RES_TEXTURE_ASSETS = new_texture_assets();
    Events(TextureAssetEvent) RES_TEXTURE_ASSET_EVENTS = events_create(TextureAssetEvent)();

    ThreadPool* RES_THREAD_POOL = thread_pool_create(4, 100);
    FileImageServer RES_FILE_IMAGE_SERVER = {
        .RES_threadpool = RES_THREAD_POOL,
        .RES_texture_assets = &RES_TEXTURE_ASSETS,
        .RES_texture_assets_events = &RES_TEXTURE_ASSET_EVENTS,
        .asset_folder = "./test-assets",
    };

    FontList RES_FONT_LIST = {0};
    dynarray_reserve(RES_FONT_LIST, .fonts, 2);
    {
        Font font_body_24 = LoadFontEx("./test-assets/Roboto-Regular.ttf", 24, 0, 400);
        SetTextureFilter(font_body_24.texture, TEXTURE_FILTER_BILINEAR);
        dynarray_push_back(RES_FONT_LIST, .fonts, font_body_24);
    }
    {
        Font font_body_16 = LoadFontEx("./test-assets/Roboto-Regular.ttf", 16, 0, 400);
        SetTextureFilter(font_body_16.texture, TEXTURE_FILTER_BILINEAR);
        dynarray_push_back(RES_FONT_LIST, .fonts, font_body_16);
    }

    initialize_justclay(&RES_FONT_LIST);
    // Clay_SetDebugModeEnabled(true);

    bool should_close = WindowShouldClose();
    while (!should_close) {
        frame_number++;
        float32 delta_time = GetFrameTime();

        bool mouse_down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        Vector2 mouse_position = GetMousePosition();
        Vector2 mouse_wheel_delta = GetMouseWheelMoveV();

        SYSTEM_PRE_PREPARE_reinit_justclay_if_necessary();
        SYSTEM_PRE_PREPARE_justclay_set_state(mouse_position, mouse_down);

        SYSTEM_POST_PREPARE_justclay_update_scroll_containers(mouse_wheel_delta, delta_time);

        Clay_BeginLayout();

        Clay_Sizing layoutExpand = {
            .width = CLAY_SIZING_GROW(0),
            .height = CLAY_SIZING_GROW(0)
        };

        Clay_Color contentBackgroundColor = { 90, 90, 90, 255 };

        // Clay_RenderCommandArray clay_render_commands = settings_page_ui();
        Clay_RenderCommandArray clay_render_commands = main_menu_ui();

        handle_main_menu_buttons(&should_close);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        SYSTEM_RENDER_justclay_ui(
            &RES_TEXTURE_ASSETS,
            RES_FONT_LIST,
            clay_render_commands
        );

        EndDrawing();

        should_close |= WindowShouldClose();
    }

    return 0;
}