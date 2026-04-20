#pragma once

#include "core.h"

#ifdef PRE_INTROSPECT_PASS
    #define introspect _introspect__just_to_make_sure_no_token_overlap__
    #define introspect_with(...) _introspect_with__just_to_make_sure_no_token_overlap__(__VA_ARGS__)
    #define alias(alias_type) _alias__just_to_make_sure_no_token_overlap__(alias_type)
    #define enum(...) _alias__just_to_make_sure_no_token_overlap__(uint32)
    #define union_header(...) _union_header__just_to_make_sure_no_token_overlap__(__VA_ARGS__)
    #define mode_discriminated_union(discriminant_field) _mode_discriminated_union__just_to_make_sure_no_token_overlap__(discriminant_field)
    #define mode_cstr(...) _mode_cstr__just_to_make_sure_no_token_overlap__(__VA_ARGS__)
    #define mode_dynarray(count_field) _mode_dynarray__just_to_make_sure_no_token_overlap__(count_field)
    #define mode_string(count_field) _mode_string__just_to_make_sure_no_token_overlap__(count_field)
    #define mode_function_ptr(...) _mode_function_ptr__just_to_make_sure_no_token_overlap__(__VA_ARGS__)
    #define defines_invalid(...) _defines_invalid__just_to_make_sure_no_token_overlap__(__VA_ARGS__)
#else
    #define introspect 
    #define introspect_with(...) 
    #define alias(alias_type) 
    #define enum(...) 
    #define union_header(...) 
    #define mode_discriminated_union(discriminant_field) 
    #define mode_cstr(...) 
    #define mode_dynarray(count_field) 
    #define mode_string(count_field) 
    #define mode_function_ptr(...) 
    #define defines_invalid(...) 

    #define _introspect__just_to_make_sure_no_token_overlap__ 
    #define _introspect_with__just_to_make_sure_no_token_overlap__(...) 
    #define _alias__just_to_make_sure_no_token_overlap__(alias_type) 
    #define _union_header__just_to_make_sure_no_token_overlap__(...) 
    #define _mode_discriminated_union__just_to_make_sure_no_token_overlap__(discriminant_field) 
    #define _mode_cstr__just_to_make_sure_no_token_overlap__(...) 
    #define _mode_dynarray__just_to_make_sure_no_token_overlap__(count_field) 
    #define _mode_string__just_to_make_sure_no_token_overlap__(count_field) 
    #define _mode_function_ptr__just_to_make_sure_no_token_overlap__(...) 
    #define _defines_invalid__just_to_make_sure_no_token_overlap__(...) 
#endif

// TODO
/**
 * TODO:
 * - handle enum types
 * - handle void type (ofc void*, not void)
 * - handle multi layer pointers (i.e. uint32***)
 * - handle multi dimension arrays (i.e. arr[3][5][6])
 * - handle type aliasing with `alias()`
 * - function pointer `function_ptr()`
 * - IMPORTANT: find a way to solve "include order" and "type not defined in introspect file" issues
 * - IMPORTANT: support dynarray count/capacity other than usize
 */

typedef enum {
    TYPE_void, // TODO
    TYPE_char,
    TYPE_byte,
    TYPE_bool,
    TYPE_uint8,
    TYPE_uint16,
    TYPE_uint32,
    TYPE_uint64,
    TYPE_int8,
    TYPE_int16,
    TYPE_int32,
    TYPE_int64,
    TYPE_usize,
    TYPE_float32,
    TYPE_float64,
    TYPE_struct,
    TYPE_union,
} Type;

typedef struct FieldInfo {
    Type type;
    char* type_str;
    char* name;
    void* ptr;
    // --
    bool is_ptr;
    uint32 ptr_depth;
    // --
    bool is_array;
    usize count; // total array length
    usize array_dim;
    usize array_dim_counts[10]; // max 10 dimensions
    // --
    bool is_cstr;
    // --
    bool is_dynarray;
    bool is_string;
    void* count_ptr;
    // --
    usize struct_size;
    uint32 field_count;
    struct FieldInfo* fields;
    // --
    bool is_named_union;
    char* union_name;
    uint32 union_header_variant;
    // --
    bool is_discriminated_union;
    void* discriminant_ptr;
    // --
    usize union_size;
    uint32 variant_count;
    struct FieldInfo* variants;
    // --
} FieldInfo;

typedef struct {
    char* name;
    char* value;
    bool iota;
    // --
    bool defines_invalid;
    // --
} EnumMemberInfo;

// -----

#define ENUM_INVALID(Enum) ENUM_INVALID__##Enum
#define ENUM_COUNT(Enum) ENUM_COUNT__##Enum
#define ENUM_NAME(Enum) ENUM_NAME__##Enum
#define ENUM_VALUE(Enum) ENUM_VALUE__##Enum

// -----

