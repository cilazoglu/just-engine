#pragma once

#include "base.h"

typedef enum {
    Rotation_CW = 1,
    Rotation_CCW = -1,
} RotationWay;

typedef struct {
    Anchor anchor;
    Vector2 position;       // position of the anchor
    Vector2 scale;
    float32 rotation;       // rotation around its anchor
    RotationWay rway;
} Transform2D;
