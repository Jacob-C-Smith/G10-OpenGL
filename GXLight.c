#include <G10/GXLight.h>

GXLight_t* createLight( )
{
	GXLight_t* ret = malloc(sizeof(GXLight_t));
	if (ret == (void*)0)
		return ret;

	ret->name = (void*)0;

	ret->color.x    = 0.f;
	ret->color.y    = 0.f;
	ret->color.z    = 0.f;

	ret->location.x = 0.f;
	ret->location.y = 0.f;
	ret->location.z = 0.f;

	ret->location.w = 0.f;

	ret->next       = 0;

	return ret;
}

int destroyLight(GXLight_t* light)
{
	free((void*)light->name);

	light->color.x    = 0.f,
	light->color.y    = 0.f,
	light->color.z    = 0.f;

	light->location.x = 0.f,
	light->location.y = 0.f,
	light->location.z = 0.f;

	light->next = 0;

	free(light);

	return 0;
}