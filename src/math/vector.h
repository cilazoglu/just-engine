#pragma once

#include "math.h"

#include "raylib.h"

#include "core.h"

typedef struct {
    int32 x;
    int32 y;
} IVector2;

static inline Vector2 IVector2ToVector2(IVector2 v) {
    return (Vector2) { .x = v.x, .y = v.y };
}

static inline IVector2 Vector2ToIVector2(Vector2 v) {
    return (IVector2) { .x = v.x, .y = v.y };
}

//----------------------------------------------------------------------------------
// START - Module Functions Definition - IVector2 math
//----------------------------------------------------------------------------------

// Vector with components value 0
static inline IVector2 IVector2Zero(void)
{
    IVector2 result = { 0, 0 };

    return result;
}

// Vector with components value 1
static inline IVector2 IVector2One(void)
{
    IVector2 result = { 1, 1 };

    return result;
}

// Add two vectors (v1 + v2)
static inline IVector2 IVector2Add(IVector2 v1, IVector2 v2)
{
    IVector2 result = { v1.x + v2.x, v1.y + v2.y };

    return result;
}

// Add vector and scalar value
static inline IVector2 IVector2AddValue(IVector2 v, int32 add)
{
    IVector2 result = { v.x + add, v.y + add };

    return result;
}

// Subtract two vectors (v1 - v2)
static inline IVector2 IVector2Subtract(IVector2 v1, IVector2 v2)
{
    IVector2 result = { v1.x - v2.x, v1.y - v2.y };

    return result;
}

// Subtract vector by scalar value
static inline IVector2 IVector2SubtractValue(IVector2 v, int32 sub)
{
    IVector2 result = { v.x - sub, v.y - sub };

    return result;
}

// Calculate vector length
static inline float IVector2Length(IVector2 v)
{
    float result = sqrtf((v.x*v.x) + (v.y*v.y));

    return result;
}

// Calculate vector square length
static inline int32 IVector2LengthSqr(IVector2 v)
{
    int32 result = (v.x*v.x) + (v.y*v.y);

    return result;
}

// Calculate two vectors dot product
static inline int32 IVector2DotProduct(IVector2 v1, IVector2 v2)
{
    int32 result = (v1.x*v2.x + v1.y*v2.y);

    return result;
}

// Calculate distance between two vectors
static inline float IVector2Distance(IVector2 v1, IVector2 v2)
{
    float result = sqrtf((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y));

    return result;
}

// Calculate square distance between two vectors
static inline int32 IVector2DistanceSqr(IVector2 v1, IVector2 v2)
{
    int32 result = ((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y));

    return result;
}

// Calculate angle between two vectors
// NOTE: Angle is calculated from origin point (0, 0)
static inline float IVector2Angle(IVector2 v1, IVector2 v2)
{
    float result = 0.0f;

    float dot = v1.x*v2.x + v1.y*v2.y;
    float det = v1.x*v2.y - v1.y*v2.x;

    result = atan2f(det, dot);

    return result;
}

// Calculate angle defined by a two vectors line
// NOTE: Parameters need to be normalized
// Current implementation should be aligned with glm::angle
static inline float IVector2LineAngle(IVector2 start, IVector2 end)
{
    float result = 0.0f;

    // TODO(10/9/2023): Currently angles move clockwise, determine if this is wanted behavior
    result = -atan2f(end.y - start.y, end.x - start.x);

    return result;
}

// Scale vector (multiply by value)
static inline IVector2 IVector2Scale(IVector2 v, int32 scale)
{
    IVector2 result = { v.x*scale, v.y*scale };

    return result;
}

// Multiply vector by vector
static inline IVector2 IVector2Multiply(IVector2 v1, IVector2 v2)
{
    IVector2 result = { v1.x*v2.x, v1.y*v2.y };

    return result;
}

// Negate vector
static inline IVector2 IVector2Negate(IVector2 v)
{
    IVector2 result = { -v.x, -v.y };

    return result;
}

// Divide vector by vector
static inline IVector2 IVector2Divide(IVector2 v1, IVector2 v2)
{
    IVector2 result = { v1.x/v2.x, v1.y/v2.y };

    return result;
}

// Normalize provided vector
static inline Vector2 IVector2Normalize(IVector2 v)
{
    Vector2 result = { 0 };
    float length = sqrtf((v.x*v.x) + (v.y*v.y));

    if (length > 0)
    {
        float ilength = 1.0f/length;
        result.x = v.x*ilength;
        result.y = v.y*ilength;
    }

    return result;
}

// Transforms a IVector2 by a given Matrix
static inline Vector2 IVector2Transform(IVector2 v, Matrix mat)
{
    Vector2 result = { 0 };

    float x = v.x;
    float y = v.y;
    float z = 0;

    result.x = mat.m0*x + mat.m4*y + mat.m8*z + mat.m12;
    result.y = mat.m1*x + mat.m5*y + mat.m9*z + mat.m13;

    return result;
}

// Calculate linear interpolation between two vectors
static inline IVector2 IVector2Lerp(IVector2 v1, IVector2 v2, float amount)
{
    IVector2 result = { 0 };

    result.x = v1.x + amount*(v2.x - v1.x);
    result.y = v1.y + amount*(v2.y - v1.y);

    return result;
}

// Calculate reflected vector to normal
static inline IVector2 IVector2Reflect(IVector2 v, IVector2 normal)
{
    IVector2 result = { 0 };

    int32 dotProduct = (v.x*normal.x + v.y*normal.y); // Dot product

    result.x = v.x - (2*normal.x)*dotProduct;
    result.y = v.y - (2*normal.y)*dotProduct;

    return result;
}

// Rotate vector by angle
static inline Vector2 IVector2Rotate(IVector2 v, float angle)
{
    Vector2 result = { 0 };

    float cosres = cosf(angle);
    float sinres = sinf(angle);

    result.x = v.x*cosres - v.y*sinres;
    result.y = v.x*sinres + v.y*cosres;

    return result;
}

// Move Vector towards target
static inline Vector2 IVector2MoveTowards(IVector2 v, IVector2 target, float maxDistance)
{
    Vector2 result = { 0 };

    float dx = target.x - v.x;
    float dy = target.y - v.y;
    float value = (dx*dx) + (dy*dy);

    if ((value == 0) || ((maxDistance >= 0) && (value <= maxDistance*maxDistance))) return IVector2ToVector2(target);

    float dist = sqrtf(value);

    result.x = v.x + dx/dist*maxDistance;
    result.y = v.y + dy/dist*maxDistance;

    return result;
}

// Invert the given vector
static inline Vector2 IVector2Invert(IVector2 v)
{
    Vector2 result = { 1.0f/v.x, 1.0f/v.y };

    return result;
}

// Clamp the components of the vector between
// min and max values specified by the given vectors
static inline IVector2 IVector2Clamp(IVector2 v, IVector2 min, IVector2 max)
{
    IVector2 result = { 0 };

    result.x = MIN(max.x, MAX(min.x, v.x));
    result.y = MIN(max.y, MAX(min.y, v.y));

    return result;
}

// Clamp the magnitude of the vector between two min and max values
static inline Vector2 IVector2ClampValue(IVector2 v, float min, float max)
{
    Vector2 result = IVector2ToVector2(v);

    float length = (v.x*v.x) + (v.y*v.y);
    if (length > 0.0f)
    {
        length = sqrtf(length);

        if (length < min)
        {
            float scale = min/length;
            result.x = v.x*scale;
            result.y = v.y*scale;
        }
        else if (length > max)
        {
            float scale = max/length;
            result.x = v.x*scale;
            result.y = v.y*scale;
        }
    }

    return result;
}

// Check whether two given vectors are almost equal
static inline bool IVector2Equals(IVector2 p, IVector2 q)
{
    bool result = p.x == q.x && p.y == q.y;

    return result;
}

//----------------------------------------------------------------------------------
// END - Module Functions Definition - IVector2 math
//----------------------------------------------------------------------------------

#define Vector2_From(val) ((Vector2) {val, val})
#define Vector2_Ones ((Vector2) {1.0, 1.0})
#define Vector2_Unit_X ((Vector2) {1.0, 0.0})
#define Vector2_Unit_Y ((Vector2) {0.0, 1.0})
#define Vector2_Neg_Unit_X ((Vector2) {-1.0, 0.0})
#define Vector2_Neg_Unit_Y ((Vector2) {0.0, -1.0})
#define Vector2_On_X(val) ((Vector2) {val, 0.0})
#define Vector2_On_Y(val) ((Vector2) {0.0, val})
#define Vector2_XX(vec) ((Vector2) {vec.x, vec.x})
#define Vector2_YY(vec) ((Vector2) {vec.y, vec.y})
#define Vector2_YX(vec) ((Vector2) {vec.y, vec.x})

static inline Vector2 vector2_from(float32 val) {
    return (Vector2) {val, val};
}

static inline Vector2 vector2_ones() {
    return (Vector2) {1.0, 1.0};
}

static inline Vector2 vector2_unit_x() {
    return (Vector2) {1.0, 0.0};
}

static inline Vector2 vector2_unit_y() {
    return (Vector2) {0.0, 1.0};
}

static inline Vector2 vector2_neg_unit_x() {
    return (Vector2) {-1.0, 0.0};
}

static inline Vector2 vector2_neg_unit_y() {
    return (Vector2) {0.0, -1.0};
}

static inline Vector2 vector2_on_x(float32 val) {
    return (Vector2) {val, 0.0};
}

static inline Vector2 vector2_on_y(float32 val) {
    return (Vector2) {0.0, val};
}

static inline Vector2 vector2_xx(Vector2 vec) {
    return (Vector2) {vec.x, vec.x};
}

static inline Vector2 vector2_yy(Vector2 vec) {
    return (Vector2) {vec.y, vec.y};
}

static inline Vector2 vector2_yx(Vector2 vec) {
    return (Vector2) {vec.y, vec.x};
}