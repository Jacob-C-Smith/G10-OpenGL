#pragma once
#include <G10/GXtypedef.h>
#include <G10/GXTexture.h>

#include <JSON/JSON.h>

struct GXMaterial_s {
    char                *name;

    GXTexture_t         *albedo; 
    GXTexture_t         *normal; 
    GXTexture_t         *rough; 
    GXTexture_t         *metal; 
    GXTexture_t         *ao; 
    GXTexture_t         *height;
    
    struct GXMaterial_s *next;
};

// Constructors
GXMaterial_t *create_material       ( void );                                            // ✅ Creates an empty material
GXMaterial_t *load_material         ( const char    path[] );                            // ✅ Loads a material from a JSON file
GXMaterial_t *load_material_as_json ( char         *token );                             // ✅ Loads a material from a JSON object

// Getters
GXMaterial_t *get_material          ( GXMaterial_t *materials, const char name[] );      // ✅ Finds a material in a list of materials

// Appemders
int           append_material       ( GXMaterial_t *head     , GXMaterial_t *material ); // ✅ Appends a material to the end of a list of materials

// Destructors
int           unload_material       ( GXMaterial_t *material );                          // ✅ Destroys a material