#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include <G10/GXtypedef.h>
#include <G10/GXLinear.h>
#include <G10/GXTransform.h>

#define MAXFORCES     32

#define AIR_DENSITY   1.225
#define WATER_DENSITY 997

struct GXRigidbody_s
{

    float                 mass;                // The mass of the entity.
    float                 radius;              // Radius of the object

    GXvec3_t              acceleration;        // The last acceleration of the entity.
    GXvec3_t              velocity;            // The last velocity of the entity.
    
    GXvec3_t              angularAcceleration; // The last angular acceleration of the entity.
    GXvec3_t              angularVelocity;     // The last angular velocity of the entity.
    
    GXvec3_t             *forces;              // A list of forces acting on the entity.
    size_t                forcesCount;         // The number of forces acting on the entity.

    struct GXRigidbody_s *touching;            // List of objects the rigidbody is touching.
};
typedef struct GXRigidbody_s GXRigidbody_t;


// Constructors
GXRigidbody_t         *createRigidbody                    ( );
GXRigidbody_t         *loadRigidbody                      ( const char     path[] );
GXRigidbody_t         *loadRigidbodyAsJSON                ( char          *token);


// Derivatives 
extern void            calculateDerivativesOfDisplacement ( void          *entity );
extern void            calculateDerivativesOfRotation     ( void          *entity );

// Force computation
extern void            computeAllForces                   ( void          *entity );
extern void            computeFrictionalForce             ( void          *entity,    float mu, GXvec3_t *normalForce );
extern void            computeTensionForce                ( void          *entity );
extern void            computeNormalForce                 ( void          *entity,    float incline );
extern void            computeAirResitanceForce           ( void          *entity,    float density );
extern void            computeAppliedForce                ( void          *entity );

// Physics
static inline GXvec3_t applyForce                         ( GXRigidbody_t *rigidbody ) {
    return vec3xf(rigidbody->acceleration, rigidbody->mass);
}
int                    updatePositionAndVelocity          ( GXRigidbody_t *rigidbody, GXTransform_t *transform, u32 deltaTime ); // ✅ Updates the position and velocity vectors
int                    computeCollision                   ( GXRigidbody_t *a,         GXRigidbody_t *b );

// Destructors
int                    destroyRigidBody                   ( GXRigidbody_t *rigidbody ); 