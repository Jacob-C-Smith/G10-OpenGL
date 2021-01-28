#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include <G10/GXtypedef.h>
#include <G10/GXDebug.h>
#include <G10/GXLinear.h>
#include <G10/GXTransform.h>

struct GXRigidbody_s
{
	GXvec3_t velocity;
	GXvec3_t acceleration;
	float    mass;
	GXvec3_t force;
	bool     useGravity;
};
typedef struct GXRigidbody_s GXRigidbody_t;


// Constructors
GXRigidbody_t*         createRigidBody           ( float mass, bool useGravity );

// Physics
static inline GXvec3_t applyForce         ( GXRigidbody_t* rigidbody ) {
	return vec3xf(rigidbody->acceleration, rigidbody->mass);
}
int                    updatePositionAndVelocity ( GXRigidbody_t* rigidbody, GXTransform_t* transform, u32 deltaTime );
int                    computeCollision          ( GXRigidbody_t* a, GXRigidbody_t* b );

// Destructors
int                    destroyRigidBody          ( GXRigidbody_t* rigidbody );