
#include "introspect.h"
#include "intospect_print.h"

static inline void sb_print_indent(StringBuilder* sb, uint32 indent_count, IndentToken indent_token) {
    for (uint32 i = 0; i < indent_count * indent_token.count; i++) {
        string_builder_append_format(sb, "%s", indent_token.token);
    }
}

#define __IMPL_____sbappend_functions__sbappend(TYPE, format) \
    void TYPE##__sbappend(StringBuilder* sb, TYPE var) { \
        string_builder_append_format(sb, format, var); \
    } \
    void TYPE##_ptr__sbappend(StringBuilder* sb, TYPE* ptr) { \
        ptr__sbappend(sb, ptr); \
        if (ptr != NULL) { \
            string_builder_append_cstr(sb, " "); \
            TYPE##__sbappend(sb, *ptr); \
        } \
    } \
    void TYPE##_array__sbappend(StringBuilder* sb, TYPE* arr, usize count) { \
        string_builder_append_cstr(sb, "[ "); \
        for (usize i = 0; i < count; i++) { \
            TYPE##__sbappend(sb, arr[i]); \
            if (i != count-1) { \
                string_builder_append_cstr(sb, ", "); \
            } \
        } \
        string_builder_append_cstr(sb, " ]"); \
    } \
    void TYPE##_array__pretty_sbappend(StringBuilder* sb, TYPE* arr, usize count, uint32 indent, IndentToken indent_token) { \
        if (count == 0) { \
            string_builder_append_cstr(sb, "[]"); \
            return; \
        } \
        string_builder_append_cstr(sb, "[\n"); \
        for (usize i = 0; i < count; i++) { \
            sb_print_indent(sb, indent+1, indent_token); \
            TYPE##__sbappend(sb, arr[i]); \
            string_builder_append_cstr(sb, ",\n"); \
        } \
        sb_print_indent(sb, indent, indent_token); \
        string_builder_append_cstr(sb, "]"); \
    } \
    void TYPE##_dynarray__sbappend(StringBuilder* sb, TYPE* arr, usize count) { \
        ptr__sbappend(sb, arr); \
        if (arr != NULL) { \
            string_builder_append_cstr(sb, " "); \
            TYPE##_array__sbappend(sb, arr, count); \
        } \
    } \
    void TYPE##_dynarray__pretty_sbappend(StringBuilder* sb, TYPE* arr, usize count, uint32 indent, IndentToken indent_token) { \
        ptr__sbappend(sb, arr); \
        if (arr != NULL) { \
            string_builder_append_cstr(sb, " "); \
            TYPE##_array__pretty_sbappend(sb, arr, count, indent, indent_token); \
        } \
    } \
    typedef enum { TYPE##__VARIANT__IMPL_____sbappend_functions__sbappend = 0 } TYPE##__ENUM__IMPL_____sbappend_functions__sbappend

void ptr__sbappend(StringBuilder* sb, void* var) {
    if (var == NULL) {
        string_builder_append_cstr(sb, "<NULL>");
    }
    else {
        string_builder_hinted_append_format(sb, 20, "<0x%p>", var);
    }
}

void ptr_array__sbappend(StringBuilder* sb, void** arr, usize count) {
    string_builder_append_cstr(sb, "[ ");
    for (usize i = 0; i < count; i++) {
        ptr__sbappend(sb, arr[i]);
        if (i != count-1) {
            string_builder_append_cstr(sb, ", ");
        }
    }
    string_builder_append_cstr(sb, " ]");
}
void ptr_array__pretty_sbappend(StringBuilder* sb, void** arr, usize count, uint32 indent, IndentToken indent_token) {
    if (count == 0) {
        string_builder_append_cstr(sb, "[]");
        return;
    }
    string_builder_append_cstr(sb, "[\n");
    for (usize i = 0; i < count; i++) {
        sb_print_indent(sb, indent+1, indent_token);
        ptr__sbappend(sb, arr[i]);
        string_builder_append_cstr(sb, ",\n");
    }
    sb_print_indent(sb, indent, indent_token);
    string_builder_append_cstr(sb, "]");
}

__IMPL_____sbappend_functions__sbappend(char, "%c");
__IMPL_____sbappend_functions__sbappend(byte, "%.2x");
// __IMPL_____sbappend_functions__sbappend(bool, "%hhu");
__IMPL_____sbappend_functions__sbappend(uint8, "%hhu");
__IMPL_____sbappend_functions__sbappend(uint16, "%hu");
__IMPL_____sbappend_functions__sbappend(uint32, "%u");
__IMPL_____sbappend_functions__sbappend(uint64, "%llu");
__IMPL_____sbappend_functions__sbappend(int8, "%hhd");
__IMPL_____sbappend_functions__sbappend(int16, "%hd");
__IMPL_____sbappend_functions__sbappend(int32, "%d");
__IMPL_____sbappend_functions__sbappend(int64, "%lld");
__IMPL_____sbappend_functions__sbappend(float32, "%f");
__IMPL_____sbappend_functions__sbappend(float64, "%Lf");
__IMPL_____sbappend_functions__sbappend(usize, "%llu");

void char_cstr__sbappend(StringBuilder* sb, char* cstr) {
    ptr__sbappend(sb, cstr);
    if (cstr != NULL) {
        string_builder_append_cstr(sb, " ");
        string_builder_append_cstr(sb, "\"");
        for (usize i = 0; cstr[i] != '\0'; i++) {
            string_builder_append_format(sb, "%c", cstr[i]);
        }
        string_builder_append_cstr(sb, "\"");
    }
}
void char_string__sbappend(StringBuilder* sb, char* str, usize count) {
    ptr__sbappend(sb, str);
    if (str != NULL) {
        string_builder_append_cstr(sb, " ");
        string_builder_append_cstr(sb, "\"");
        for (usize i = 0; i < count; i++) {
            string_builder_append_format(sb, "%c", str[i]);
        }
        string_builder_append_cstr(sb, "\"");
    }
}

void bool__sbappend(StringBuilder* sb, bool var) {
    if (var) {
        string_builder_append_cstr(sb, "true");
    }
    else {
        string_builder_append_cstr(sb, "false");
    }
}
void bool_ptr__sbappend(StringBuilder* sb, bool* ptr) {
    ptr__sbappend(sb, ptr);
    if (ptr != NULL) {
        string_builder_append_cstr(sb, " ");
        bool__sbappend(sb, *ptr);
    }
}
void bool_array__sbappend(StringBuilder* sb, bool* arr, usize count) {
    string_builder_append_cstr(sb, "[ ");
    for (usize i = 0; i < count; i++) {
        bool__sbappend(sb, arr[i]);
        if (i != count-1) {
            string_builder_append_cstr(sb, ", ");
        }
    }
    string_builder_append_cstr(sb, " ]");
}
void bool_array__pretty_sbappend(StringBuilder* sb, bool* arr, usize count, uint32 indent, IndentToken indent_token) {
    if (count == 0) {
        string_builder_append_cstr(sb, "[]");
        return;
    }
    string_builder_append_cstr(sb, "[\n");
    for (usize i = 0; i < count; i++) {
        sb_print_indent(sb, indent+1, indent_token);
        bool__sbappend(sb, arr[i]);
        string_builder_append_cstr(sb, ",\n");
    }
    sb_print_indent(sb, indent, indent_token);
    string_builder_append_cstr(sb, "]");
}
void bool_dynarray__sbappend(StringBuilder* sb, bool* arr, usize count) {
    ptr__sbappend(sb, arr);
    if (arr != NULL) {
        string_builder_append_cstr(sb, " ");
        bool_array__sbappend(sb, arr, count);
    }
}
void bool_dynarray__pretty_sbappend(StringBuilder* sb, bool* arr, usize count, uint32 indent, IndentToken indent_token) {
    ptr__sbappend(sb, arr);
    if (arr != NULL) {
        string_builder_append_cstr(sb, " ");
        bool_array__pretty_sbappend(sb, arr, count, indent, indent_token);
    }
}

void struct__sbappend(StringBuilder* sb, void* var, FieldInfo* fields, uint32 field_count) {
    string_builder_append_cstr(sb, "{ ");
    for (uint32 i = 0; i < field_count; i++) {
        FieldInfo field = fields[i];
        string_builder_append_cstr(sb, field.name);
        string_builder_append_cstr(sb, ": ");
        introspect_field_sbappend(sb, field, var);
        if (i != field_count-1) {
            string_builder_append_cstr(sb, ", ");
        }
    }
    string_builder_append_cstr(sb, " }");
}
void struct__pretty_sbappend(StringBuilder* sb, void* var, FieldInfo* fields, uint32 field_count, uint32 indent, IndentToken indent_token) {
    string_builder_append_cstr(sb, "{\n");
    for (uint32 i = 0; i < field_count; i++) {
        FieldInfo field = fields[i];
        sb_print_indent(sb, indent+1, indent_token);
        // TODO: think better way
        if (!field.is_discriminated_union) {
            string_builder_append_cstr(sb, field.name);
            string_builder_append_cstr(sb, ": ");
        }
        introspect_field_pretty_sbappend(sb, field, var, indent+1, indent_token);
        string_builder_append_cstr(sb, ",\n");
    }
    sb_print_indent(sb, indent, indent_token);
    string_builder_append_cstr(sb, "}");
}
void struct_ptr__sbappend(StringBuilder* sb, void** ptr, FieldInfo* fields, uint32 field_count) {
    ptr__sbappend(sb, *ptr);
    if (*ptr != NULL) {
        string_builder_append_cstr(sb, " ");
        struct__sbappend(sb, *ptr, fields, field_count);
    }
}
void struct_ptr__pretty_sbappend(StringBuilder* sb, void** ptr, FieldInfo* fields, uint32 field_count, uint32 indent, IndentToken indent_token) {
    ptr__sbappend(sb, *ptr);
    if (*ptr != NULL) {
        string_builder_append_cstr(sb, " ");
        struct__pretty_sbappend(sb, *ptr, fields, field_count, indent, indent_token);
    }
}
void struct_array__sbappend(StringBuilder* sb, void* arr, usize count, uint32 struct_size, FieldInfo* fields, uint32 field_count) {
    string_builder_append_cstr(sb, "[ ");
    for (uint32 i = 0; i < count; i++) {
        void* var = (((byte*)arr) + (struct_size * i));
        struct__sbappend(sb, var, fields, field_count);
    }
    string_builder_append_cstr(sb, " ]");
}
void struct_array__pretty_sbappend(StringBuilder* sb, void* arr, usize count, uint32 struct_size, FieldInfo* fields, uint32 field_count, uint32 indent, IndentToken indent_token) {
    if (count == 0) {
        string_builder_append_cstr(sb, "[]");
        return;
    }
    string_builder_append_cstr(sb, "[\n");
    for (uint32 i = 0; i < count; i++) {
        void* var = (((byte*)arr) + (struct_size * i));
        sb_print_indent(sb, indent+1, indent_token);
        struct__pretty_sbappend(sb, var, fields, field_count, indent+1, indent_token);
        string_builder_append_cstr(sb, ",\n");
    }
    sb_print_indent(sb, indent, indent_token);
    string_builder_append_cstr(sb, "]");
}
void struct_dynarray__sbappend(StringBuilder* sb, void* arr, usize count, uint32 struct_size, FieldInfo* fields, uint32 field_count) {
    ptr__sbappend(sb, arr);
    if (arr != NULL) {
        string_builder_append_cstr(sb, " ");
        struct_array__sbappend(sb, arr, count, struct_size, fields, field_count);
    }
}
void struct_dynarray__pretty_sbappend(StringBuilder* sb, void* arr, usize count, uint32 struct_size, FieldInfo* fields, uint32 field_count, uint32 indent, IndentToken indent_token) {
    ptr__sbappend(sb, arr);
    if (arr != NULL) {
        string_builder_append_cstr(sb, " ");
        struct_array__pretty_sbappend(sb, arr, count, struct_size, fields, field_count, indent, indent_token);
    }
}

void union__sbappend(StringBuilder* sb, void* struct_var, void* var, FieldInfo self, uint32 variant_index, FieldInfo* variants, uint32 variant_count) {
    FieldInfo variant = variants[variant_index];
    variant.ptr = self.ptr;
    if (self.is_discriminated_union) {
        if (self.is_named_union) {
            string_builder_append_cstr(sb, self.union_name);
            string_builder_append_cstr(sb, ".");
        }
        string_builder_append_cstr(sb, variant.name);
        string_builder_append_cstr(sb, ": ");
    }
    introspect_field_sbappend(sb, variant, struct_var);
}
void union__pretty_sbappend(StringBuilder* sb, void* struct_var, void* var, FieldInfo self, uint32 variant_index, FieldInfo* variants, uint32 variant_count, uint32 indent, IndentToken indent_token) {
    FieldInfo variant = variants[variant_index];
    variant.ptr = self.ptr;
    if (self.is_discriminated_union) {
        if (self.is_named_union) {
            string_builder_append_cstr(sb, self.union_name);
            string_builder_append_cstr(sb, ".");
        }
        string_builder_append_cstr(sb, variant.name);
        string_builder_append_cstr(sb, ": ");
    }
    introspect_field_pretty_sbappend(sb, variant, struct_var, indent, indent_token);
}
void union_ptr__sbappend(StringBuilder* sb, void* struct_var, void** ptr, FieldInfo self, uint32 variant_index, FieldInfo* variants, uint32 variant_count) {
    ptr__sbappend(sb, *ptr);
    if (*ptr != NULL) {
        string_builder_append_cstr(sb, " ");
        union__sbappend(sb, struct_var, *ptr, self, variant_index, variants, variant_count);
    }
}
void union_ptr__pretty_sbappend(StringBuilder* sb, void* struct_var, void** ptr, FieldInfo self, uint32 variant_index, FieldInfo* variants, uint32 variant_count, uint32 indent, IndentToken indent_token) {
    ptr__sbappend(sb, *ptr);
    if (*ptr != NULL) {
        string_builder_append_cstr(sb, " ");
        union__pretty_sbappend(sb, struct_var, *ptr, self, variant_index, variants, variant_count, indent, indent_token);
    }
}
void union_array__sbappend(StringBuilder* sb, void* struct_var, void* arr, usize count, uint32 union_size, FieldInfo self, uint32 variant_index, FieldInfo* variants, uint32 variant_count) {
    string_builder_append_cstr(sb, "[ ");
    for (uint32 i = 0; i < count; i++) {
        void* var = (((byte*)arr) + (union_size * i));
        union__sbappend(sb, struct_var, var, self, variant_index, variants, variant_count);
    }
    string_builder_append_cstr(sb, " ]");
}
void union_array__pretty_sbappend(StringBuilder* sb, void* struct_var, void* arr, usize count, uint32 union_size, FieldInfo self, uint32 variant_index, FieldInfo* variants, uint32 variant_count, uint32 indent, IndentToken indent_token) {
    if (count == 0) {
        string_builder_append_cstr(sb, "[]");
        return;
    }
    string_builder_append_cstr(sb, "[\n");
    for (uint32 i = 0; i < count; i++) {
        void* var = (((byte*)arr) + (union_size * i));
        sb_print_indent(sb, indent+1, indent_token);
        union__pretty_sbappend(sb, struct_var, var, self, variant_index, variants, variant_count, indent, indent_token);
        string_builder_append_cstr(sb, ",\n");
    }
    sb_print_indent(sb, indent, indent_token);
    string_builder_append_cstr(sb, "]");
}
void union_dynarray__sbappend(StringBuilder* sb, void* struct_var, void* arr, usize count, uint32 union_size, FieldInfo self, uint32 variant_index, FieldInfo* variants, uint32 variant_count) {
    ptr__sbappend(sb, arr);
    if (arr != NULL) {
        string_builder_append_cstr(sb, " ");
        union_array__sbappend(sb, struct_var, arr, count, union_size, self, variant_index, variants, variant_count);
    }
}
void union_dynarray__pretty_sbappend(StringBuilder* sb, void* struct_var, void* arr, usize count, uint32 union_size, FieldInfo self, uint32 variant_index, FieldInfo* variants, uint32 variant_count, uint32 indent, IndentToken indent_token) {
    ptr__sbappend(sb, arr);
    if (arr != NULL) {
        string_builder_append_cstr(sb, " ");
        union_array__pretty_sbappend(sb, struct_var, arr, count, union_size, self, variant_index, variants, variant_count, indent, indent_token);
    }
}

#define field_sbappend(TYPE, sb, field) \
    do { \
        void* field_ptr = (void*)(((usize)var) + ((usize)(field).ptr)); \
        if ((field).is_dynarray) { \
            TYPE** items_ptr = field_ptr; \
            usize* count = (void*)(((usize)var) + ((usize)(field).count_ptr)); \
            TYPE##_dynarray__sbappend((sb), *items_ptr, *count); \
        } \
        else if ((field).is_array) { \
            if ((field).is_ptr) { \
                void** var_ptr = field_ptr; \
                ptr_array__sbappend((sb), var_ptr, (field).count); \
            } \
            else { \
                TYPE* var_ptr = field_ptr; \
                TYPE##_array__sbappend((sb), var_ptr, (field).count); \
            } \
        } \
        else if ((field).is_ptr) { \
            TYPE** var_ptr = field_ptr; \
            TYPE##_ptr__sbappend((sb), *var_ptr); \
        } \
        else { \
            TYPE* var_ptr = field_ptr; \
            TYPE##__sbappend((sb), *var_ptr); \
        } \
    } while (0)

#define field_pretty_sbappend(TYPE, sb, field, ...) \
    do { \
        void* field_ptr = (void*)(((usize)var) + ((usize)(field).ptr)); \
        if ((field).is_dynarray) { \
            TYPE** items_ptr = field_ptr; \
            usize* count = (void*)(((usize)var) + ((usize)(field).count_ptr)); \
            TYPE##_dynarray__pretty_sbappend((sb), *items_ptr, *count, __VA_ARGS__); \
        } \
        else if ((field).is_array) { \
            if ((field).is_ptr) { \
                void** var_ptr = field_ptr; \
                ptr_array__pretty_sbappend((sb), var_ptr, (field).count, __VA_ARGS__); \
            } \
            else { \
                TYPE* var_ptr = field_ptr; \
                TYPE##_array__pretty_sbappend((sb), var_ptr, (field).count, __VA_ARGS__); \
            } \
        } \
        else if ((field).is_ptr) { \
            TYPE** var_ptr = field_ptr; \
            TYPE##_ptr__sbappend((sb), *var_ptr); \
        } \
        else { \
            TYPE* var_ptr = field_ptr; \
            TYPE##__sbappend((sb), *var_ptr); \
        } \
    } while (0)

void introspect_field_sbappend(StringBuilder* sb, FieldInfo field, void* var) {
    switch (field.type) {
    case TYPE_void: {
        if (!field.is_ptr) {
            PANIC("TYPE_void has to be a pointer, void field is not allowed.\n");
        }
        void* field_ptr = (void*)(((usize)var) + ((usize)field.ptr));
        void** var_ptr = field_ptr;
        ptr__sbappend(sb, *var_ptr);
        break;
    }
    case TYPE_char: {
        void* field_ptr = (void*)(((usize)var) + ((usize)field.ptr));

        if (field.is_cstr) {
            char** cstr_ptr = field_ptr;
            char_cstr__sbappend(sb, *cstr_ptr);
        }
        else if (field.is_string) {
            char** str_ptr = field_ptr;
            usize* count = (void*)(((usize)var) + ((usize)field.count_ptr));
            char_string__sbappend(sb, *str_ptr, *count);
        }
        else if (field.is_dynarray) {
            char** items_ptr = field_ptr;
            usize* count = (void*)(((usize)var) + ((usize)field.count_ptr));
            char_dynarray__sbappend(sb, *items_ptr, *count);
        }
        else if (field.is_array) {
            if (field.is_ptr) {
                void** var_ptr = field_ptr;
                ptr_array__sbappend(sb, var_ptr, field.count);
            }
            else {
                char* var_ptr = field_ptr;
                char_array__sbappend(sb, var_ptr, field.count);
            }
        }
        else if (field.is_ptr) {
            char** var_ptr = field_ptr;
            char_ptr__sbappend(sb, *var_ptr);
        }
        else {
            char* var_ptr = field_ptr;
            char__sbappend(sb, *var_ptr);
        }
        break;
    }
    case TYPE_bool:
        field_sbappend(bool, sb, field);
        break;
    case TYPE_byte:
        field_sbappend(byte, sb, field);
        break;
    case TYPE_uint8:
        field_sbappend(uint8, sb, field);
        break;
    case TYPE_uint16:
        field_sbappend(uint16, sb, field);
        break;
    case TYPE_uint32:
        field_sbappend(uint32, sb, field);
        break;
    case TYPE_uint64:
        field_sbappend(uint64, sb, field);
        break;
    case TYPE_int8:
        field_sbappend(int8, sb, field);
        break;
    case TYPE_int16:
        field_sbappend(int16, sb, field);
        break;
    case TYPE_int32:
        field_sbappend(int32, sb, field);
        break;
    case TYPE_int64:
        field_sbappend(int64, sb, field);
        break;
    case TYPE_usize:
        void* field_ptr = (void*)(((usize)var) + ((usize)field.ptr));
        usize* p = field_ptr;
        field_sbappend(usize, sb, field);
        break;
    case TYPE_float32:
        field_sbappend(float32, sb, field);
        break;
    case TYPE_float64:
        field_sbappend(float64, sb, field);
        break;
    case TYPE_struct: {
        void* field_ptr = (void*)(((usize)var) + ((usize)field.ptr));
        if ((field).is_dynarray) {
            void** items_ptr = field_ptr;
            usize* count = (void*)(((usize)var) + ((usize)(field).count_ptr));
            struct_dynarray__sbappend(sb, *items_ptr, *count, field.struct_size, field.fields, field.field_count);
        }
        else if ((field).is_array) {
            if (field.is_ptr) {
                void** items_ptr = field_ptr;
                ptr_array__sbappend(sb, items_ptr, field.count);
            }
            else {
                struct_array__sbappend(sb, field_ptr, field.count, field.struct_size, field.fields, field.field_count);
            }
        }
        else if ((field).is_ptr) {
            void** var_ptr = field_ptr;
            struct_ptr__sbappend(sb, var_ptr, field.fields, field.field_count);
        }
        else {
            struct__sbappend(sb, field_ptr, field.fields, field.field_count);
        }
        break;
    }
    case TYPE_union: {
        void* field_ptr = (void*)(((usize)var) + ((usize)field.ptr));
        uint32 variant_index = field.union_header_variant;
        if (field.is_discriminated_union) {
            uint32* discriminant_value = (void*)(((usize)var) + ((usize)(field).discriminant_ptr));
            variant_index = *discriminant_value;
        }
        variant_index = MIN(variant_index, field.variant_count-1);

        if ((field).is_dynarray) {
            void** items_ptr = field_ptr;
            usize* count = (void*)(((usize)var) + ((usize)(field).count_ptr));
            union_dynarray__sbappend(sb, var, *items_ptr, *count, field.union_size, field, variant_index, field.variants, field.variant_count);
        }
        else if ((field).is_array) {
            if (field.is_ptr) {
                void** items_ptr = field_ptr;
                ptr_array__sbappend(sb, items_ptr, field.count);
            }
            else {
                union_array__sbappend(sb, var, field_ptr, field.count, field.union_size, field, variant_index, field.variants, field.variant_count);
            }
        }
        else if ((field).is_ptr) {
            void** var_ptr = field_ptr;
            union_ptr__sbappend(sb, var, var_ptr, field, variant_index, field.variants, field.variant_count);
        }
        else {
            union__sbappend(sb, var, field_ptr, field, variant_index, field.variants, field.variant_count);
        }
        break;
    }
    default:
        UNREACHABLE();
        break;
    }
}

void introspect_field_pretty_sbappend(StringBuilder* sb, FieldInfo field, void* var, uint32 indent, IndentToken indent_token) {
    switch (field.type) {
    case TYPE_char: {
        void* field_ptr = (void*)(((usize)var) + ((usize)field.ptr));
        if (field.is_cstr) {
            char** cstr_ptr = field_ptr;
            char_cstr__sbappend(sb, *cstr_ptr);
        }
        else if (field.is_string) {
            char** str_ptr = field_ptr;
            usize* count = (void*)(((usize)var) + ((usize)field.count_ptr));
            char_string__sbappend(sb, *str_ptr, *count);
        }
        else if (field.is_dynarray) {
            char** items_ptr = field_ptr;
            usize* count = (void*)(((usize)var) + ((usize)field.count_ptr));
            char_dynarray__pretty_sbappend(sb, *items_ptr, *count, indent, indent_token);
        }
        else if (field.is_array) {
            if (field.is_ptr) {
                void** var_ptr = field_ptr;
                ptr_array__pretty_sbappend(sb, var_ptr, field.count, indent, indent_token);
            }
            else {
                char* var_ptr = field_ptr;
                char_array__pretty_sbappend(sb, var_ptr, field.count, indent, indent_token);
            }
        }
        else if (field.is_ptr) {
            char** var_ptr = field_ptr;
            char_ptr__sbappend(sb, *var_ptr);
        }
        else {
            char* var_ptr = field_ptr;
            char__sbappend(sb, *var_ptr);
        }
        break;
    }
    case TYPE_bool:
        field_pretty_sbappend(bool, sb, field, indent, indent_token);
        break;
    case TYPE_byte:
        field_pretty_sbappend(byte, sb, field, indent, indent_token);
        break;
    case TYPE_uint8:
        field_pretty_sbappend(uint8, sb, field, indent, indent_token);
        break;
    case TYPE_uint16:
        field_pretty_sbappend(uint16, sb, field, indent, indent_token);
        break;
    case TYPE_uint32:
        field_pretty_sbappend(uint32, sb, field, indent, indent_token);
        break;
    case TYPE_uint64:
        field_pretty_sbappend(uint64, sb, field, indent, indent_token);
        break;
    case TYPE_int8:
        field_pretty_sbappend(int8, sb, field, indent, indent_token);
        break;
    case TYPE_int16:
        field_pretty_sbappend(int16, sb, field, indent, indent_token);
        break;
    case TYPE_int32:
        field_pretty_sbappend(int32, sb, field, indent, indent_token);
        break;
    case TYPE_int64:
        field_pretty_sbappend(int64, sb, field, indent, indent_token);
        break;
    case TYPE_usize:
        void* field_ptr = (void*)(((usize)var) + ((usize)field.ptr));
        usize* p = field_ptr;
        field_pretty_sbappend(usize, sb, field, indent, indent_token);
        break;
    case TYPE_float32:
        field_pretty_sbappend(float32, sb, field, indent, indent_token);
        break;
    case TYPE_float64:
        field_pretty_sbappend(float64, sb, field, indent, indent_token);
        break;
    case TYPE_struct: {
        void* field_ptr = (void*)(((usize)var) + ((usize)field.ptr));
        if ((field).is_dynarray) {
            void** items_ptr = field_ptr;
            usize* count = (void*)(((usize)var) + ((usize)(field).count_ptr));
            struct_dynarray__pretty_sbappend(sb, *items_ptr, *count, field.struct_size, field.fields, field.field_count, indent, indent_token);
        }
        else if ((field).is_array) {
            if (field.is_ptr) {
                void** items_ptr = field_ptr;
                ptr_array__pretty_sbappend(sb, items_ptr, field.count, indent, indent_token);
            }
            else {
                struct_array__pretty_sbappend(sb, field_ptr, field.count, field.struct_size, field.fields, field.field_count, indent, indent_token);
            }
        }
        else if ((field).is_ptr) {
            void** var_ptr = field_ptr;
            struct_ptr__pretty_sbappend(sb, var_ptr, field.fields, field.field_count, indent, indent_token);
        }
        else {
            struct__pretty_sbappend(sb, field_ptr, field.fields, field.field_count, indent, indent_token);
        }
        break;
    }
    case TYPE_union: {
        void* field_ptr = (void*)(((usize)var) + ((usize)field.ptr));
        uint32 variant_index = field.union_header_variant;
        if (field.is_discriminated_union) {
            uint32* discriminant_value = (void*)(((usize)var) + ((usize)(field).discriminant_ptr));
            variant_index = *discriminant_value;
        }
        variant_index = MIN(variant_index, field.variant_count-1);

        if ((field).is_dynarray) {
            void** items_ptr = field_ptr;
            usize* count = (void*)(((usize)var) + ((usize)(field).count_ptr));
            union_dynarray__pretty_sbappend(sb, var, *items_ptr, *count, field.union_size, field, variant_index, field.variants, field.variant_count, indent, indent_token);
        }
        else if ((field).is_array) {
            if (field.is_ptr) {
                void** items_ptr = field_ptr;
                ptr_array__pretty_sbappend(sb, items_ptr, field.count, indent, indent_token);
            }
            else {
                union_array__pretty_sbappend(sb, var, field_ptr, field.count, field.union_size, field, variant_index, field.variants, field.variant_count, indent, indent_token);
            }
        }
        else if ((field).is_ptr) {
            void** var_ptr = field_ptr;
            union_ptr__pretty_sbappend(sb, var, var_ptr, field, variant_index, field.variants, field.variant_count, indent, indent_token);
        }
        else {
            union__pretty_sbappend(sb, var, field_ptr, field, variant_index, field.variants, field.variant_count, indent, indent_token);
        }
        break;
    }
    default:
        UNREACHABLE();
        break;
    }
}
