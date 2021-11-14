#pragma once
#include <G10/GXtypedef.h>
#include <G10/GXTexture.h>
#include <G10/GXJSON.h>

struct GXMaterial_s {
    char                *name;

    GXTexture_t         *albedo; 
    GXTexture_t         *normal; 
    GXTexture_t         *rough; 
    GXTexture_t         *metal; 
    GXTexture_t         *AO; 
    GXTexture_t         *height;
    
    struct GXMaterial_s *next;
};

// Constructors
GXMaterial_t *createMaterial       ( void );                                            // ✅ Creates an empty material
GXMaterial_t *loadMaterial         ( const char    path[] );                            // ✅ Loads a material from a JSON file
GXMaterial_t *loadMaterialAsJSON   ( char         *token );                             // ✅ Loads a material from a JSON object

// Getters
GXMaterial_t *getMaterial          ( GXMaterial_t *materials, const char name[] );      // ✅ Finds a material in a list of materials

// Appemders
int           appendMaterial       ( GXMaterial_t *head     , GXMaterial_t *material ); // ✅ Appends a material to the end of a list of materials

// Destructors
int           unloadMaterial       ( GXMaterial_t *material );                          // ✅ Destroys a material