#pragma once

#include "justcstd.h"
#include "core.h"

typedef void (*CoroutineFn)(void* state, void* data);

typedef struct {
    CoroutineFn fn;
    void* state; // internal state within fn
    void* data; // outside state
} Coroutine;

Coroutine make_coroutine(CoroutineFn fn, void* state, void* data);

#define alloc_make_coroutine(coroutine, fn, state_val, data_val) \
    do { \
        typeof((state_val))* alloc_make_coroutine__state = std_malloc(sizeof((state_val))); \
        *alloc_make_coroutine__state = (state_val); \
        typeof((data_val))* alloc_make_coroutine__data = std_malloc(sizeof((data_val))); \
        *alloc_make_coroutine__data = (data_val); \
        (coroutine) = make_coroutine((fn), alloc_make_coroutine__state, alloc_make_coroutine__data); \
    } while(0)

typedef struct {
    usize count;
    usize capacity;
    Coroutine* coroutines;
} CoroutineList;

// single threaded
typedef struct {
    CoroutineList coroutine_list;
} CoroutineExecutor;

bool add_coroutine(CoroutineExecutor* executor, Coroutine coroutine);
bool remove_coroutine(CoroutineExecutor* executor, Coroutine coroutine);

void execute_coroutines(CoroutineExecutor* executor);

void __end_coroutine();
#define end_coroutine() do { __end_coroutine(); return; } while(0)