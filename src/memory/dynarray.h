#pragma once

#include "core.h"
#include "allocator.h"

// NOTE: use of __typeof__

// ------------------------------------
// ------------------------------------
// dynarray<T_Size, ...T_Item_I>:
// - Allocator? allocator
// - T_Size count
// - T_Size capacity
// - [...T_Item_I* {items_field_i}]
// ------------------------------------
// ------------------------------------

#define dynarray_inmacro_variable_prefix __dynarray_inmacro_variable_prefix__
#define DYNARRAY_INITIAL_CAPACITY 2
#define DYNARRAY_GROWTH_FACTOR 2

// clear:
// - .count

#define dynarray_clear(arr) \
    do { \
        (arr).count = 0; \
    } while(0)

// free:
// - .count
// - .capacity
// - .items_field

#define dynarray_free(arr, items_field) \
    do { \
        if ((arr).capacity > 0) { \
            (arr).count = 0; \
            (arr).capacity = 0; \
            std_free((arr)items_field); \
        } \
    } while(0)

#define dynarray_free2(arr, items_field_1, items_field_2) \
    do { \
        if ((arr).capacity > 0) { \
            (arr).count = 0; \
            (arr).capacity = 0; \
            std_free((arr)items_field_1); \
            std_free((arr)items_field_2); \
        } \
    } while(0)

#define dynarray_free3(arr, items_field_1, items_field_2, items_field_3) \
    do { \
        if ((arr).capacity > 0) { \
            (arr).count = 0; \
            (arr).capacity = 0; \
            std_free((arr)items_field_1); \
            std_free((arr)items_field_2); \
            std_free((arr)items_field_3); \
        } \
    } while(0)

// free_in:
// - .allocator
// - .count
// - .capacity
// - .items_field

#define dynarray_free_in(arr, items_field) \
    do { \
        if ((arr).capacity > 0) { \
            (arr).count = 0; \
            (arr).capacity = 0; \
            just_free((arr).allocator, (arr)items_field); \
        } \
    } while(0)

#define dynarray_free2_in(arr, items_field_1, items_field_2) \
    do { \
        if ((arr).capacity > 0) { \
            (arr).count = 0; \
            (arr).capacity = 0; \
            just_free((arr).allocator, (arr)items_field_1); \
            just_free((arr).allocator, (arr)items_field_2); \
        } \
    } while(0)

#define dynarray_free3_in(arr, items_field_1, items_field_2, items_field_3) \
    do { \
        if ((arr).capacity > 0) { \
            (arr).count = 0; \
            (arr).capacity = 0; \
            just_free((arr).allocator, (arr)items_field_1); \
            just_free((arr).allocator, (arr)items_field_2); \
            just_free((arr).allocator, (arr)items_field_3); \
        } \
    } while(0)

// reserve:
// - .count
// - .capacity
// - .items_field

#define dynarray_reserve(arr, items_field, reserve_count) \
    do { \
        __typeof__((arr).capacity) dynarray_reserve__new_capacity = (arr).count + reserve_count; \
        if ((arr).capacity < dynarray_reserve__new_capacity) { \
            (arr).capacity = dynarray_reserve__new_capacity; \
            (arr)items_field = std_realloc((arr)items_field, (arr).capacity * sizeof((arr)items_field[0])); \
        } \
    } while(0)

#define dynarray_reserve2(arr, items_field_1, items_field_2, reserve_count) \
    do { \
        __typeof__((arr).capacity) dynarray_reserve__new_capacity = (arr).count + reserve_count; \
        if ((arr).capacity < dynarray_reserve__new_capacity) { \
            (arr).capacity = dynarray_reserve__new_capacity; \
            (arr)items_field_1 = std_realloc((arr)items_field_1, (arr).capacity * sizeof((arr)items_field_1[0])); \
            (arr)items_field_2 = std_realloc((arr)items_field_2, (arr).capacity * sizeof((arr)items_field_2[0])); \
        } \
    } while(0)

#define dynarray_reserve3(arr, items_field_1, items_field_2, items_field_3, reserve_count) \
    do { \
        __typeof__((arr).capacity) dynarray_reserve__new_capacity = (arr).count + reserve_count; \
        if ((arr).capacity < dynarray_reserve__new_capacity) { \
            (arr).capacity = dynarray_reserve__new_capacity; \
            (arr)items_field_1 = std_realloc((arr)items_field_1, (arr).capacity * sizeof((arr)items_field_1[0])); \
            (arr)items_field_2 = std_realloc((arr)items_field_2, (arr).capacity * sizeof((arr)items_field_2[0])); \
            (arr)items_field_3 = std_realloc((arr)items_field_3, (arr).capacity * sizeof((arr)items_field_3[0])); \
        } \
    } while(0)

// reserve_in:
// - .allocator
// - .count
// - .capacity
// - .items_field

#define dynarray_reserve_in(arr, items_field, reserve_count) \
    do { \
        __typeof__((arr).capacity) dynarray_reserve__new_capacity = (arr).count + reserve_count; \
        if ((arr).capacity < dynarray_reserve__new_capacity) { \
            (arr).capacity = dynarray_reserve__new_capacity; \
            (arr)items_field = just_realloc_array((arr).allocator, /*Type*/((arr)items_field[0]), /*ptr*/((arr)items_field), /*count*/((arr).capacity)); \
        } \
    } while(0)

#define dynarray_reserve2_in(arr, items_field_1, items_field_2, reserve_count) \
    do { \
        __typeof__((arr).capacity) dynarray_reserve__new_capacity = (arr).count + reserve_count; \
        if ((arr).capacity < dynarray_reserve__new_capacity) { \
            (arr).capacity = dynarray_reserve__new_capacity; \
            (arr)items_field_1 = just_realloc_array((arr).allocator, /*Type*/((arr)items_field_1[0]), /*ptr*/((arr)items_field_1), /*count*/((arr).capacity)); \
            (arr)items_field_2 = just_realloc_array((arr).allocator, /*Type*/((arr)items_field_2[0]), /*ptr*/((arr)items_field_2), /*count*/((arr).capacity)); \
        } \
    } while(0)

#define dynarray_reserve3_in(arr, items_field_1, items_field_2, items_field_3, reserve_count) \
    do { \
        __typeof__((arr).capacity) dynarray_reserve__new_capacity = (arr).count + reserve_count; \
        if ((arr).capacity < dynarray_reserve__new_capacity) { \
            (arr).capacity = dynarray_reserve__new_capacity; \
            (arr)items_field_1 = just_realloc_array((arr).allocator, /*Type*/((arr)items_field_1[0]), /*ptr*/((arr)items_field_1), /*count*/((arr).capacity)); \
            (arr)items_field_2 = just_realloc_array((arr).allocator, /*Type*/((arr)items_field_2[0]), /*ptr*/((arr)items_field_2), /*count*/((arr).capacity)); \
            (arr)items_field_3 = just_realloc_array((arr).allocator, /*Type*/((arr)items_field_3[0]), /*ptr*/((arr)items_field_3), /*count*/((arr).capacity)); \
        } \
    } while(0)

// push_back:
// - .count
// - .capacity
// - .items_field

#define dynarray_push_back(arr, items_field, item) \
    do { \
        if ((arr).capacity == 0) { \
            (arr).capacity = DYNARRAY_INITIAL_CAPACITY; \
            (arr)items_field = std_malloc((arr).capacity * sizeof((item))); \
        } \
        else if ((arr).count == (arr).capacity) { \
            (arr).capacity = DYNARRAY_GROWTH_FACTOR * (arr).capacity; \
            (arr)items_field = std_realloc((arr)items_field, (arr).capacity * sizeof((item))); \
        } \
        \
        (arr)items_field[(arr).count] = (item); \
        (arr).count++; \
    } while(0)

#define dynarray_push_back2(arr, items_field_1, item_1, items_field_2, item_2) \
    do { \
        if ((arr).capacity == 0) { \
            (arr).capacity = DYNARRAY_INITIAL_CAPACITY; \
            (arr)items_field_1 = std_malloc((arr).capacity * sizeof((item_1))); \
            (arr)items_field_2 = std_malloc((arr).capacity * sizeof((item_2))); \
        } \
        else if ((arr).count == (arr).capacity) { \
            (arr).capacity = DYNARRAY_GROWTH_FACTOR * (arr).capacity; \
            (arr)items_field_1 = std_realloc((arr)items_field_1, (arr).capacity * sizeof((item_1))); \
            (arr)items_field_2 = std_realloc((arr)items_field_2, (arr).capacity * sizeof((item_2))); \
        } \
        \
        (arr)items_field_1[(arr).count] = (item_1); \
        (arr)items_field_2[(arr).count] = (item_2); \
        (arr).count++; \
    } while(0)

#define dynarray_push_back3(arr, items_field_1, item_1, items_field_2, item_2, items_field_3, item_3) \
    do { \
        if ((arr).capacity == 0) { \
            (arr).capacity = DYNARRAY_INITIAL_CAPACITY; \
            (arr)items_field_1 = std_malloc((arr).capacity * sizeof((item_1))); \
            (arr)items_field_2 = std_malloc((arr).capacity * sizeof((item_2))); \
            (arr)items_field_3 = std_malloc((arr).capacity * sizeof((item_3))); \
        } \
        else if ((arr).count == (arr).capacity) { \
            (arr).capacity = DYNARRAY_GROWTH_FACTOR * (arr).capacity; \
            (arr)items_field_1 = std_realloc((arr)items_field_1, (arr).capacity * sizeof((item_1))); \
            (arr)items_field_2 = std_realloc((arr)items_field_2, (arr).capacity * sizeof((item_2))); \
            (arr)items_field_3 = std_realloc((arr)items_field_3, (arr).capacity * sizeof((item_3))); \
        } \
        \
        (arr)items_field_1[(arr).count] = (item_1); \
        (arr)items_field_2[(arr).count] = (item_2); \
        (arr)items_field_3[(arr).count] = (item_3); \
        (arr).count++; \
    } while(0)

// push_back_in:
// - .allocator
// - .count
// - .capacity
// - .items_field

#define dynarray_push_back_in(arr, items_field, item) \
    do { \
        if ((arr).capacity == 0) { \
            (arr).capacity = DYNARRAY_INITIAL_CAPACITY; \
            (arr)items_field = just_alloc_array((arr).allocator, /*Type*/(item), /*count*/((arr).capacity)); \
        } \
        else if ((arr).count == (arr).capacity) { \
            (arr).capacity = DYNARRAY_GROWTH_FACTOR * (arr).capacity; \
            (arr)items_field = just_realloc_array((arr).allocator, /*Type*/(item), /*ptr*/((arr)items_field), /*count*/((arr).capacity)); \
        } \
        \
        (arr)items_field[(arr).count] = (item); \
        (arr).count++; \
    } while(0)

#define dynarray_push_back2_in(arr, items_field_1, item_1, items_field_2, item_2) \
    do { \
        if ((arr).capacity == 0) { \
            (arr).capacity = DYNARRAY_INITIAL_CAPACITY; \
            (arr)items_field_1 = just_alloc_array((arr).allocator, /*Type*/(item_1), /*count*/((arr).capacity)); \
            (arr)items_field_2 = just_alloc_array((arr).allocator, /*Type*/(item_2), /*count*/((arr).capacity)); \
        } \
        else if ((arr).count == (arr).capacity) { \
            (arr).capacity = DYNARRAY_GROWTH_FACTOR * (arr).capacity; \
            (arr)items_field_1 = just_realloc_array((arr).allocator, /*Type*/(item_1), /*ptr*/((arr)items_field_1), /*count*/((arr).capacity)); \
            (arr)items_field_2 = just_realloc_array((arr).allocator, /*Type*/(item_2), /*ptr*/((arr)items_field_2), /*count*/((arr).capacity)); \
        } \
        \
        (arr)items_field_1[(arr).count] = (item_1); \
        (arr)items_field_2[(arr).count] = (item_2); \
        (arr).count++; \
    } while(0)

#define dynarray_push_back3_in(arr, items_field_1, item_1, items_field_2, item_2, items_field_3, item_3) \
    do { \
        if ((arr).capacity == 0) { \
            (arr).capacity = DYNARRAY_INITIAL_CAPACITY; \
            (arr)items_field_1 = just_alloc_array((arr).allocator, /*Type*/(item_1), /*count*/((arr).capacity)); \
            (arr)items_field_2 = just_alloc_array((arr).allocator, /*Type*/(item_2), /*count*/((arr).capacity)); \
            (arr)items_field_3 = just_alloc_array((arr).allocator, /*Type*/(item_3), /*count*/((arr).capacity)); \
        } \
        else if ((arr).count == (arr).capacity) { \
            (arr).capacity = DYNARRAY_GROWTH_FACTOR * (arr).capacity; \
            (arr)items_field_1 = just_realloc_array((arr).allocator, /*Type*/(item_1), /*ptr*/((arr)items_field_1), /*count*/((arr).capacity)); \
            (arr)items_field_2 = just_realloc_array((arr).allocator, /*Type*/(item_2), /*ptr*/((arr)items_field_2), /*count*/((arr).capacity)); \
            (arr)items_field_3 = just_realloc_array((arr).allocator, /*Type*/(item_3), /*ptr*/((arr)items_field_3), /*count*/((arr).capacity)); \
        } \
        \
        (arr)items_field_1[(arr).count] = (item_1); \
        (arr)items_field_2[(arr).count] = (item_2); \
        (arr)items_field_3[(arr).count] = (item_3); \
        (arr).count++; \
    } while(0)

// insert:
// - .count
// - .capacity
// - .items_field

#define dynarray_insert(arr, index, items_field, item) \
    do { \
        usize dynarray_insert__index = MIN(index, (arr).count); \
        dynarray_reserve((arr), items_field, 1); \
        for (usize dynarray_insert__i = (arr).count; dynarray_insert__i > dynarray_insert__index; dynarray_insert__i--) { \
            (arr)items_field[dynarray_insert__i] = (arr)items_field[dynarray_insert__i - 1]; \
        } \
        (arr)items_field[dynarray_insert__index] = item; \
        (arr).count++; \
    } while(0)

#define dynarray_insert2(arr, index, items_field_1, item_1, items_field_2, item_2) \
    do { \
        usize dynarray_insert__index = MIN(index, (arr).count); \
        dynarray_reserve((arr), items_field, 1); \
        for (usize dynarray_insert__i = (arr).count; dynarray_insert__i > dynarray_insert__index; dynarray_insert__i--) { \
            (arr)items_field_1[dynarray_insert__i] = (arr)items_field_1[dynarray_insert__i - 1]; \
            (arr)items_field_2[dynarray_insert__i] = (arr)items_field_2[dynarray_insert__i - 1]; \
        } \
        (arr)items_field_1[dynarray_insert__index] = item_1; \
        (arr)items_field_2[dynarray_insert__index] = item_2; \
        (arr).count++; \
    } while(0)

#define dynarray_insert3(arr, index, items_field_1, item_1, items_field_2, item_2, items_field_3, item_3) \
    do { \
        usize dynarray_insert__index = MIN(index, (arr).count); \
        dynarray_reserve((arr), items_field, 1); \
        for (usize dynarray_insert__i = (arr).count; dynarray_insert__i > dynarray_insert__index; dynarray_insert__i--) { \
            (arr)items_field_1[dynarray_insert__i] = (arr)items_field_1[dynarray_insert__i - 1]; \
            (arr)items_field_2[dynarray_insert__i] = (arr)items_field_2[dynarray_insert__i - 1]; \
            (arr)items_field_3[dynarray_insert__i] = (arr)items_field_3[dynarray_insert__i - 1]; \
        } \
        (arr)items_field_1[dynarray_insert__index] = item_1; \
        (arr)items_field_2[dynarray_insert__index] = item_2; \
        (arr)items_field_3[dynarray_insert__index] = item_3; \
        (arr).count++; \
    } while(0)

// insert_in:
// - .allocator
// - .count
// - .capacity
// - .items_field

#define dynarray_insert_in(arr, index, items_field, item) \
    do { \
        usize dynarray_insert__index = MIN(index, (arr).count); \
        dynarray_reserve_in((arr), items_field, 1); \
        for (usize dynarray_insert__i = (arr).count; dynarray_insert__i > dynarray_insert__index; dynarray_insert__i--) { \
            (arr)items_field[dynarray_insert__i] = (arr)items_field[dynarray_insert__i - 1]; \
        } \
        (arr)items_field[dynarray_insert__index] = item; \
        (arr).count++; \
    } while(0)

#define dynarray_insert2_in(arr, index, items_field_1, item_1, items_field_2, item_2) \
    do { \
        usize dynarray_insert__index = MIN(index, (arr).count); \
        dynarray_reserve_in((arr), items_field, 1); \
        for (usize dynarray_insert__i = (arr).count; dynarray_insert__i > dynarray_insert__index; dynarray_insert__i--) { \
            (arr)items_field_1[dynarray_insert__i] = (arr)items_field_1[dynarray_insert__i - 1]; \
            (arr)items_field_2[dynarray_insert__i] = (arr)items_field_2[dynarray_insert__i - 1]; \
        } \
        (arr)items_field_1[dynarray_insert__index] = item_1; \
        (arr)items_field_2[dynarray_insert__index] = item_2; \
        (arr).count++; \
    } while(0)

#define dynarray_insert3_in(arr, index, items_field_1, item_1, items_field_2, item_2, items_field_4, item_3) \
    do { \
        usize dynarray_insert__index = MIN(index, (arr).count); \
        dynarray_reserve_in((arr), items_field, 1); \
        for (usize dynarray_insert__i = (arr).count; dynarray_insert__i > dynarray_insert__index; dynarray_insert__i--) { \
            (arr)items_field_1[dynarray_insert__i] = (arr)items_field_1[dynarray_insert__i - 1]; \
            (arr)items_field_2[dynarray_insert__i] = (arr)items_field_2[dynarray_insert__i - 1]; \
            (arr)items_field_3[dynarray_insert__i] = (arr)items_field_3[dynarray_insert__i - 1]; \
        } \
        (arr)items_field_1[dynarray_insert__index] = item_1; \
        (arr)items_field_2[dynarray_insert__index] = item_2; \
        (arr)items_field_3[dynarray_insert__index] = item_3; \
        (arr).count++; \
    } while(0)

// swap_remove:
// - .count
// - .items_field

#define dynarray_swap_remove(arr, index, items_field) \
    do { \
        (arr)items_field[index] = (arr)items_field[(arr).count - 1]; \
        (arr).count--; \
    } while(0)

#define dynarray_swap_remove2(arr, index, items_field_1, items_field_2) \
    do { \
        (arr)items_field_1[index] = (arr)items_field_1[(arr).count - 1]; \
        (arr)items_field_2[index] = (arr)items_field_2[(arr).count - 1]; \
        (arr).count--; \
    } while(0)

#define dynarray_swap_remove3(arr, index, items_field_1, items_field_2, items_field_3) \
    do { \
        (arr)items_field_1[index] = (arr)items_field_1[(arr).count - 1]; \
        (arr)items_field_2[index] = (arr)items_field_2[(arr).count - 1]; \
        (arr)items_field_3[index] = (arr)items_field_3[(arr).count - 1]; \
        (arr).count--; \
    } while(0)

// clone:
// - .count
// - .capacity
// - .items_field

#define dynarray_clone(dst_arr, src_arr, items_field) \
    do { \
        (dst_arr).count = (src_arr).count; \
        (dst_arr).capacity = (src_arr).count; \
        \
        (dst_arr)items_field = (src_arr)items_field; \
        (dst_arr)items_field = std_malloc(sizeof((src_arr)items_field[0]) * (src_arr).count); \
        std_memcpy((dst_arr)items_field, (src_arr)items_field, sizeof((src_arr)items_field[0]) * (src_arr).count); \
    } while(0)

#define dynarray_clone2(dst_arr, src_arr, items_field_1, items_field_2) \
    do { \
        (dst_arr).count = (src_arr).count; \
        (dst_arr).capacity = (src_arr).count; \
        \
        (dst_arr)items_field_1 = (src_arr)items_field_1; \
        (dst_arr)items_field_1 = std_malloc(sizeof((src_arr)items_field_1[0]) * (src_arr).count); \
        std_memcpy((dst_arr)items_field_1, (src_arr)items_field_1, sizeof((src_arr)items_field_1[0]) * (src_arr).count); \
        \
        (dst_arr)items_field_2 = (src_arr)items_field_2; \
        (dst_arr)items_field_2 = std_malloc(sizeof((src_arr)items_field_2[0]) * (src_arr).count); \
        std_memcpy((dst_arr)items_field_2, (src_arr)items_field_2, sizeof((src_arr)items_field_2[0]) * (src_arr).count); \
    } while(0)

#define dynarray_clone3(dst_arr, src_arr, items_field_1, items_field_2, items_field_3) \
    do { \
        (dst_arr).count = (src_arr).count; \
        (dst_arr).capacity = (src_arr).count; \
        \
        (dst_arr)items_field_1 = (src_arr)items_field_1; \
        (dst_arr)items_field_1 = std_malloc(sizeof((src_arr)items_field_1[0]) * (src_arr).count); \
        std_memcpy((dst_arr)items_field_1, (src_arr)items_field_1, sizeof((src_arr)items_field_1[0]) * (src_arr).count); \
        \
        (dst_arr)items_field_2 = (src_arr)items_field_2; \
        (dst_arr)items_field_2 = std_malloc(sizeof((src_arr)items_field_2[0]) * (src_arr).count); \
        std_memcpy((dst_arr)items_field_2, (src_arr)items_field_2, sizeof((src_arr)items_field_2[0]) * (src_arr).count); \
        \
        (dst_arr)items_field_3 = (src_arr)items_field_3; \
        (dst_arr)items_field_3 = std_malloc(sizeof((src_arr)items_field_3[0]) * (src_arr).count); \
        std_memcpy((dst_arr)items_field_3, (src_arr)items_field_3, sizeof((src_arr)items_field_3[0]) * (src_arr).count); \
    } while(0)


// clone_in:
// - dst_arr.allocator
// - .count
// - .capacity
// - .items_field

#define dynarray_clone_in(dst_arr, src_arr, items_field) \
    do { \
        (dst_arr).count = (src_arr).count; \
        (dst_arr).capacity = (src_arr).count; \
        \
        (dst_arr)items_field = (src_arr)items_field; \
        (dst_arr)items_field = just_alloc_array((dst_arr).allocator, /*Type*/((dst_arr)items_field[0]), /*count*/((src_arr).count)); \
        std_memcpy((dst_arr)items_field, (src_arr)items_field, (sizeof((src_arr)items_field[0]) * (src_arr).count)); \
    } while(0)

#define dynarray_clone2_in(dst_arr, src_arr, items_field_1, items_field_2) \
    do { \
        (dst_arr).count = (src_arr).count; \
        (dst_arr).capacity = (src_arr).count; \
        \
        (dst_arr)items_field_1 = (src_arr)items_field_1; \
        (dst_arr)items_field_1 = just_alloc_array((dst_arr).allocator, /*Type*/((dst_arr)items_field_1[0]), /*count*/((src_arr).count)); \
        std_memcpy((dst_arr)items_field_1, (src_arr)items_field_1, (sizeof((src_arr)items_field_1[0]) * (src_arr).count)); \
        \
        (dst_arr)items_field_2 = (src_arr)items_field_2; \
        (dst_arr)items_field_2 = just_alloc_array((dst_arr).allocator, /*Type*/((dst_arr)items_field_2[0]), /*count*/((src_arr).count)); \
        std_memcpy((dst_arr)items_field_2, (src_arr)items_field_2, (sizeof((src_arr)items_field_2[0]) * (src_arr).count)); \
    } while(0)

#define dynarray_clone3_in(dst_arr, src_arr, items_field_1, items_field_2, items_field_3) \
    do { \
        (dst_arr).count = (src_arr).count; \
        (dst_arr).capacity = (src_arr).count; \
        \
        (dst_arr)items_field_1 = (src_arr)items_field_1; \
        (dst_arr)items_field_1 = just_alloc_array((dst_arr).allocator, /*Type*/((dst_arr)items_field_1[0]), /*count*/((src_arr).count)); \
        std_memcpy((dst_arr)items_field_1, (src_arr)items_field_1, (sizeof((src_arr)items_field_1[0]) * (src_arr).count)); \
        \
        (dst_arr)items_field_2 = (src_arr)items_field_2; \
        (dst_arr)items_field_2 = just_alloc_array((dst_arr).allocator, /*Type*/((dst_arr)items_field_2[0]), /*count*/((src_arr).count)); \
        std_memcpy((dst_arr)items_field_2, (src_arr)items_field_2, (sizeof((src_arr)items_field_2[0]) * (src_arr).count)); \
        \
        (dst_arr)items_field_3 = (src_arr)items_field_3; \
        (dst_arr)items_field_3 = just_alloc_array((dst_arr).allocator, /*Type*/((dst_arr)items_field_3[0]), /*count*/((src_arr).count)); \
        std_memcpy((dst_arr)items_field_3, (src_arr)items_field_3, (sizeof((src_arr)items_field_3[0]) * (src_arr).count)); \
    } while(0)

// -----
