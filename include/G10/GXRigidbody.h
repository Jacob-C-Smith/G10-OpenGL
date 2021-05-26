#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include <G10/GXtypedef.h>
#include <G10/GXLinear.h>
#include <G10/GXTransform.h>

#define MAXFORCES 32

struct GXRigidbody_s
{

	float     mass;                // The mass of the entity.
	float     radius;              // Radius of the object
	bool      useGravity;          // Does the entity use gravity.

	GXvec3_t  acceleration;        // The last acceleration of the entity.
	GXvec3_t  velocity;            // The last velocity of the entity.
	
	GXvec3_t  angularAcceleration; // The last angular acceleration of the entity.
	GXvec3_t  angularVelocity;     // The last angular velocity of the entity.
	
	GXvec3_t* forces;              // A list of forces acting on the entity.
	size_t    forcesCount;         // The number of forces acting on the entity.
};
typedef struct GXRigidbody_s GXRigidbody_t;


// Constructors
GXRigidbody_t*         createRigidbody                    ( float mass, float radius, bool useGravity );

extern void            calculateDerivativesOfDisplacement ( void* entity );

// Physics
static inline GXvec3_t applyForce                         ( GXRigidbody_t* rigidbody ) {
	return vec3xf(rigidbody->acceleration, rigidbody->mass);
}
int                    updatePositionAndVelocity          ( GXRigidbody_t* rigidbody, GXTransform_t* transform, u32 deltaTime );
int                    computeCollision                   ( GXRigidbody_t* a, GXRigidbody_t* b );

// Destructors
int                    destroyRigidBody                   ( GXRigidbody_t* rigidbody );