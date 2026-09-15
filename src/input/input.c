#include "input.h"

KeyState next_key_state(KeyState key_state, bool state_isdown) {
    switch (key_state) {
    case KEY_STATE_NULL:
    case KEY_STATE_RELEASED:
        return branchless_if(state_isdown, KEY_STATE_PRESSED, KEY_STATE_NULL);
    case KEY_STATE_PRESSED:
    case KEY_STATE_REPEATED:
        return branchless_if(state_isdown, KEY_STATE_REPEATED, KEY_STATE_RELEASED);
        break;
    }
}

// -- Keyboard

bool key_just_pressed(KeyInputs* key_inputs, uint32 key) {
    return key_inputs->keys[key] == KEY_STATE_PRESSED;
}
bool key_is_repeated(KeyInputs* key_inputs, uint32 key) {
    return key_inputs->keys[key] == KEY_STATE_REPEATED;
}
bool key_is_released(KeyInputs* key_inputs, uint32 key) {
    return key_inputs->keys[key] == KEY_STATE_RELEASED;
}
bool key_is_up(KeyInputs* key_inputs, uint32 key) {
    return key_inputs->keys[key] <= KEY_STATE_RELEASED; // KEY_STATE_NULL || KEY_STATE_RELEASED
}
bool key_is_down(KeyInputs* key_inputs, uint32 key) {
    return key_inputs->keys[key] >= KEY_STATE_PRESSED; // KEY_STATE_PRESSED || KEY_STATE_REPEATED
}

void update_key_state(KeyInputs* key_inputs, uint32 key, bool state_isdown) {
    KeyState* key_state = &key_inputs->keys[key];
    *key_state = next_key_state(*key_state, state_isdown);
}

void set_key_repeated(KeyInputs* key_inputs, uint32 key) {
    key_inputs->keys[key] = KEY_STATE_REPEATED;
}

void SYSTEM_INPUT_get_keyboard_inputs(KeyInputs* RES_key_inputs) {
    for (uint32 key = 0; key < MAX_KEYBOARD_KEYS; key++) {
        if (IsKeyPressedRepeat(key)) {
            set_key_repeated(RES_key_inputs, key);
        }
        else {
            update_key_state(RES_key_inputs, key, IsKeyDown(key));
        }
    }
}

// -- Gamepad

bool gamepad_button_just_pressed(GamepadInputs* gamepad_inputs, uint32 button) {
    return gamepad_inputs->buttons[button] == KEY_STATE_PRESSED;
}
bool gamepad_button_is_repeated(GamepadInputs* gamepad_inputs, uint32 button) {
    return gamepad_inputs->buttons[button] == KEY_STATE_REPEATED;
}
bool gamepad_button_is_released(GamepadInputs* gamepad_inputs, uint32 button) {
    return gamepad_inputs->buttons[button] == KEY_STATE_RELEASED;
}
bool gamepad_button_is_up(GamepadInputs* gamepad_inputs, uint32 button) {
    return gamepad_inputs->buttons[button] <= KEY_STATE_RELEASED; // KEY_STATE_NULL || KEY_STATE_RELEASED
}
bool gamepad_button_is_down(GamepadInputs* gamepad_inputs, uint32 button) {
    return gamepad_inputs->buttons[button] >= KEY_STATE_PRESSED; // KEY_STATE_PRESSED || KEY_STATE_REPEATED
}
float32 gamepad_axis_value(GamepadInputs* gamepad_inputs, uint32 axis) {
    return gamepad_inputs->axis_values[axis];
}
float32 gamepad_axis_delta(GamepadInputs* gamepad_inputs, uint32 axis) {
    return gamepad_inputs->axis_values[axis] - gamepad_inputs->prev_axis_values[axis];
}

void update_gamepad_button_state(GamepadInputs* gamepad_inputs, uint32 button, bool state_isdown) {
    KeyState* button_state = &gamepad_inputs->buttons[button];
    *button_state = next_key_state(*button_state, state_isdown);
}

void update_gamepad_axis_value(GamepadInputs* gamepad_inputs, uint32 axis, float32 value) {
    gamepad_inputs->prev_axis_values[axis] = gamepad_inputs->axis_values[axis];
    gamepad_inputs->axis_values[axis] = value;
}

// -- ControlsMap

void controls_map_set_control(ControlsMap* controls, uint32 input, uint32 control) {
    controls->map[control] = input;
    controls->invmap[input] = control;
}

uint32 controls_map_get_input(ControlsMap* controls, uint32 control) {
    return controls->map[control];
}

uint32 controls_map_get_control(ControlsMap* controls, uint32 input) {
    return controls->invmap[input];
}
