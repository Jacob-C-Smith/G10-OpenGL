#include <G10/GXPhysics.h>

GXvec3_t* summateForces ( GXvec3_t* forces, size_t forceCount )
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

int integrateDisplacement(GXEntity_t* entity, float deltaTime)
{
    GXRigidbody_t* rigidbody  = entity->rigidbody;
    GXTransform_t* transform  = entity->transform;

    rigidbody->acceleration.x = (rigidbody->forces->x / rigidbody->mass) * deltaTime,
    rigidbody->acceleration.y = (rigidbody->forces->y / rigidbody->mass) * deltaTime,
    rigidbody->acceleration.z = (rigidbody->forces->z / rigidbody->mass) * deltaTime,
    rigidbody->acceleration.w = (rigidbody->forces->w / rigidbody->mass) * deltaTime;

    rigidbody->velocity.x     += (float) 0.5 * (rigidbody->acceleration.x * fabs(rigidbody->acceleration.x)),
    rigidbody->velocity.y     += (float) 0.5 * (rigidbody->acceleration.y * fabs(rigidbody->acceleration.y)),
    rigidbody->velocity.z     += (float) 0.5 * (rigidbody->acceleration.z * fabs(rigidbody->acceleration.z)),
    rigidbody->velocity.w     += (float) 0.5 * (rigidbody->acceleration.w * fabs(rigidbody->acceleration.w));

    transform->location.x     += (float) 0.5 * (rigidbody->velocity.x * fabs(rigidbody->velocity.x)),
    transform->location.y     += (float) 0.5 * (rigidbody->velocity.y * fabs(rigidbody->velocity.y)),
    transform->location.z     += (float) 0.5 * (rigidbody->velocity.z * fabs(rigidbody->velocity.z)),
    transform->location.w     += (float) 0.5 * (rigidbody->velocity.w * fabs(rigidbody->velocity.w));

    return 0;
}

int integrateRotation ( GXEntity_t* entity )
{
    GXRigidbody_t* rigidbody = entity->rigidbody;
    GXTransform_t* transform = entity->transform;

    transform->rotation.x += (float) 0.5 * (rigidbody->angularVelocity.x * fabs(rigidbody->angularVelocity.x)),
    transform->rotation.y += (float) 0.5 * (rigidbody->angularVelocity.y * fabs(rigidbody->angularVelocity.y)),
    transform->rotation.z += (float) 0.5 * (rigidbody->angularVelocity.z * fabs(rigidbody->angularVelocity.z)),
    transform->rotation.w += (float) 0.5 * (rigidbody->angularVelocity.w * fabs(rigidbody->angularVelocity.w));


    return 0;
}

bool collision(GXEntity_t* a, GXEntity_t* b)
{
    // Initialized data
    GXvec3_t aLocation = a->transform->location,
             bLocation = b->transform->location;

    GXCollider_t  *aCollider  = a->collider,
                  *bCollider  = b->collider;

    if (aCollider->type == box && bCollider->type == box)
    {
        GXvec3_t aMin = (GXvec3_t){ 0.f,0.f,0.f,0.f }, 
                 aMax = (GXvec3_t){ 0.f,0.f,0.f,0.f }, 
                 bMin = (GXvec3_t){ 0.f,0.f,0.f,0.f }, 
                 bMax = (GXvec3_t){ 0.f,0.f,0.f,0.f };

        aMin.x = aLocation.x - aCollider->collisionVector.x,
        aMin.y = aLocation.y - aCollider->collisionVector.y,
        aMin.z = aLocation.z - aCollider->collisionVector.z,
        aMin.w = aLocation.w - aCollider->collisionVector.w;
        
        aMax.x = aLocation.x + aCollider->collisionVector.x,
        aMax.y = aLocation.y + aCollider->collisionVector.y,
        aMax.z = aLocation.z + aCollider->collisionVector.z,
        aMax.w = aLocation.w + aCollider->collisionVector.w;

        bMin.x = bLocation.x - bCollider->collisionVector.x,
        bMin.y = bLocation.y - bCollider->collisionVector.y,
        bMin.z = bLocation.z - bCollider->collisionVector.z,
        bMin.w = bLocation.w - bCollider->collisionVector.w;
        
        bMax.x = bLocation.x - bCollider->collisionVector.x,
        bMax.y = bLocation.y - bCollider->collisionVector.y,
        bMax.z = bLocation.z - bCollider->collisionVector.z,
        bMax.w = bLocation.w - bCollider->collisionVector.w;
        
        if (aMin.x <= bMax.x && aMax.x >= bMin.x &&
            aMin.y <= bMax.y && aMax.y >= bMin.y &&
            aMin.z <= bMax.z && aMax.z >= bMin.z)
            return true;
        else
            return false;
    }

    return false;
}

int resolve(GXEntity_t* a, GXEntity_t* b)
{

    GXvec3_t  lastAVel = a->rigidbody->velocity,
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
    calculateDerivativesOfDisplacement(entity, 0);
    
    // TODO: Calculate angular acceleration, velocity, and rotation
    calculateDerivativesOfRotation(entity, 0);

    // TODO: Detect collisions
    // TODO: Implement BVH algorithm

	return 0;
}

int summateForces(GXEntity_t* entity)
{
    return 0;
}
*/