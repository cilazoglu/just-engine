#define CLAY_IMPLEMENTATION
#include "clay.h"

#include "base.h"
#include "memory/memory.h"
#include "memory/juststring.h"

#include "justclay.h"

Clay_String string_to_clay_string(String string) {
    return (Clay_String) {
        .isStaticallyAllocated = false,
        .length = string.count,
        .chars = string.str,
    };
}

String clay_string_to_string(Clay_String clay_string) {
    String string = string_with_capacity(clay_string.length);
    string_nappend_cstr(&string, (char*) clay_string.chars, clay_string.length);
    return string;
}

__IMPL_____EVENT_SYSTEM__ACCESS_SINGLE_THREADED(JustClay_PointerEvent);

static JustClay_ElementStore JUSTCLAY_ELEMENT_STORE = STRUCT_ZERO_INIT;
static Events(JustClay_PointerEvent) JUSTCLAY_POINTER_EVENTS = STARTUP_INIT; // events_create(JustClay_PointerEvent)()

static void just_internal_consume_pointer_events() {
    events_swap_buffers(JustClay_PointerEvent)(&JUSTCLAY_POINTER_EVENTS);

    for (usize i = 0; i < JUSTCLAY_ELEMENT_STORE.count; i++) {
        JustClay_ElementKV* elemkv = &JUSTCLAY_ELEMENT_STORE.items[i];
        JustClay_PointerState* elem_pointer_state = &elemkv->element.state.pointer;
        // Reset State
        elemkv->element.state.is_modified = false;
        elemkv->element.state.element_id.id = CLAY_NULLID;
        // elem_pointer_state->on_hover = false;   // true: ok, false: ok
        // elem_pointer_state->on_press = false;   // true: ok, false: ok
        elem_pointer_state->just_begin_hover = false;   // true: ok, false: ok
        elem_pointer_state->just_end_hover = false;     // true: ok, false: ok
        elem_pointer_state->just_pressed = false;       // true: ok, false: ok
        elem_pointer_state->just_released = false;      // true: ok, false: ok
        elem_pointer_state->just_clicked = false;       // true: ok, false: ok
    }

    static usize event_offset = 0;
    EventsIter(JustClay_PointerEvent) events = events_begin_iter(JustClay_PointerEvent)(&JUSTCLAY_POINTER_EVENTS, event_offset);

    Clay_ElementId* hovered_element_id = &JUSTCLAY_ELEMENT_STORE.hovered_element_id;
    Clay_ElementId* pressed_element_id = &JUSTCLAY_ELEMENT_STORE.pressed_element_id;

    Clay_ElementId new_hovered_element_id = {0};
    new_hovered_element_id.id = CLAY_NULLID;

    while(events_iter_has_next(JustClay_PointerEvent)(&events)) {
        JustClay_PointerEvent event = events_iter_consume_next(JustClay_PointerEvent)(&events);

        JustClay_Element* elem = JustClay_FindElement(event.element_id.id);
        JustClay_PointerState* pointer_state = &elem->state.pointer;

        elem->state.is_modified = true;
        elem->state.element_id = event.element_id;

        switch (event.interraction_type) {
        case JUSTCLAY_POINTER_INTERRACTION_ONHOVER: {
            if (!pointer_state->on_hover) {
                pointer_state->just_begin_hover = true;
            }
            pointer_state->on_hover = true;
            new_hovered_element_id = event.element_id;
        } break;
        case JUSTCLAY_POINTER_INTERRACTION_PRESSED: {
            pointer_state->just_pressed = true;
            pointer_state->on_press = true;
            pressed_element_id->id = event.element_id.id;
        } break;
        case JUSTCLAY_POINTER_INTERRACTION_RELEASED: {
            if (event.element_id.id == pressed_element_id->id) {
                pointer_state->on_press = false;
                pointer_state->just_clicked = true;
            }
            else {
                JustClay_Element* pressed_elem = JustClay_FindElement(pressed_element_id->id);
                pressed_elem->state.is_modified = true;
                pressed_elem->state.element_id = *pressed_element_id;
                pressed_elem->state.pointer.on_press = false;
            }
            pointer_state->just_released = true;
            pressed_element_id->id = CLAY_NULLID;
        } break;
        default:
            PANIC("Unknown JustClay_PointerInterractionType\n");
        }
    }
    event_offset = events_iter_end(JustClay_PointerEvent)(&events);

    Clay_Id old_hovered_id = hovered_element_id->id;
    Clay_Id new_hovered_id = new_hovered_element_id.id;
    if (old_hovered_id != CLAY_NULLID && old_hovered_id != new_hovered_id) {
        JustClay_Element* elem = JustClay_FindElement(old_hovered_id);
        JustClay_PointerState* pointer_state = &elem->state.pointer;
        elem->state.is_modified = true;
        elem->state.element_id = *hovered_element_id;
        pointer_state->on_hover = false;
        pointer_state->just_end_hover = true;
    }
    *hovered_element_id = new_hovered_element_id;

    // TODO: improve
    for (usize i = 0;  i < JUSTCLAY_ELEMENT_STORE.count; i++) {
        JustClay_ElementKV* elemkv = &JUSTCLAY_ELEMENT_STORE.items[i];
        if (elemkv->element.state.is_modified) {
            JustClay_OnPointerInterract_UserData fn = elemkv->element.just_on_pointer_interract_user_data;
            if (fn.on_interract_fn != NULL) (fn.on_interract_fn)(elemkv->element.state.element_id, elemkv->element.state.pointer, fn.user_data);
        }
    }
}

static void just_internal_on_hover(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t _userData_null) {
    JustClay_ElementKV* this_elemkv = NULL;

    JustClay_PointerEvent hover_event = {
        .consumed = false,
        .element_id = elementId,
        .interraction_type = JUSTCLAY_POINTER_INTERRACTION_ONHOVER,
    };
    events_send_single(JustClay_PointerEvent)(&JUSTCLAY_POINTER_EVENTS, hover_event);

    JustClay_PointerEvent pointer_event = {
        .consumed = false,
        .element_id = elementId,
        .interraction_type = 0,
    };
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        pointer_event.interraction_type = JUSTCLAY_POINTER_INTERRACTION_PRESSED;
        events_send_single(JustClay_PointerEvent)(&JUSTCLAY_POINTER_EVENTS, pointer_event);
    }
    else if (pointerData.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
        pointer_event.interraction_type = JUSTCLAY_POINTER_INTERRACTION_RELEASED;
        events_send_single(JustClay_PointerEvent)(&JUSTCLAY_POINTER_EVENTS, pointer_event);
    }
}

Clay_Id JustClay_InterractWithPointer(JustClay_OnPointerInterractFn on_pointer_interract_fn, void* user_data) {
    // -- Copied from Clay_OnHover --
    Clay_Context* context = Clay_GetCurrentContext();
    if (context->booleanWarnings.maxElementsExceeded) {
        return CLAY_NULLID;
    }
    Clay_LayoutElement *openLayoutElement = Clay__GetOpenLayoutElement(); // 1966619372, 1528724250, 340503119
    if (openLayoutElement->id == 0) {
        Clay__GenerateIdForAnonymousElement(openLayoutElement);
    }
    // ------------------------------

    JustClay_ElementKV* this_elemkv = NULL;

    for (usize i = 0; i < JUSTCLAY_ELEMENT_STORE.count; i++) {
        JustClay_ElementKV* elemkv = &JUSTCLAY_ELEMENT_STORE.items[i];
        if (elemkv->id == openLayoutElement->id) {
            this_elemkv = elemkv;
            break;
        }
    }
    if (this_elemkv == NULL) {
        JustClay_ElementKV elemkv = {
            .id = openLayoutElement->id,
            .element = {0},
        };
        dynarray_push_back(JUSTCLAY_ELEMENT_STORE, .items, elemkv);
        this_elemkv = &JUSTCLAY_ELEMENT_STORE.items[JUSTCLAY_ELEMENT_STORE.count-1];
    }

    this_elemkv->element.just_on_pointer_interract_user_data = (JustClay_OnPointerInterract_UserData) {
        .user_data = user_data,
        .on_interract_fn = on_pointer_interract_fn,
    };

    Clay_OnHover(just_internal_on_hover, (intptr_t) NULL);
    return this_elemkv->id;
}

void Just_Clay_OnHover(void (*onHoverFunction)(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData), intptr_t userData) {
    Clay_Context* context = Clay_GetCurrentContext();
    if (context->booleanWarnings.maxElementsExceeded) {
        return;
    }
    Clay_LayoutElement *openLayoutElement = Clay__GetOpenLayoutElement(); // 1966619372, 1528724250, 340503119
    if (openLayoutElement->id == 0) {
        Clay__GenerateIdForAnonymousElement(openLayoutElement);
    }
    Clay_LayoutElementHashMapItem *hashMapItem = Clay__GetHashMapItem(openLayoutElement->id);
    hashMapItem->onHoverFunction = onHoverFunction;
    hashMapItem->hoverFunctionUserData = userData;
}

static Clay_Id OPEN_LAYOUT_ELEMENT_REAL_ID = CLAY_NULLID;

Clay_ElementId JUSTCLAY_INTERRACT(Clay_ElementId element_id) {
    OPEN_LAYOUT_ELEMENT_REAL_ID = element_id.id;
    return element_id;
}

void JustClay__OpenElement() {
    OPEN_LAYOUT_ELEMENT_REAL_ID = CLAY_NULLID;
    Clay__OpenElement();
}

void JustClay__CloseElement() {
    OPEN_LAYOUT_ELEMENT_REAL_ID = CLAY_NULLID;
    Clay__CloseElement();
}

void JustClay__ConfigureOpenElement(const Clay_ElementDeclaration config) {
    OPEN_LAYOUT_ELEMENT_REAL_ID = CLAY_NULLID;
    Clay__ConfigureOpenElement(config);
}

static bool __JustClay_Check(JustClay_PointerStateEnum state, Clay_Id check_id) {
    for (usize i = 0; i < JUSTCLAY_ELEMENT_STORE.count; i++) {
        JustClay_ElementKV* elemkv = &JUSTCLAY_ELEMENT_STORE.items[i];
        if (elemkv->id == check_id) {
            switch (state) {
            case JUSTCLAY_POINTER_ONHOVER:          return elemkv->element.state.pointer.on_hover;
            case JUSTCLAY_POINTER_ONPRESS:          return elemkv->element.state.pointer.on_press;
            case JUSTCLAY_POINTER_JUSTBEGINHOVER:   return elemkv->element.state.pointer.just_begin_hover;
            case JUSTCLAY_POINTER_JUSTENDHOVER:     return elemkv->element.state.pointer.just_end_hover;
            case JUSTCLAY_POINTER_JUSTPRESSED:      return elemkv->element.state.pointer.just_pressed;
            case JUSTCLAY_POINTER_JUSTRELEASED:     return elemkv->element.state.pointer.just_released;
            case JUSTCLAY_POINTER_JUSTCLICKED:      return elemkv->element.state.pointer.just_clicked;
            default: return false; // unknown
            }
        }
    }
    return false;
}

bool JustClay_This_Check(JustClay_PointerStateEnum state) {
    Clay_Context* context = Clay_GetCurrentContext();
    if (context->booleanWarnings.maxElementsExceeded) {
        return false;
    }

    Clay_Id check_id = OPEN_LAYOUT_ELEMENT_REAL_ID;
    return __JustClay_Check(state, check_id);
}

bool JustClay_Check(JustClay_PointerStateEnum state) {
    // -- Copied from Clay_Hovered --
    Clay_Context* context = Clay_GetCurrentContext();
    if (context->booleanWarnings.maxElementsExceeded) {
        return false;
    }
    Clay_LayoutElement* openLayoutElement = Clay__GetOpenLayoutElement(); // 3721769855, 4024129418, 947972312
    // If the element has no id attached at this point, we need to generate one
    if (openLayoutElement->id == 0) {                                     // 1966619372, 1528724250, 340503119
        Clay__GenerateIdForAnonymousElement(openLayoutElement);
    }
    // ------------------------------

    Clay_Id check_id = openLayoutElement->id;
    return __JustClay_Check(state, check_id);
}

bool JUST_Clay_Hovered(void) {
    Clay_Context* context = Clay_GetCurrentContext();
    if (context->booleanWarnings.maxElementsExceeded) {
        return false;
    }
    Clay_LayoutElement *openLayoutElement = Clay__GetOpenLayoutElement(); // 3721769855, 4024129418, 947972312
    // If the element has no id attached at this point, we need to generate one
    if (openLayoutElement->id == 0) {
        Clay__GenerateIdForAnonymousElement(openLayoutElement);
    }
    for (int32_t i = 0; i < context->pointerOverIds.length; ++i) {
        if (Clay_ElementIdArray_Get(&context->pointerOverIds, i)->id == openLayoutElement->id) {
            return true;
        }
    }
    return false; // 947972312
}

bool JustClay_This_OnHover() {
    return JustClay_This_Check(JUSTCLAY_POINTER_ONHOVER);
}
bool JustClay_This_OnPress() {
    return JustClay_This_Check(JUSTCLAY_POINTER_ONPRESS);
}
bool JustClay_This_JustBeginHover() {
    return JustClay_This_Check(JUSTCLAY_POINTER_JUSTBEGINHOVER);
}
bool JustClay_This_JustEndHover() {
    return JustClay_This_Check(JUSTCLAY_POINTER_JUSTENDHOVER);
}
bool JustClay_This_JustPressed() {
    return JustClay_This_Check(JUSTCLAY_POINTER_JUSTPRESSED);
}
bool JustClay_This_JustReleased() {
    return JustClay_This_Check(JUSTCLAY_POINTER_JUSTRELEASED);
}
bool JustClay_This_JustClicked() {
    return JustClay_This_Check(JUSTCLAY_POINTER_JUSTCLICKED);
}

bool JustClay_OnHover() {
    return JustClay_Check(JUSTCLAY_POINTER_ONHOVER);
}
bool JustClay_OnPress() {
    return JustClay_Check(JUSTCLAY_POINTER_ONPRESS);
}
bool JustClay_JustBeginHover() {
    return JustClay_Check(JUSTCLAY_POINTER_JUSTBEGINHOVER);
}
bool JustClay_JustEndHover() {
    return JustClay_Check(JUSTCLAY_POINTER_JUSTENDHOVER);
}
bool JustClay_JustPressed() {
    return JustClay_Check(JUSTCLAY_POINTER_JUSTPRESSED);
}
bool JustClay_JustReleased() {
    return JustClay_Check(JUSTCLAY_POINTER_JUSTRELEASED);
}
bool JustClay_JustClicked() {
    return JustClay_Check(JUSTCLAY_POINTER_JUSTCLICKED);
}

JustClay_Element* JustClay_FindElement(Clay_Id id) {
    for (usize i = 0; i < JUSTCLAY_ELEMENT_STORE.count; i++) {
        JustClay_ElementKV* elemkv = &JUSTCLAY_ELEMENT_STORE.items[i];
        if (elemkv->id == id) {
            return &elemkv->element;
        }
    }
    return NULL;
}

JustClay_ElementState JustClay_GetElementState(Clay_Id id) {
    for (usize i = 0; i < JUSTCLAY_ELEMENT_STORE.count; i++) {
        JustClay_ElementKV* elemkv = &JUSTCLAY_ELEMENT_STORE.items[i];
        if (elemkv->id == id) {
            return elemkv->element.state;
        }
    }
    PANIC("id not found\n");
    return (JustClay_ElementState){0};
}

static bool reinitialize_clay = false;
static void* justclay_arena_memory;

void justclay_handle_errors(Clay_ErrorData errorData) {
    JUST_LOG_WARN("%s\n", errorData.errorText.chars);
    if (errorData.errorType == CLAY_ERROR_TYPE_ELEMENTS_CAPACITY_EXCEEDED) {
        reinitialize_clay = true;
        Clay_SetMaxElementCount(Clay_GetMaxElementCount() * 2);
    }
    else if (errorData.errorType == CLAY_ERROR_TYPE_TEXT_MEASUREMENT_CAPACITY_EXCEEDED) {
        reinitialize_clay = true;
        Clay_SetMaxMeasureTextCacheWordCount(Clay_GetMaxMeasureTextCacheWordCount() * 2);
    }
}

Clay_Dimensions justclay_measure_text(Clay_StringSlice text, Clay_TextElementConfig* config, void* user_data) {
    FontList* font_list = (FontList*) user_data;

    // Measure string size for Font
    Clay_Dimensions textSize = { 0 };

    float32 max_text_width = 0.0f;
    float32 line_text_width = 0;
    int32 max_line_char_count = 0;
    int32 line_char_count = 0;

    float32 text_height = config->fontSize;
    Font font_to_use = font_list->fonts[config->fontId];
    // Font failed to load, likely the fonts are in the wrong place relative to the execution dir.
    // RayLib ships with a default font, so we can continue with that built in one. 
    if (!font_to_use.glyphs) {
        font_to_use = GetFontDefault();
    }

    float32 scale_factor = config->fontSize / (float32)font_to_use.baseSize;

    for (int32 i = 0; i < text.length; ++i, line_char_count++) {
        if (text.chars[i] == '\n') {
            max_text_width = fmax(max_text_width, line_text_width);
            max_line_char_count = CLAY__MAX(max_line_char_count, line_char_count);
            line_text_width = 0;
            line_char_count = 0;
            continue;
        }
        int32 index = text.chars[i] - 32;
        if (font_to_use.glyphs[index].advanceX != 0) line_text_width += font_to_use.glyphs[index].advanceX;
        else line_text_width += (font_to_use.recs[index].width + font_to_use.glyphs[index].offsetX);
    }

    max_text_width = fmax(max_text_width, line_text_width);
    max_line_char_count = CLAY__MAX(max_line_char_count, line_char_count);

    textSize.width = max_text_width * scale_factor + (line_char_count * config->letterSpacing);
    textSize.height = text_height;

    return textSize;
}

void initialize_justclay(FontList* font_list) {
    JUSTCLAY_POINTER_EVENTS = events_create(JustClay_PointerEvent)();

    uint64_t totalMemorySize = Clay_MinMemorySize();

    justclay_arena_memory = std_malloc(totalMemorySize);
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, justclay_arena_memory);

    Clay_Initialize(clayMemory, (Clay_Dimensions) { (float32)GetScreenWidth(), (float32)GetScreenHeight() }, (Clay_ErrorHandler) { justclay_handle_errors, 0 });
    Clay_SetMeasureTextFunction(justclay_measure_text, font_list);
}

void SYSTEM_PRE_PREPARE_reinit_justclay_if_necessary() {
    if (reinitialize_clay) {
        uint64_t totalMemorySize = Clay_MinMemorySize();

        std_free(justclay_arena_memory);
        justclay_arena_memory = std_malloc(totalMemorySize);
        Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, justclay_arena_memory);

        Clay_Initialize(clayMemory, (Clay_Dimensions) { (float32)GetScreenWidth(), (float32)GetScreenHeight() }, (Clay_ErrorHandler) { justclay_handle_errors, 0 });
        reinitialize_clay = false;
    }
}

void SYSTEM_PRE_PREPARE_justclay_set_state(
    Vector2 mouse_position,
    bool mouse_down
) {
    Clay_Vector2 clay_mouse_position = RAYLIB_VECTOR2_TO_CLAY_VECTOR2(mouse_position);
    Clay_SetPointerState(clay_mouse_position, mouse_down);
    just_internal_consume_pointer_events();
    Clay_SetLayoutDimensions((Clay_Dimensions) { (float)GetScreenWidth(), (float)GetScreenHeight() });
}

void SYSTEM_POST_PREPARE_justclay_update_scroll_containers(
    Vector2 mouse_wheel_delta,
    float32 delta_time
) {
    Clay_Vector2 clay_mouse_wheel_delta = RAYLIB_VECTOR2_TO_CLAY_VECTOR2(mouse_wheel_delta);
    Clay_UpdateScrollContainers(true, clay_mouse_wheel_delta, delta_time);
    // TODO: remove old and stale elements
}

static String TEMP_STRING;

void SYSTEM_RENDER_justclay_ui(
    TextureAssets* RES_TEXTURE_ASSETS,
    FontList RES_FONT_LIST,
    Clay_RenderCommandArray renderCommands
) {
    for (int32 j = 0; j < renderCommands.length; j++) {
        Clay_RenderCommand *renderCommand = Clay_RenderCommandArray_Get(&renderCommands, j);
        Clay_BoundingBox boundingBox = {
            .x = roundf(renderCommand->boundingBox.x),
            .y = roundf(renderCommand->boundingBox.y),
            .width = roundf(renderCommand->boundingBox.width),
            .height = roundf(renderCommand->boundingBox.height),
        };
        
        switch (renderCommand->commandType) {
        case CLAY_RENDER_COMMAND_TYPE_TEXT: {
            Clay_TextRenderData *textData = &renderCommand->renderData.text;
            Font fontToUse = RES_FONT_LIST.fonts[textData->fontId];

            // Raylib uses standard C strings so isn't compatible with cheap slices, we need to clone the string to append null terminator
            clear_string(&TEMP_STRING);
            dynarray_reserve(TEMP_STRING, .str, textData->stringContents.length);
            string_nappend_cstr(&TEMP_STRING, (char*) textData->stringContents.chars, textData->stringContents.length);

            DrawTextEx(
                fontToUse,
                TEMP_STRING.cstr,
                (Vector2){boundingBox.x, boundingBox.y},
                (float)textData->fontSize,
                (float)textData->letterSpacing,
                CLAY_COLOR_TO_RAYLIB_COLOR(textData->textColor)
            );

            break;
        }
        case CLAY_RENDER_COMMAND_TYPE_IMAGE: {
            TextureHandle texture_handle = *(TextureHandle*)renderCommand->renderData.image.imageData;
            Texture* texture = texture_assets_get_texture_or_default(RES_TEXTURE_ASSETS, texture_handle);

            Clay_Color tintColor = renderCommand->renderData.image.backgroundColor;
            if (tintColor.r == 0 && tintColor.g == 0 && tintColor.b == 0 && tintColor.a == 0) {
                tintColor = (Clay_Color) { 255, 255, 255, 255 };
            }
            DrawTexturePro(
                *texture,
                (Rectangle) { 0, 0, texture->width, texture->height },
                CLAY_RECTANGLE_TO_RAYLIB_RECTANGLE(boundingBox),
                (Vector2) {},
                0,
                CLAY_COLOR_TO_RAYLIB_COLOR(tintColor)
            );
            break;
        }
        case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START: {
            BeginScissorMode((int)roundf(boundingBox.x), (int)roundf(boundingBox.y), (int)roundf(boundingBox.width), (int)roundf(boundingBox.height));
            break;
        }
        case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END: {
            EndScissorMode();
            break;
        }
        case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
            Clay_RectangleRenderData *config = &renderCommand->renderData.rectangle;
            if (config->cornerRadius.topLeft > 0) {
                float radius = (config->cornerRadius.topLeft * 2) / (float)((boundingBox.width > boundingBox.height) ? boundingBox.height : boundingBox.width);
                DrawRectangleRounded(
                    CLAY_RECTANGLE_TO_RAYLIB_RECTANGLE(boundingBox),
                    radius,
                    8,
                    CLAY_COLOR_TO_RAYLIB_COLOR(config->backgroundColor)
                );
            } else {
                DrawRectangle(boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height, CLAY_COLOR_TO_RAYLIB_COLOR(config->backgroundColor));
            }
            break;
        }
        case CLAY_RENDER_COMMAND_TYPE_BORDER: {
            Clay_BorderRenderData *config = &renderCommand->renderData.border;
            // Left border
            if (config->width.left > 0) {
                DrawRectangle((int)roundf(boundingBox.x), (int)roundf(boundingBox.y + config->cornerRadius.topLeft), (int)config->width.left, (int)roundf(boundingBox.height - config->cornerRadius.topLeft - config->cornerRadius.bottomLeft), CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
            }
            // Right border
            if (config->width.right > 0) {
                DrawRectangle((int)roundf(boundingBox.x + boundingBox.width - config->width.right), (int)roundf(boundingBox.y + config->cornerRadius.topRight), (int)config->width.right, (int)roundf(boundingBox.height - config->cornerRadius.topRight - config->cornerRadius.bottomRight), CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
            }
            // Top border
            if (config->width.top > 0) {
                DrawRectangle((int)roundf(boundingBox.x + config->cornerRadius.topLeft), (int)roundf(boundingBox.y), (int)roundf(boundingBox.width - config->cornerRadius.topLeft - config->cornerRadius.topRight), (int)config->width.top, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
            }
            // Bottom border
            if (config->width.bottom > 0) {
                DrawRectangle((int)roundf(boundingBox.x + config->cornerRadius.bottomLeft), (int)roundf(boundingBox.y + boundingBox.height - config->width.bottom), (int)roundf(boundingBox.width - config->cornerRadius.bottomLeft - config->cornerRadius.bottomRight), (int)config->width.bottom, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
            }
            if (config->cornerRadius.topLeft > 0) {
                DrawRing((Vector2) { roundf(boundingBox.x + config->cornerRadius.topLeft), roundf(boundingBox.y + config->cornerRadius.topLeft) }, roundf(config->cornerRadius.topLeft - config->width.top), config->cornerRadius.topLeft, 180, 270, 10, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
            }
            if (config->cornerRadius.topRight > 0) {
                DrawRing((Vector2) { roundf(boundingBox.x + boundingBox.width - config->cornerRadius.topRight), roundf(boundingBox.y + config->cornerRadius.topRight) }, roundf(config->cornerRadius.topRight - config->width.top), config->cornerRadius.topRight, 270, 360, 10, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
            }
            if (config->cornerRadius.bottomLeft > 0) {
                DrawRing((Vector2) { roundf(boundingBox.x + config->cornerRadius.bottomLeft), roundf(boundingBox.y + boundingBox.height - config->cornerRadius.bottomLeft) }, roundf(config->cornerRadius.bottomLeft - config->width.bottom), config->cornerRadius.bottomLeft, 90, 180, 10, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
            }
            if (config->cornerRadius.bottomRight > 0) {
                DrawRing((Vector2) { roundf(boundingBox.x + boundingBox.width - config->cornerRadius.bottomRight), roundf(boundingBox.y + boundingBox.height - config->cornerRadius.bottomRight) }, roundf(config->cornerRadius.bottomRight - config->width.bottom), config->cornerRadius.bottomRight, 0.1, 90, 10, CLAY_COLOR_TO_RAYLIB_COLOR(config->color));
            }
            break;
        }
        case CLAY_RENDER_COMMAND_TYPE_CUSTOM: {
            Clay_CustomRenderData* config = &renderCommand->renderData.custom;
            ClayCustomElement* custom_element = (ClayCustomElement*) config->customData;
            if (!custom_element) continue;
            switch (custom_element->type) {
            case CLAY_CUSTOM_ELEMENT_CHECKBOX: {
                ClayCustomElement_CheckBox checkbox = custom_element->custom_data.checkbox;

                Color color = RED;
                if (checkbox.active) {
                    color = GREEN;
                }
                DrawRectangle(boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height, color);
                break;
            }
            default:
                PANIC("Error: unhandled custom element type.\n");
            }
            break;
        }
        default:
            PANIC("Error: unhandled render command.\n");
        }
    }
}
