#pragma once

#include <stdbool.h>

#include "justcstd.h"
#include "logging.h"

typedef     unsigned char           uint8;
typedef     unsigned short          uint16;
typedef     unsigned int            uint32;
typedef     unsigned long long      uint64;

typedef     char                    int8;
typedef     short                   int16;
typedef     int                     int32;
typedef     long long               int64;

typedef     float                   float32;
typedef     double                  float64;

typedef     uint64                  usize;
typedef     unsigned char           byte;
// typedef     uint8                   bool;

#define UINT8_MIN       ((uint8)    0x0)
#define UINT16_MIN      ((uint16)   0x0)
#define UINT32_MIN      ((uint32)   0x0)
#define UINT64_MIN      ((uint64)   0x0)

#define UINT8_MAX       ((uint8)    0xff)
#define UINT16_MAX      ((uint16)   0xffff)
#define UINT32_MAX      ((uint32)   0xffffffff)
#define UINT64_MAX      ((uint64)   0xffffffffffffffff)

#define USIZE_MIN       ((usize)    UINT64_MIN)
#define USIZE_MAX       ((usize)    UINT64_MAX)

#define Option(Type) DeclType_Option_##Type
#define Option_None {0}
#define Option_Some(val) { .is_some = true, .value = val, }
#define DECLARE__Option(Type) typedef struct { bool is_some; Type value; } Option(Type);

DECLARE__Option(uint8);
DECLARE__Option(uint16);
DECLARE__Option(uint32);
DECLARE__Option(uint64);
DECLARE__Option(int8);
DECLARE__Option(int16);
DECLARE__Option(int32);
DECLARE__Option(int64);
DECLARE__Option(float32);
DECLARE__Option(float64);
DECLARE__Option(usize);
DECLARE__Option(byte);
DECLARE__Option(char);

#define STRUCT_ZERO_INIT {0}    // Compile Time Zero Init
#define STARTUP_INIT {0}        // Left for Runtime Init on Startup
#define LAZY_INIT {0}           // Lazy Init on initial access
#define LATER_INIT {0}          // Init at some later time
#define UNINIT {0}              // Left uninitialized

#define ARRAY_LENGTH(arr) (sizeof((arr)) / sizeof((arr)[0]))

#define IS_EVEN(a)  ((a & 1))
#define IS_ODD(a)   (!IS_EVEN(a))

#define MAX(a, b) (((a) >= (b)) ? (a) : (b))
#define MIN(a, b) (((a) <= (b)) ? (a) : (b))

#define SIGNOF(x) ( ((x) == 0) ? 0 : ( ((x) > 0) ? 1 : -1 ) )

#define branchless_if(cond, on_true, on_false) ( ( (!!(cond)) * (on_true) ) + ( (!!!(cond)) * (on_false) ) )

#define typeof_equals(var, Type) __builtin_types_compatible_p(__typeof__((var)), Type)

#define typeof_field(Type, field_name) (__typeof__(((Type*)0)->field_name))
#define offsetof(Type, field_name) ((usize)&((Type*)0)->field_name)

#define PANIC(...) do { JUST_LOG_PANIC("[%s:%d]\n", __FILE__, __LINE__); JUST_LOG_PANIC(__VA_ARGS__); std_exit(STD_EXIT_FAILURE); } while(0)
#define UNREACHABLE() do { JUST_LOG_PANIC("UNREACHABLE: [%s:%d]\n", __FILE__, __LINE__); std_exit(STD_EXIT_FAILURE); } while(0)
#define ASSERT(expr) do { if (!((expr))) { JUST_LOG_PANIC("Assertion Failed: [%s:%d]\n", __FILE__, __LINE__); std_exit(STD_EXIT_FAILURE); } } while(0)

typedef struct {
    usize id;
    usize generation;
} EntityId;

static inline EntityId new_entity_id(usize id, usize generation) {
    return (EntityId) { id, generation };
}

// Memory is owned
typedef struct {
    usize length;
    byte* bytes;
} Buffer;

// Memory is only viewed
typedef Buffer BufferSlice;

typedef struct {
    usize length;
    byte* cursor;
    byte* bytes;
} FillBuffer;

static inline BufferSlice buffer_as_slice(Buffer buffer, usize start, usize length) {
    return (BufferSlice) {
        .bytes = buffer.bytes + start,
        .length = length,
    };
}

static inline BufferSlice buffer_into_slice(Buffer buffer) {
    return buffer_as_slice(buffer, 0, buffer.length);
}

static inline Buffer buffer_clone(Buffer buffer) {
    byte* bytes_clone = std_malloc(buffer.length);
    std_memcpy(bytes_clone, buffer.bytes, buffer.length);
    return (Buffer) {
        .length = buffer.length,
        .bytes = bytes_clone,
    };
}

static inline usize filled_length(FillBuffer* buffer) {
    return buffer->cursor - buffer->bytes;
}