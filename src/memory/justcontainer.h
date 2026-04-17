#pragma once

#include "core.h"
#include "memory.h"
#include "dynarray.h"

#define Queue(Type) Queue_##Type
#define queue_new(Type) Queue_##Type##__queue_new
#define queue_free(Type) Queue_##Type##__queue_free
#define queue_reset(Type) Queue_##Type##__queue_reset
#define queue_is_empty(Type) Queue_##Type##__queue_is_empty
#define queue_has_next(Type) Queue_##Type##__queue_has_next
#define queue_push(Type) Queue_##Type##__queue_push
#define queue_pop(Type) Queue_##Type##__queue_pop

typedef struct {
    usize head;
    usize tail;
    // --
    usize count;
    usize capacity;
    usize* items;
} Queue_usize;

Queue_usize Queue_usize__queue_new(usize capacity);
void Queue_usize__queue_free(Queue_usize* q);
void Queue_usize__queue_reset(Queue_usize* q);
bool Queue_usize__queue_is_full(Queue_usize* q);
bool Queue_usize__queue_is_empty(Queue_usize* q);
bool Queue_usize__queue_has_next(Queue_usize* q);
bool Queue_usize__queue_push(Queue_usize* q, usize item);
bool Queue_usize__queue_pop(Queue_usize* q, usize* set_item);

typedef struct {
    usize count; // top
    usize capacity;
    usize* items;
} Stack_usize;

Stack_usize Stack_usize__stack_new(usize capacity);
void Stack_usize__stack_free(Stack_usize* s);
void Stack_usize__stack_reset(Stack_usize* s);
bool Stack_usize__stack_is_full(Stack_usize* s);
bool Stack_usize__stack_is_empty(Stack_usize* s);
bool Stack_usize__stack_has_next(Stack_usize* s);
bool Stack_usize__stack_push(Stack_usize* s, usize item);
bool Stack_usize__stack_pop(Stack_usize* s, usize* set_item);

// TODO: InfiniteQueue
// TODO: InfiniteStack