#pragma once

// TODO: Remove and replace with a SIMD function that multiplies a constant.

#define _USE_MATH_DEFINES // For some (utterly stupid) reason, macros for numerical constants are not automatically defined... 
#include <math.h>

static inline float to_degrees ( float radians ) // ✅ converts radians to degrees
{
    return radians * (180 / (float)M_PI);
}

static inline float to_radians ( float degrees ) // ✅ converts degrees to radians
{
    return  degrees * ((float)M_PI / 180);
}