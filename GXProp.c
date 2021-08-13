#include <G10/GXProp.h>

GXProp_t* createProp()
{
	GXProp_t* ret = calloc(sizeof(GXProp_t), 1);

	ret->name     = (void*)0;
	ret->part     = createPart();
	ret->texture  = (void*)0;
	
	return ret;
}

GXProp_t* loadProp(const char path[])
{
	GXProp_t* ret = createProp();

	return ret;
}

GXProp_t* loadPropAsJSON(char* token)
{
	GXProp_t* ret = createProp();

	return ret;
}

int drawProp(GXProp_t* prop)
{
	loadTextureToTextureUnit(prop->texture);
	return 0;
}

int destroyProp(GXProp_t* prop)
{
	return 0;
}
