#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include <G10/GXtypedef.h>
#include <G10/GXLinear.h>
#include <G10/GXTransform.h>

#define MAXFORCES     32

#define AIR_DENSITY   1.225
#define WATER_DENSITY 997

enum rigidbodyType_e {
    active  = 1,
    passive = 2 
};
typedef enum rigidbodyType_e rigidbodyType_t;

struct GXRigidbody_s
{
    rigidbodyType_t       type;                // Active or passive

    float                 mass;                // The mass of the entity.
    float                 radius;              // Radius of the object

    vec3                  acceleration;        // The last acceleration of the entity.
    vec3                  velocity;            // The last velocity of the entity.
    
    vec3                  angularAcceleration; // The last angular acceleration of the entity.
    vec3                  angularVelocity;     // The last angular velocity of the entity.
    
    vec3                 *forces;              // A list of displacement forces acting on the entity.
    size_t                forcesCount;         // The number of forces acting on the entity.

    vec3                 *angularForces;       // A list of angular forces acting on the entity.
    size_t                angularForcesCount;  // The number of angular forces acting on the entity.

};

// Constructors
GXRigidbody_t         *createRigidbody                    ( void );
GXRigidbody_t         *loadRigidbody                      ( const char     path[] );
GXRigidbody_t         *loadRigidbodyAsJSON                ( char          *token);


// Derivatives 
extern void            calculateDerivativesOfDisplacement ( void          *entity );
extern void            calculateDerivativesOfRotation     ( void          *entity );

// Force computation
extern void            computeAllForces                   ( void          *entity );
extern void            computeFrictionalForce             ( void          *entity,    float mu, vec3 *normalForce );
extern void            computeTensionForce                ( void          *entity );
extern void            computeNormalForce                 ( void          *entity,    float incline );
extern void            computeAirResitanceForce           ( void          *entity,    float density );
extern void            computeAppliedForce                ( void          *entity );

// Physics
static inline vec3 applyForce                             ( GXRigidbody_t *rigidbody ) {
    return vec3xf(rigidbody->acceleration, rigidbody->mass);
}
int                    updatePositionAndVelocity          ( GXRigidbody_t *rigidbody, GXTransform_t *transform, u32 deltaTime ); // ✅ Updates the position and velocity vectors
int                    computeCollision                   ( GXRigidbody_t *a,         GXRigidbody_t *b );

// Destructors
int                    destroyRigidBody                   ( GXRigidbody_t *rigidbody ); 