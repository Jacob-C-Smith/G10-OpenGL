#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <G10/GXtypedef.h>
#include <G10/G10.h>
#include <JSON/JSON.h>

#include <G10/GXLinear.h>

#include <G10/GXTexture.h>
#include <G10/GXLight.h>
#include <G10/GXCamera.h>

// Describes a shader uniform
struct GXUniform_s
{
    char               *key, 
                       *value;
    u32                 type;
    struct GXUniform_s *next;
};

// Contains information about a shader
struct GXShader_s
{
    char              *name;
    unsigned int       shader_program_id;
    size_t             requested_data_count;
    GXUniform_t       *requested_data;
};

#define GXUNIFBOOL    0x6c6f6f62 // "loob"
#define GXUNIFUINT    0x746e6975 // "tniu"
#define GXUNIFINT     0x00746e69 // 0"tni"
#define GXUNIFFLOAT   0x616f6c66 // "aolf"
#define GXUNIFVEC3    0x33636576 // "3cev"
#define GXUNIFVEC4    0x34636576 // "4cev"
#define GXUNIFMAT4    0x3474616d // "4tam"
#define GXUNIFTEXTURE 0x74786574 // "txet"
#define GXUNIFSTRUCT  0x75727473 // "urts"


// Constructors
GXShader_t        *create_shader        ( void );                                                                                 // ✅ Creates an empty shader.
GXShader_t        *load_shader          ( const char  path[] );                                                                   // ✅ Load a shader from a JSON file.
GXShader_t        *load_shader_as_json  ( char       *token );                                                                    // ✅ Load a shader from a JSON token.

// Shader parsing
GXUniform_t       *load_uniform_as_json ( char       *token );                                                                    // ✅ Load a uniform as a JSON token.

// Shader processing
GXShader_t        *load_compile_shader  ( const char *vertexPath, const char  *fragmentPath    , const char *shaderName );        // ✅ Loads a glsl vertex and glsl fragment shader from path and return a pointer to GXShader_t
int                compile_from_text    ( GXShader_t *shader    , char        *vertexShaderText, char       *fragmentShaderText); // ✅ Compiles a shader from shader text

// Shader manipulation
int                use_shader           ( GXShader_t *shader );                                                                   // ✅ Uses a shader program

void               set_shader_int       ( GXShader_t *shader, const char       *name,   int                  value );             // ✅ Replaces an int uniform
void               set_shader_float     ( GXShader_t *shader, const char       *name,   float                value );             // ✅ Replaces a float uniform
void               set_shader_vec3      ( GXShader_t *shader, const char       *name,   vec3                 vector );            // ✅ Replaces a vec3 uniform
void               set_shader_mat4      ( GXShader_t *shader, const char       *name,   mat4                *matrix );            // ✅ Replaces a 4x4 matrix uniform

void               set_shader_texture   ( GXShader_t *shader, const char       *name,      GXTexture_t         *texture );           // ✅ Replaces a 4x4 matrix uniform
void               set_shader_transform ( GXShader_t *shader, GXTransform_t    *transform );
void               set_shader_camera    ( GXShader_t *shader, GXCamera_t       *camera    );                                         // ✅ Replaces a 4x4 matrix uniform
void               set_shader_lights    ( GXShader_t *shader, GXLight_t        *lights,    size_t               numLights );         // ✅ Replaces a 4x4 matrix uniform
void               set_shader_material  ( GXShader_t *shader, GXMaterial_t     *material );
void               set_shader_ibl       ( GXShader_t *shader, GXSkybox_t       *skybox   );
void               set_shader_bone      ( GXShader_t *shader, GXBone_t         *bone     );
void               set_shader_rig       ( GXShader_t *shader, GXRig_t          *rig      );

// Destructors
int                unload_shader        ( GXShader_t *shader );                                                                   // ✅ Unloads a shader and deletes all its contents
