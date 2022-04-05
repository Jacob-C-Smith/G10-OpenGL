// Preprocessor directives //
#pragma once

// Includes //

// Standard library includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// G10 Includes
#include <G10/GXtypedef.h>
#include <G10/G10.h>

// Structure definitions //

struct GXParticleSystem_s
{
    u32                count;

    GXShader_t        *shader;
    GXTexture_t       *texture;
    GXPart_t          *quad;

    struct Particle_s *particles;
};

struct GXParticle_s
{
    vec3                 position,
                         velocity;
    float                life;
    struct GXParticle_s* next;
};

// Functions //

// Allocators
GXParticleSystem_t *create_particle_system       ( void );
GXParticle_t       *create_particle              ( void );

// Constructors
GXParticleSystem_t *load_particle_system         ( const char         *path );
GXParticleSystem_t *load_particle_system_as_json ( char               *token );
GXParticle_t       *load_particle_as_json        ( char               *token );
GXParticle_t       *duplicate_particle           ( GXParticle_t       *particle );

// Physics


// Drawers


// Destructors
void                destroy_particle_system      ( GXParticleSystem_t *particle_system );
void                destroy_particle             ( GXParticle_t       *particle );