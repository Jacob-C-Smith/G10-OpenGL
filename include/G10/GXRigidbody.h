#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include <G10/GXtypedef.h>
#include <G10/G10.h>
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
    bool                  active;               // Apply displacement and rotational forces?
     
    float                 mass;                 // The mass of the entity.
    float                 radius;               // Radius of the object

    vec3                  acceleration;         // The acceleration of the entity.
    vec3                  velocity;             // The velocity of the entity.
    
    quaternion            angular_acceleration; // The angular acceleration of the entity.
    quaternion            angular_velocity;     // The angular velocity of the entity.
    
    vec3                 *forces;               // A list of displacement forces acting on the entity.
    size_t                forces_count;         // The number of forces acting on the entity.

    vec3                 *torques;              // A list of angular forces acting on the entity.
    size_t                torque_count;         // The number of angular forces acting on the entity.

};

// Constructors
GXRigidbody_t         *create_rigidbody                      ( void );
GXRigidbody_t         *load_rigidbody                        ( const char     path[] );
GXRigidbody_t         *load_rigidbody_as_json                ( char          *token);


// Derivatives 
extern void            calculate_derivatives_of_displacement ( void          *entity );
extern void            calculate_derivatives_of_rotation     ( void          *entity );

// Force computation
extern void            compute_all_forces                    ( void          *entity );

// Contact forces
extern void            compute_frictional_force              ( void          *entity,    float mu, vec3 *normal_force );
extern void            compute_tension_force                 ( void          *entity );
extern void            compute_normal_force                  ( void          *entity,    float incline );
extern void            compute_applied_force                 ( void          *entity );

//
extern void            compute_air_resitance_force           ( void          *entity,    float density );
extern void            compute_gravitational_force           ( void          *entity,    float density );

// Physics
static inline vec3     apply_force                           ( GXRigidbody_t *rigidbody ) {
    return mul_vec3_f(rigidbody->acceleration, rigidbody->mass);
}
int                    update_position_and_velocity          ( GXRigidbody_t *rigidbody, GXTransform_t *transform, u32 delta_time ); // ✅ Updates the position and velocity vectors
int                    compute_collision                     ( GXRigidbody_t *a,         GXRigidbody_t *b );

// Destructors
int                    destroy_rigidbody                     ( GXRigidbody_t *rigidbody ); 