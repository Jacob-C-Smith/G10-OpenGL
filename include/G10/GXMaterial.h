#pragma once
#include <G10/GXtypedef.h>
#include <G10/GXTexture.h>
#include <G10/GXShader.h>
#include <G10/GXJSON.h>

struct GXMaterial_s {
	GXTexture_t* albedo;
	GXTexture_t* normal;
	GXTexture_t* roughness;
	GXTexture_t* metallic;
	GXTexture_t* AO;
};
typedef struct GXMaterial_s GXMaterial_t;

// Constructors
GXMaterial_t* createMaterial ( );                                            // ✅ Creates an empty material
GXMaterial_t* loadMaterial   ( const char path[] );                          // ✅ Loads a material from a JSON file

// Material
int           assignMaterial ( GXMaterial_t* material, GXShader_t* shader ); // ✅ Sets uniforms and binds textures for drawing calls

// Destructors
int           unloadMaterial ( GXMaterial_t* material );                     // ✅ Destroys a material