#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <G10/GXDebug.h>
#include <G10/GXLinear.h>

struct GXcamera_s {
	
	// View
	GXvec3_t where;
	GXvec3_t target;
	GXvec3_t up;

	// Projection
	float fov;
	float near;
	float far;
	float aspectRatio;

	// Matricies
	GXmat4_t view;
	GXmat4_t projection;
};
typedef struct GXcamera_s GXcamera_t;

GXcamera_t*     createCamera            ( GXvec3_t where, GXvec3_t target, GXvec3_t up, float fov, float near, float far, float aspectRatio ); // Creates a camera object to render a scene
GXmat4_t        perspective             ( float fov, float aspect, float near, float far );                                                    // Computes perspective projection matrix from FOV, aspect ratio, near and far clipping planes.
GXcamera_t*     computeProjectionMatrix ( GXcamera_t* camera );                                                                                // Computes an updated projection matrix
inline GXmat4_t lookAt                  ( GXvec3_t eye, GXvec3_t target, GXvec3_t up )                                                         // Computes a view matrix from eye, target, and up vectors
{ 
	// Compute forward direction
	GXvec3_t f = normalize((GXvec3_t) {
		eye.x - target.x,
		eye.y - target.y,
		eye.z - target.z
	});

	// Compute left direction as cross product of up and forward
	GXvec3_t l = normalize(crossProductVec3(up, f));
	// Recompute up
	GXvec3_t u = crossProductVec3(f, l);

	// Return the view matrix
	return (GXmat4_t) {
		l.x, u.x, f.x, 0,
		l.y, u.y, f.y, 0,
		l.z, u.z, f.z, 0,
		(-l.x * eye.x - l.y * eye.y - l.z * eye.z), (-u.x * eye.x - u.y * eye.y - u.z * eye.z), (-f.x * eye.x - f.y * eye.y - f.z * eye.z), 1
	};
};
inline void     computeViewMatrix       ( GXcamera_t* camera )                                                                                 // Computes an updated view matrix
{
	camera->view = lookAt(camera->where, camera->target, camera->up);
};