#pragma once

#include "core.h"
#include "memory/juststring.h"

#include "introspect.h"

typedef struct {
    char* token;
    uint32 count;
} IndentToken;

#define DEFAULT_INDENT_TOKEN ((IndentToken){ .count = 1, .token = "\t" })

// -----

#define just_print(Type) Type##__print0
#define just_pretty_print(Type) Type##__pretty_print0
#define just_pretty_print_with(Type) Type##__pretty_print_with0
#define just_array_print(Type) Type##_array__print0
#define just_array_pretty_print(Type) Type##_array__pretty_print0
#define just_array_pretty_print_with(Type) Type##_array__pretty_print_with0

// -----

#define __DECLARE__print_functions__stdout(TYPE) \
    void TYPE##__print(TYPE var); \
    void TYPE##_ptr__print(TYPE* ptr); \
    void TYPE##_array__print(TYPE* arr, usize count); \
    void TYPE##_array__pretty_print(TYPE* arr, usize count, uint32 indent, IndentToken indent_token); \
    void TYPE##_dynarray__print(TYPE* arr, usize count); \
    void TYPE##_dynarray__pretty_print(TYPE* arr, usize count, uint32 indent, IndentToken indent_token); \
\
    typedef enum { TYPE##__VARIANT__DECLARE__print_functions__stdout = 0 } TYPE##__ENUM__DECLARE__print_functions__stdout

void ptr__print(void* var);

__DECLARE__print_functions__stdout(char);
__DECLARE__print_functions__stdout(byte);
__DECLARE__print_functions__stdout(bool);
__DECLARE__print_functions__stdout(uint8);
__DECLARE__print_functions__stdout(uint16);
__DECLARE__print_functions__stdout(uint32);
__DECLARE__print_functions__stdout(uint64);
__DECLARE__print_functions__stdout(int8);
__DECLARE__print_functions__stdout(int16);
__DECLARE__print_functions__stdout(int32);
__DECLARE__print_functions__stdout(int64);
__DECLARE__print_functions__stdout(float32);
__DECLARE__print_functions__stdout(float64);
__DECLARE__print_functions__stdout(usize);

void char_cstr__print(char* cstr);
void char_string__print(char* str, usize count);

void struct__print(void* var, FieldInfo* fields, uint32 field_count);
void struct__pretty_print(void* var, FieldInfo* fields, uint32 field_count, uint32 indent, IndentToken indent_token);
void struct_ptr__print(void** ptr, FieldInfo* fields, uint32 field_count);
void struct_ptr__pretty_print(void** ptr, FieldInfo* fields, uint32 field_count, uint32 indent, IndentToken indent_token);
void struct_array__print(void* arr, usize count, uint32 struct_size, FieldInfo* fields, uint32 field_count);
void struct_array__pretty_print(void* arr, usize count, uint32 struct_size, FieldInfo* fields, uint32 field_count, uint32 indent, IndentToken indent_token);
void struct_dynarray__print(void* arr, usize count, uint32 struct_size, FieldInfo* fields, uint32 field_count);
void struct_dynarray__pretty_print(void* arr, usize count, uint32 struct_size, FieldInfo* fields, uint32 field_count, uint32 indent, IndentToken indent_token);

void introspect_field_print(FieldInfo field, void* var);
void introspect_field_pretty_print(FieldInfo field, void* var, uint32 indent, IndentToken indent_token);

// -----

#define just_sbappend(Type) Type##__sbappend0
#define just_pretty_sbappend(Type) Type##__pretty_sbappend0
#define just_pretty_sbappend_with(Type) Type##__pretty_sbappend_with0
#define just_array_sbappend(Type) Type##_array__sbappend0
#define just_array_pretty_sbappend(Type) Type##_array__pretty_sbappend0
#define just_array_pretty_sbappend_with(Type) Type##_array__pretty_sbappend_with0

#define just_tostring(Type) Type##__tostring0
#define just_pretty_tostring(Type) Type##__pretty_tostring0
#define just_pretty_tostring_with(Type) Type##__pretty_tostring_with0
#define just_array_tostring(Type) Type##_array__tostring0
#define just_array_pretty_tostring(Type) Type##_array__pretty_tostring0
#define just_array_pretty_tostring_with(Type) Type##_array__pretty_tostring_with0

#define just_tostring_in(Type) Type##__tostring_in0
#define just_pretty_tostring_in(Type) Type##__pretty_tostring_in0
#define just_pretty_tostring_in_with(Type) Type##__pretty_tostring_in_with0
#define just_array_tostring_in(Type) Type##_array__tostring_in0
#define just_array_pretty_tostring_in(Type) Type##_array__pretty_tostring_in0
#define just_array_pretty_tostring_in_with(Type) Type##_array__pretty_tostring_in_with0

// -----

#define __DECLARE__print_functions__sbappend(TYPE) \
    void TYPE##__sbappend(StringBuilder* sb, TYPE var); \
    void TYPE##_ptr__sbappend(StringBuilder* sb, TYPE* ptr); \
    void TYPE##_array__sbappend(StringBuilder* sb, TYPE* arr, usize count); \
    void TYPE##_array__pretty_sbappend(StringBuilder* sb, TYPE* arr, usize count, uint32 indent, IndentToken indent_token); \
    void TYPE##_dynarray__sbappend(StringBuilder* sb, TYPE* arr, usize count); \
    void TYPE##_dynarray__pretty_sbappend(StringBuilder* sb, TYPE* arr, usize count, uint32 indent, IndentToken indent_token); \
\
    typedef enum { TYPE##__VARIANT__DECLARE__print_functions__sbappend = 0 } TYPE##__ENUM__DECLARE__print_functions__sbappend

void ptr__sbappend(StringBuilder* sb, void* var);

__DECLARE__print_functions__sbappend(char);
__DECLARE__print_functions__sbappend(byte);
__DECLARE__print_functions__sbappend(bool);
__DECLARE__print_functions__sbappend(uint8);
__DECLARE__print_functions__sbappend(uint16);
__DECLARE__print_functions__sbappend(uint32);
__DECLARE__print_functions__sbappend(uint64);
__DECLARE__print_functions__sbappend(int8);
__DECLARE__print_functions__sbappend(int16);
__DECLARE__print_functions__sbappend(int32);
__DECLARE__print_functions__sbappend(int64);
__DECLARE__print_functions__sbappend(float32);
__DECLARE__print_functions__sbappend(float64);
__DECLARE__print_functions__sbappend(usize);

void char_cstr__sbappend(StringBuilder* sb, char* cstr);
void char_string__sbappend(StringBuilder* sb, char* str, usize count);

void struct__sbappend(StringBuilder* sb, void* var, FieldInfo* fields, uint32 field_count);
void struct__pretty_sbappend(StringBuilder* sb, void* var, FieldInfo* fields, uint32 field_count, uint32 indent, IndentToken indent_token);
void struct_ptr__sbappend(StringBuilder* sb, void** ptr, FieldInfo* fields, uint32 field_count);
void struct_ptr__pretty_sbappend(StringBuilder* sb, void** ptr, FieldInfo* fields, uint32 field_count, uint32 indent, IndentToken indent_token);
void struct_array__sbappend(StringBuilder* sb, void* arr, usize count, uint32 struct_size, FieldInfo* fields, uint32 field_count);
void struct_array__pretty_sbappend(StringBuilder* sb, void* arr, usize count, uint32 struct_size, FieldInfo* fields, uint32 field_count, uint32 indent, IndentToken indent_token);
void struct_dynarray__sbappend(StringBuilder* sb, void* arr, usize count, uint32 struct_size, FieldInfo* fields, uint32 field_count);
void struct_dynarray__pretty_sbappend(StringBuilder* sb, void* arr, usize count, uint32 struct_size, FieldInfo* fields, uint32 field_count, uint32 indent, IndentToken indent_token);

void introspect_field_sbappend(StringBuilder* sb, FieldInfo field, void* var);
void introspect_field_pretty_sbappend(StringBuilder* sb, FieldInfo field, void* var, uint32 indent, IndentToken indent_token);

// -----

#define __IMPL_____generate_print_functions(TYPE) \
    static inline void TYPE##__print(TYPE* var) { \
        struct__print(var, TYPE##__fields, ARRAY_LENGTH(TYPE##__fields)); \
    } \
    static inline void TYPE##__pretty_print_with(TYPE* var, uint32 indent, IndentToken indent_token) { \
        struct__pretty_print(var, TYPE##__fields, ARRAY_LENGTH(TYPE##__fields), indent, indent_token); \
    } \
    static inline void TYPE##__pretty_print(TYPE* var, uint32 indent) { \
        TYPE##__pretty_print_with(var, 0, DEFAULT_INDENT_TOKEN); \
    } \
\
    static inline void TYPE##_array__print(TYPE* var, usize count) { \
        struct_array__print(var, count, sizeof(TYPE), TYPE##__fields, ARRAY_LENGTH(TYPE##__fields)); \
    } \
    static inline void TYPE##_array__pretty_print_with(TYPE* var, usize count, uint32 indent, IndentToken indent_token) { \
        struct_array__pretty_print(var, count, sizeof(TYPE), TYPE##__fields, ARRAY_LENGTH(TYPE##__fields), indent, indent_token); \
    } \
    static inline void TYPE##_array__pretty_print(TYPE* var, usize count, uint32 indent) { \
        TYPE##_array__pretty_print_with(var, count, 0, DEFAULT_INDENT_TOKEN); \
    } \
\
    static inline void TYPE##__print0(char* name, TYPE* var) { \
        if (name) printf("%s: ", name); \
        TYPE##__print(var); \
        printf("\n"); \
    } \
    static inline void TYPE##__pretty_print0(char* name, TYPE* var) { \
        if (name) printf("%s: ", name); \
        TYPE##__pretty_print(var, 0); \
        printf("\n"); \
    } \
    static inline void TYPE##__pretty_print_with0(char* name, TYPE* var, IndentToken indent_token) { \
        if (name) printf("%s: ", name); \
        TYPE##__pretty_print_with(var, 0, indent_token); \
        printf("\n"); \
    } \
\
    static inline void TYPE##_array__print0(char* name, TYPE* var, usize count) { \
        if (name) printf("%s: ", name); \
        TYPE##_array__print(var, count); \
        printf("\n"); \
    } \
    static inline void TYPE##_array__pretty_print0(char* name, TYPE* var, usize count) { \
        if (name) printf("%s: ", name); \
        TYPE##_array__pretty_print(var, count, 0); \
        printf("\n"); \
    } \
    static inline void TYPE##_array__pretty_print_with0(char* name, TYPE* var, usize count, IndentToken indent_token) { \
        if (name) printf("%s: ", name); \
        TYPE##_array__pretty_print_with(var, count, 0, indent_token); \
        printf("\n"); \
    } \
\
\
\
    static inline void TYPE##__sbappend(StringBuilder* sb, TYPE* var) { \
        struct__sbappend(sb, var, TYPE##__fields, ARRAY_LENGTH(TYPE##__fields)); \
    } \
    static inline void TYPE##__pretty_sbappend_with(StringBuilder* sb, TYPE* var, uint32 indent, IndentToken indent_token) { \
        struct__pretty_sbappend(sb, var, TYPE##__fields, ARRAY_LENGTH(TYPE##__fields), indent, indent_token); \
    } \
    static inline void TYPE##__pretty_sbappend(StringBuilder* sb, TYPE* var, uint32 indent) { \
        TYPE##__pretty_sbappend_with(sb, var, 0, DEFAULT_INDENT_TOKEN); \
    } \
\
    static inline void TYPE##_array__sbappend(StringBuilder* sb, TYPE* var, usize count) { \
        struct_array__sbappend(sb, var, count, sizeof(TYPE), TYPE##__fields, ARRAY_LENGTH(TYPE##__fields)); \
    } \
    static inline void TYPE##_array__pretty_sbappend_with(StringBuilder* sb, TYPE* var, usize count, uint32 indent, IndentToken indent_token) { \
        struct_array__pretty_sbappend(sb, var, count, sizeof(TYPE), TYPE##__fields, ARRAY_LENGTH(TYPE##__fields), indent, indent_token); \
    } \
    static inline void TYPE##_array__pretty_sbappend(StringBuilder* sb, TYPE* var, usize count, uint32 indent) { \
        TYPE##_array__pretty_sbappend_with(sb, var, count, 0, DEFAULT_INDENT_TOKEN); \
    } \
\
    static inline void TYPE##__sbappend0(StringBuilder* sb, char* name, TYPE* var) { \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##__sbappend(sb, var); \
        string_builder_append_cstr(sb, "\n"); \
    } \
    static inline void TYPE##__pretty_sbappend0(StringBuilder* sb, char* name, TYPE* var) { \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##__pretty_sbappend(sb, var, 0); \
        string_builder_append_cstr(sb, "\n"); \
    } \
    static inline void TYPE##__pretty_sbappend_with0(StringBuilder* sb, char* name, TYPE* var, IndentToken indent_token) { \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##__pretty_sbappend_with(sb, var, 0, indent_token); \
        string_builder_append_cstr(sb, "\n"); \
    } \
\
    static inline void TYPE##_array__sbappend0(StringBuilder* sb, char* name, TYPE* var, usize count) { \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##_array__sbappend(sb, var, count); \
        string_builder_append_cstr(sb, "\n"); \
    } \
    static inline void TYPE##_array__pretty_sbappend0(StringBuilder* sb, char* name, TYPE* var, usize count) { \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##_array__pretty_sbappend(sb, var, count, 0); \
        string_builder_append_cstr(sb, "\n"); \
    } \
    static inline void TYPE##_array__pretty_sbappend_with0(StringBuilder* sb, char* name, TYPE* var, usize count, IndentToken indent_token) { \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##_array__pretty_sbappend_with(sb, var, count, 0, indent_token); \
        string_builder_append_cstr(sb, "\n"); \
    } \
\
\
\
    static inline String TYPE##__tostring0(char* name, TYPE* var) { \
        StringBuilder sb_var = string_builder_new(); \
        StringBuilder* sb = &sb_var; \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##__sbappend(sb, var); \
        string_builder_append_cstr(sb, "\n"); \
        return build_string(sb); \
    } \
    static inline String TYPE##__pretty_tostring0(char* name, TYPE* var) { \
        StringBuilder sb_var = string_builder_new(); \
        StringBuilder* sb = &sb_var; \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##__pretty_sbappend(sb, var, 0); \
        string_builder_append_cstr(sb, "\n"); \
        return build_string(sb); \
    } \
    static inline String TYPE##__pretty_tostring_with0(char* name, TYPE* var, IndentToken indent_token) { \
        StringBuilder sb_var = string_builder_new(); \
        StringBuilder* sb = &sb_var; \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##__pretty_sbappend_with(sb, var, 0, indent_token); \
        string_builder_append_cstr(sb, "\n"); \
        return build_string(sb); \
    } \
\
    static inline String TYPE##_array__tostring0(char* name, TYPE* var, usize count) { \
        StringBuilder sb_var = string_builder_new(); \
        StringBuilder* sb = &sb_var; \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##_array__sbappend(sb, var, count); \
        string_builder_append_cstr(sb, "\n"); \
        return build_string(sb); \
    } \
    static inline String TYPE##_array__pretty_tostring0(char* name, TYPE* var, usize count) { \
        StringBuilder sb_var = string_builder_new(); \
        StringBuilder* sb = &sb_var; \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##_array__pretty_sbappend(sb, var, count, 0); \
        string_builder_append_cstr(sb, "\n"); \
        return build_string(sb); \
    } \
    static inline String TYPE##_array__pretty_tostring_with0(char* name, TYPE* var, usize count, IndentToken indent_token) { \
        StringBuilder sb_var = string_builder_new(); \
        StringBuilder* sb = &sb_var; \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##_array__pretty_sbappend_with(sb, var, count, 0, indent_token); \
        string_builder_append_cstr(sb, "\n"); \
        return build_string(sb); \
    } \
\
\
\
    static inline String TYPE##__tostring_in0(Allocator allocator, char* name, TYPE* var) { \
        StringBuilder sb_var = string_builder_new_in(allocator); \
        StringBuilder* sb = &sb_var; \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##__sbappend(sb, var); \
        string_builder_append_cstr(sb, "\n"); \
        return build_string(sb); \
    } \
    static inline String TYPE##__pretty_tostring_in0(Allocator allocator, char* name, TYPE* var) { \
        StringBuilder sb_var = string_builder_new_in(allocator); \
        StringBuilder* sb = &sb_var; \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##__pretty_sbappend(sb, var, 0); \
        string_builder_append_cstr(sb, "\n"); \
        return build_string(sb); \
    } \
    static inline String TYPE##__pretty_tostring_in_with0(Allocator allocator, char* name, TYPE* var, IndentToken indent_token) { \
        StringBuilder sb_var = string_builder_new_in(allocator); \
        StringBuilder* sb = &sb_var; \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##__pretty_sbappend_with(sb, var, 0, indent_token); \
        string_builder_append_cstr(sb, "\n"); \
        return build_string(sb); \
    } \
\
    static inline String TYPE##_array__tostring_in0(Allocator allocator, char* name, TYPE* var, usize count) { \
        StringBuilder sb_var = string_builder_new_in(allocator); \
        StringBuilder* sb = &sb_var; \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##_array__sbappend(sb, var, count); \
        string_builder_append_cstr(sb, "\n"); \
        return build_string(sb); \
    } \
    static inline String TYPE##_array__pretty_tostring_in0(Allocator allocator, char* name, TYPE* var, usize count) { \
        StringBuilder sb_var = string_builder_new_in(allocator); \
        StringBuilder* sb = &sb_var; \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##_array__pretty_sbappend(sb, var, count, 0); \
        string_builder_append_cstr(sb, "\n"); \
        return build_string(sb); \
    } \
    static inline String TYPE##_array__pretty_tostring_in_with0(Allocator allocator, char* name, TYPE* var, usize count, IndentToken indent_token) { \
        StringBuilder sb_var = string_builder_new_in(allocator); \
        StringBuilder* sb = &sb_var; \
        if (name) string_builder_append_format(sb, "%s: ", name); \
        TYPE##_array__pretty_sbappend_with(sb, var, count, 0, indent_token); \
        string_builder_append_cstr(sb, "\n"); \
        return build_string(sb); \
    } \
\
\
\
    typedef enum { TYPE##__VARIANT__IMPL_____generate_print_functions = 0 } TYPE##__IMPL_____generate_print_functions

// -----

