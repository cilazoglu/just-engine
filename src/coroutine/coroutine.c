
#include "memory/memory.h"
#include "memory/dynarray.h"

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
    dynarray_push_back(executor->coroutine_list, .coroutines, coroutine);
    return true;
}

static void remove_coroutine_on_index(CoroutineExecutor* executor, usize index) {
    dynarray_swap_remove(executor->coroutine_list, index, .coroutines);
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
