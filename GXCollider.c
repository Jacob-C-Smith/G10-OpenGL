#include <G10/GXCollider.h>

GXCollider_t *createCollider     ( ) 
{
	GXCollider_t* ret = calloc(1, sizeof(GXCollider_t));

	#ifndef NDEBUG
		if (ret == (void*)0)
			goto outOfMem;
	#endif

	return ret;

	// Error handling
	{
		outOfMem:
		#ifndef NDEBUG
			gPrintError("[G10] [Collider] Out of memory!\n");
		#endif
		return 0;
	}
}

GXCollider_t *loadCollider       ( const char   *path ) 
{
	// TODO: Write
	return (GXCollider_t*)0;
}

GXCollider_t *loadColliderAsJSON ( char         *token )
{
	// TODO: Write
	return (GXCollider_t*)0;
}

int          destroyCollider     ( GXCollider_t *collider )
{
	// TODO: Write
	return 0;
}
