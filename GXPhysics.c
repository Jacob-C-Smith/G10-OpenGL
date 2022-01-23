#include <G10/GXPhysics.h>

vec3* summate_forces       ( vec3       *forces, size_t forceCount )
{
    
    forces[0].x = 0.f,
    forces[0].y = 0.f,
    forces[0].z = 0.f,
    forces[0].w = 0.f;

    for (size_t i = 1; i < forceCount+1; i++)
    {
        forces[0].x += forces[i].x,
        forces[0].y += forces[i].y,
        forces[0].z += forces[i].z,
        forces[0].w += forces[i].w;
    }
    AVXSumVecs(forceCount, forces + 16);
    return forces;
}

bool collision            ( GXEntity_t *a, GXEntity_t *b )
{
    return false;
}

int resolve               ( GXEntity_t *a, GXEntity_t *b )
{

    vec3  lastAVel = a->rigidbody->velocity,
              lastBVel = b->rigidbody->velocity,
             *aVel     = &a->rigidbody->velocity,
             *bVel     = &b->rigidbody->velocity;

    float     aMass   = a->rigidbody->mass,
              bMass   = b->rigidbody->mass;

    float     c       = (aMass - bMass) / (aMass + bMass),
              d       = (bMass - aMass) / (aMass + bMass),
              e       = (2 * bMass) / (aMass + bMass),
              f       = (2 * aMass) / (aMass + bMass);
    
    aVel->x           = (lastAVel.x * c) + (lastBVel.x * e);
    aVel->y           = (lastAVel.y * c) + (lastBVel.y * e);
    aVel->z           = (lastAVel.z * c) + (lastBVel.z * e);
    aVel->w           = (lastAVel.w * c) + (lastBVel.w * e);

    bVel->x           = (lastAVel.x * f) + (lastBVel.x * d);
    bVel->y           = (lastAVel.y * f) + (lastBVel.y * d);
    bVel->z           = (lastAVel.z * f) + (lastBVel.z * d);
    bVel->w           = (lastAVel.w * f) + (lastBVel.w * d);

    return 0;
}

/* 
int processPhysics ( GXEntity_t* entity )
{
    // Summate forces 
    AVXSumVecs(entity->rigidbody->forcesCount, entity->rigidbody->forces);

    // Calculate new acceleration, velocity, and position
    calculate_derivatives_of_displacement(entity, 0);
    
    // TODO: Calculate angular acceleration, velocity, and rotation
    calculate_derivatives_of_rotation(entity, 0);

    // TODO: Detect collisions
    // TODO: Implement BVH algorithm

	return 0;
}

int summate_forces(GXEntity_t* entity)
{
    return 0;
}
*/