#include <G10/GXSkybox.h>


GXSkybox_t *createSkybox     ( )
{
    GXSkybox_t* ret = calloc(1, sizeof(GXSkybox_t));
    
    // Check the memory
    #ifndef NDEBUG
        if (ret == 0)
            goto noMem;
    #endif

    return ret;

    // Error handling
    {
        noMem:
        #ifndef NDEBUG
            gPrintError("[G10] [Skybox] Out of memory.\n");
        #endif
        return 0;
    }
}

GXSkybox_t *loadSkybox       ( const char  path[] )
{
        
    return 0;
}

GXSkybox_t* loadSkyboxAsJSON ( char *token )
{
    return 0;
}

int         drawSkybox       ( GXSkybox_t *skybox, GXCamera_t *camera )
{
	
}

int         destroySkybox    ( GXSkybox_t *skybox )
{
    return 0;
}
