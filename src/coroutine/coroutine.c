
#include "memory/memory.h"

#include "coroutine.h"

// Coroutine

static inline bool coroutine_equals(Coroutine co1, Coroutine co2) {
    return co1.fn == co2.fn
        && co1.state == co2.state
        && co1.data == co2.data;
}

Coroutine make_coroutine(CoroutineFn fn, void* state, void* data) {
    return (Coroutine) {
        .fn = fn,
        .state = state,
        .data = data,
    };
}

// CoroutineExecutor

static bool find_coroutine(CoroutineExecutor* executor, Coroutine coroutine, usize* set_index) {
    for (usize i = 0; i < executor->coroutine_list.count; i++) {
        Coroutine coroutine_i = executor->coroutine_list.coroutines[i];
        if (coroutine_equals(coroutine_i, coroutine)) {
            if (set_index != NULL) *set_index = i;
            return true;
        }
    }
    return false;
}

bool add_coroutine(CoroutineExecutor* executor, Coroutine coroutine) {
    if (find_coroutine(executor, coroutine, NULL)) {
        return false;
    }
    dynarray_push_back_custom(executor->coroutine_list, .coroutines, coroutine);
    return true;
}

static void remove_coroutine_on_index(CoroutineExecutor* executor, usize index) {
    dynarray_swap_remove_custom(executor->coroutine_list, .coroutines, index);
}

bool remove_coroutine(CoroutineExecutor* executor, Coroutine coroutine) {
    usize index;
    if (find_coroutine(executor, coroutine, &index)) {
        remove_coroutine_on_index(executor, index);
        return true;
    }
    return false;
}

static bool end_current_coroutine = false;
void __end_coroutine() {
    end_current_coroutine = true;
}

void execute_coroutines(CoroutineExecutor* executor) {
    for (usize i = 0; i < executor->coroutine_list.count; i++) {
        Coroutine coroutine = executor->coroutine_list.coroutines[i];
        coroutine.fn(coroutine.state, coroutine.data);
        if (end_current_coroutine) {
            end_current_coroutine = false;
            remove_coroutine_on_index(executor, i);
            i--;
        }
    }
}

#include "animation/animation.h"
#include "animation/tween.h"
#include "render2d/sprite.h"
#include "lib.h"

typedef struct {
    Tween(Vector2) position_tween;
} CoroutineState_SpritePositionTween;

typedef struct {
    SpriteStore* sprite_store;
    SpriteEntityId sprite_id;
    // --
    bool* is_paused;
} CoroutineData_SpritePositionTween;

void coroutine_fn_sprite_position_tween(CoroutineState_SpritePositionTween* state, CoroutineData_SpritePositionTween* data) {
    bool tween_finished = state->position_tween.state.mode == TWEEN_ONCE && state->position_tween.state.elapsed > state->position_tween.state.duration;
    if (tween_finished || !sprite_is_valid(data->sprite_store, data->sprite_id)) {
        std_free(state);
        std_free(data);
        end_coroutine();
    }
    if (data->is_paused != NULL && *data->is_paused) {
        return;
    }

    float32 delta_time = JUST_GLOBAL.delta_time; // TODO: or use *data->delta_time
    Vector2 new_position = tween_tick(Vector2)(&state->position_tween, delta_time);
    
    SpriteTransform* transform = &data->sprite_store->transforms[data->sprite_id.id];
    transform->position = new_position;
}

Coroutine make_coroutine_sprite_position_tween(SpriteStore* sprite_store, SpriteEntityId sprite_id, Tween(Vector2) position_tween, bool* is_paused) {
    CoroutineState_SpritePositionTween state = {
        .position_tween = position_tween,
    };
    CoroutineData_SpritePositionTween data = {
        .sprite_store = sprite_store,
        .sprite_id = sprite_id,
        .is_paused = is_paused,
    };

    Coroutine coroutine;
    alloc_make_coroutine(coroutine, (CoroutineFn)coroutine_fn_sprite_position_tween, state, data);
    return coroutine;
}

typedef struct {
    SpriteSheetAnimationState anim_state;
    uint32 repeated_count;
} CoroutineState_SpriteSheetAnimation;

typedef struct {
    SpriteStore* sprite_store;
    SpriteEntityId sprite_id;
    uint32 repeat_times;
    // --
    bool* is_paused;
} CoroutineData_SpriteSheetAnimation;

void coroutine_fn_sprite_sheet_animation(CoroutineState_SpriteSheetAnimation* state, CoroutineData_SpriteSheetAnimation* data) {
    if (!sprite_is_valid(data->sprite_store, data->sprite_id)) {
        std_free(state);
        std_free(data);
        end_coroutine();
    }
    if (data->is_paused != NULL && *data->is_paused) {
        return;
    }

    tick_animation_state(&state->anim_state);
    if (animation_is_finished(&state->anim_state)) {
        state->repeated_count++;
        if (state->repeated_count == data->repeat_times){
            end_coroutine();
        }
    }
}

Coroutine make_coroutine_sprite_sheet_animation(
    SpriteStore* sprite_store, SpriteEntityId sprite_id,
    SpriteSheetAnimationState anim_state, uint32 repeat_times,
    bool* is_paused
) {
    CoroutineState_SpriteSheetAnimation state = {
        .anim_state = anim_state,
        .repeated_count = 0,
    };
    CoroutineData_SpriteSheetAnimation data = {
        .sprite_store = sprite_store,
        .sprite_id = sprite_id,
        .repeat_times = repeat_times,
        .is_paused = is_paused,
    };

    Coroutine coroutine;
    alloc_make_coroutine(coroutine, (CoroutineFn)coroutine_fn_sprite_sheet_animation, state, data);
    return coroutine;
}