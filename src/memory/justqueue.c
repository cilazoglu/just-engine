
#include "justqueue.h"

// Queue

Queue_usize Queue_usize__queue_new(usize capacity) {
    Queue_usize q = {0};
    dynarray_reserve(q, capacity);
    return q;
}

void Queue_usize__queue_free(Queue_usize* q) {
    dynarray_free(*q);
}

void Queue_usize__queue_reset(Queue_usize* q) {
    void* items = q->items;
    *q = (Queue_usize) {0};
    q->items = items;
}

bool Queue_usize__queue_is_full(Queue_usize* q) {
    return q->count == q->capacity;
}

bool Queue_usize__queue_is_empty(Queue_usize* q) {
    return q->count == 0;
}

bool Queue_usize__queue_has_next(Queue_usize* q) {
    return q->count > 0;
}

bool Queue_usize__queue_push(Queue_usize* q, usize item) {
    if (Queue_usize__queue_is_full(q)) {
        return false;
    }
    q->items[q->tail] = item;
    q->tail = (q->tail + 1) % q->capacity;
    q->count++;
    return true;
}

bool Queue_usize__queue_pop(Queue_usize* q, usize* set_item) {
    if (Queue_usize__queue_is_empty(q)) {
        return false;
    }
    *set_item = q->items[q->head];
    q->count--;
    q->head = (q->head + 1) % q->capacity;
    return true;
}

// Stack

Stack_usize Stack_usize__stack_new(usize capacity) {
    Stack_usize s = {0};
    dynarray_reserve(s, capacity);
    return s;
}

void Stack_usize__stack_free(Stack_usize* s) {
    dynarray_free(*s);
}

void Stack_usize__stack_reset(Stack_usize* s) {
    s->count = 0;
}

bool Stack_usize__stack_is_full(Stack_usize* s) {
    return s->count == s->capacity;
}

bool Stack_usize__stack_is_empty(Stack_usize* s) {
    return s->count == 0;
}

bool Stack_usize__stack_has_next(Stack_usize* s) {
    return s->count > 0;
}

bool Stack_usize__stack_push(Stack_usize* s, usize item) {
    if (Stack_usize__stack_is_full(s)) {
        return false;
    }
    s->items[s->count] = item;
    s->count++;
    return true;
}

bool Stack_usize__stack_pop(Stack_usize* s, usize* set_item) {
    if (Stack_usize__stack_is_empty(s)) {
        return false;
    }
    *set_item = s->items[s->count];
    s->count--;
    return true;
}
