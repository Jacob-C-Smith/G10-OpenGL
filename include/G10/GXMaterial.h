﻿#pragma once
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
GXMaterial_t *createMaterial       ( );                                            // ✅ Creates an empty material
GXMaterial_t *loadMaterial         ( const char    path[] );                       // ✅ Loads a material from a JSON file
GXMaterial_t *loadMaterialAsJSON   ( char         *token );                        // ✅ Loads a material from a JSON object

// Finders 
GXMaterial_t *getMaterial          ( GXMaterial_t *materials, const char name[] ); // 

// Destructors
int           unloadMaterial       ( GXMaterial_t *material );                     // ✅ Destroys a material