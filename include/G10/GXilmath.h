#pragma once
#define _USE_MATH_DEFINES // For some (utterly stupid) reason, macros for numerical constants are not automatically defined... 
#include <math.h>

float toDegrees(float radians)
{
	return (float) radians * (180 / M_PI);
}

float toRadians(float degrees)
{
	return (float) degrees * (M_PI / 180);
}