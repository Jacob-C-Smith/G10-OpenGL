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

