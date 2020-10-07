#pragma once

#define _USE_MATH_DEFINES // For some (utterly stupid) reason, macros for numerical constants are not automatically defined... 
#include <math.h>

static inline float toDegrees(float radians) // converts radians to degrees
{
	return (float) radians * (360 / M_PI);   // For another (utterly stupid) reson, OpenGL cuts your angle in half, so we double the numerator
}

static inline float toRadians(float degrees) // converts degrees to radians
{
	return (float) degrees * (M_PI / 360);
}
