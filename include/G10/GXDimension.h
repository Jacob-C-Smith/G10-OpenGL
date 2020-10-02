#pragma once
#include <G10/GXDebug.h>

// 2D point
struct point2_s {
	float u;
	float v;
};
typedef struct point2_s point2_t;

// 3D point
struct point3_s {
	float x;
	float y;
	float z;
};
typedef struct point3_s point3_t;

// 4D point
struct point4_s {
	float s;
	float i;
	float j;
	float k;
};
typedef struct point4_s point4_t;

// 3D int point
struct int3_s {
	unsigned long x;
	unsigned long y;
	unsigned long z;
};
typedef struct int3_s int3_t;