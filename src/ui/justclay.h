#pragma once

#include <math.h>

#include "clay.h"

#include "introspect/introspect.h"
#include "assets/asset.h"
#include "events/declmacro.h"

#define JCLAY(...)                                                                                                                                                  \
    for (                                                                                                                                                           \
        CLAY__ELEMENT_DEFINITION_LATCH = (JustClay__OpenElement(), JustClay__ConfigureOpenElement(CLAY__CONFIG_WRAPPER(Clay_ElementDeclaration, __VA_ARGS__)), 0);  \
        CLAY__ELEMENT_DEFINITION_LATCH < 1;                                                                                                                         \
        CLAY__ELEMENT_DEFINITION_LATCH=1, JustClay__CloseElement()                                                                                                  \
    )

typedef uint32 Clay_Id;
#define CLAY_NULLID 0

#define RAYLIB_VECTOR2_TO_CLAY_VECTOR2(vector) (Clay_Vector2) { .x = (vector).x, .y = (vector).y }
#define CLAY_RECTANGLE_TO_RAYLIB_RECTANGLE(rectangle) (Rectangle) { .x = (rectangle).x, .y = (rectangle).y, .width = (rectangle).width, .height = (rectangle).height }
#define CLAY_COLOR_TO_RAYLIB_COLOR(color) (Color) { .r = (unsigned char)roundf((color).r), .g = (unsigned char)roundf((color).g), .b = (unsigned char)roundf((color).b), .a = (unsigned char)roundf((color).a) }
#define RAYLIB_COLOR_TO_CLAY_COLOR(color) (Clay_Color) { .r = (float)(color).r, .g = (float)(color).g, .b = (float)(color).b, .a = (float)((color).a) }

Clay_String string_to_clay_string(String string);
String clay_string_to_string(Clay_String clay_string);

// -----

typedef enum {
    CLAY_CUSTOM_ELEMENT_CHECKBOX,
} ClayCustomElementType;

typedef struct {
    bool active;
} ClayCustomElement_CheckBox;

typedef struct {
    ClayCustomElementType type;
    union {
        ClayCustomElement_CheckBox checkbox;
    } custom_data;
} ClayCustomElement;

// -----

typedef enum {
    JUSTCLAY_POINTER_INTERRACTION_ONHOVER,
    JUSTCLAY_POINTER_INTERRACTION_PRESSED,
    JUSTCLAY_POINTER_INTERRACTION_RELEASED,
} JustClay_PointerInterractionType;

typedef struct {
    bool consumed;
    Clay_ElementId element_id; // Clay_ElementId::id
    JustClay_PointerInterractionType interraction_type;
} JustClay_PointerEvent;

__DECLARE__EVENT_SYSTEM__ACCESS_SINGLE_THREADED(JustClay_PointerEvent);

typedef enum {
    JUSTCLAY_POINTER_ONHOVER,
    JUSTCLAY_POINTER_ONPRESS,
    JUSTCLAY_POINTER_JUSTBEGINHOVER,
    JUSTCLAY_POINTER_JUSTENDHOVER,
    JUSTCLAY_POINTER_JUSTPRESSED,
    JUSTCLAY_POINTER_JUSTRELEASED,
    JUSTCLAY_POINTER_JUSTCLICKED,
} JustClay_PointerStateEnum;

introspect
typedef struct {
    bool on_hover;
    bool on_press;
    bool just_begin_hover;
    bool just_end_hover;
    // active on the frame `MouseLeftButton` is pressed on the elements
    bool just_pressed;
    // active on the frame `MouseLeftButton` is released on the elements
    bool just_released;
    // active on the frame `MouseLeftButton` is released on the element
    // if the same element was previously pressed
    bool just_clicked;
} JustClay_PointerState;

typedef struct {
    bool is_modified;
    Clay_ElementId element_id; // set if .is_modified is true
    JustClay_PointerState pointer;
} JustClay_ElementState;

typedef void (*Clay_OnHoverFunction)(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData);
typedef void (*JustClay_OnPointerInterractFn)(Clay_ElementId element_id, JustClay_PointerState pointer_state, void* user_data);

typedef struct {
    void* user_data;
    JustClay_OnPointerInterractFn on_interract_fn;
} JustClay_OnPointerInterract_UserData;

typedef struct {
    JustClay_OnPointerInterract_UserData just_on_pointer_interract_user_data;
    JustClay_ElementState state;
} JustClay_Element;

typedef struct {
    Clay_Id id; // Clay_ElementId::id
    JustClay_Element element;
} JustClay_ElementKV;

typedef struct {
    usize count;
    usize capacity;
    JustClay_ElementKV* items;
    Clay_ElementId hovered_element_id;
    Clay_ElementId pressed_element_id;
    // Clay_ElementId hovered_element_clay_id;
    // Clay_ElementId pressed_element_clay_id;
} JustClay_ElementStore;

JustClay_Element* JustClay_FindElement(Clay_Id id);
JustClay_ElementState JustClay_GetElementState(Clay_Id id);

Clay_ElementId JUSTCLAY_INTERRACT(Clay_ElementId element_id);
void JustClay__OpenElement();
void JustClay__CloseElement();
void JustClay__ConfigureOpenElement(const Clay_ElementDeclaration config);

Clay_Id JustClay_InterractWithPointer(JustClay_OnPointerInterractFn on_pointer_interract_fn, void* user_data);

// -----
// inside element creation
// -----
bool JustClay_This_Check(JustClay_PointerStateEnum state);
// --
bool JustClay_This_OnHover();
bool JustClay_This_OnPress();
bool JustClay_This_JustBeginHover();
bool JustClay_This_JustEndHover();
bool JustClay_This_JustPressed();
bool JustClay_This_JustReleased();
bool JustClay_This_JustClicked();
// -----

// -----
// after element creation, inside procedural block
// -----
bool JustClay_Check(JustClay_PointerStateEnum state);
// --
bool JustClay_OnHover();
bool JustClay_OnPress();
bool JustClay_JustBeginHover();
bool JustClay_JustEndHover();
bool JustClay_JustPressed();
bool JustClay_JustReleased();
bool JustClay_JustClicked();
// -----

// -----

typedef struct {
    usize count;
    usize capacity;
    Font* fonts;
} FontList;

void initialize_justclay(FontList* font_list);

// -- SYSTEM --

void SYSTEM_PRE_PREPARE_reinit_justclay_if_necessary();

void SYSTEM_PRE_PREPARE_justclay_set_state(
    Vector2 mouse_position,
    bool mouse_down
);

void SYSTEM_POST_PREPARE_justclay_update_scroll_containers(
    Vector2 mouse_wheel_delta,
    float32 delta_time
);

void SYSTEM_RENDER_justclay_ui(
    TextureAssets* RES_TEXTURE_ASSETS,
    FontList RES_FONT_LIST,
    Clay_RenderCommandArray renderCommands
);