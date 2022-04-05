#include <G10/GXParticleSystem.h>

GXParticleSystem_t *create_particle_system  ( void )
{
	return calloc(1, sizeof(GXParticleSystem_t));
}

GXParticle_t       *create_particle         ( void )
{
	return calloc(1, sizeof(GXParticle_t));
}

GXParticleSystem_t* load_particle_system(const char* path)
{

	return 0;
}

GXParticleSystem_t *load_particle_system_as_json ( char               *token )
{
	return 0;
}

GXParticle_t* load_particle_as_json(char* token)
{
	return 0;
}


GXParticle_t* duplicate_particle(GXParticle_t* particle)
{
	GXParticle_t* ret = create_particle();

	ret->position = particle->position,
	ret->velocity = particle->velocity,
	ret->life     = particle->life;

	return ret;
}

void                destroy_particle_system ( GXParticleSystem_t *particle_system )
{
	free(particle_system);
}

void                destroy_particle        ( GXParticle_t       *particle )
{
	free(particle);
}

