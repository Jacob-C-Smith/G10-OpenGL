#pragma once
#include <G10/GXtypedef.h>
#include <G10/GXTexture.h>
#include <G10/GXShader.h>

struct GXMaterial_s {
	GXTexture_t* albedo;
	GXTexture_t* normal;
	GXTexture_t* roughness;
	GXTexture_t* metallic;
	GXTexture_t* AO;
};
typedef struct GXMaterial_s GXMaterial_t;

GXMaterial_t* createMaterial( );                                            // ✅ Creates an empty material
int           assignMaterial( GXMaterial_t* material, GXshader_t* shader ); // ✅ Sets uniforms and binds textures for drawing calls
int           unloadMaterial( GXMaterial_t* material );                     // ✅ Destroys a material