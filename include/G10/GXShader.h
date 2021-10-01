#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <G10/GXtypedef.h>
#include <G10/G10.h>
#include <G10/GXJSON.h>

#include <G10/GXLinear.h>

#include <G10/GXTexture.h>
#include <G10/GXLight.h>
#include <G10/GXCamera.h>
#include <G10/GXKeyValue.h>

// Describes a shader uniform
struct GXUniform_s
{
    GXKeyValue_t             *keyValue;
    u32                       type;
    struct GXUniform_s       *next;
};

// Contains information about a shader
struct GXShader_s
{
    char              *name;
    unsigned int       shaderProgramID;
    size_t             requestedDataCount;
    GXUniform_t       *requestedData;
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
GXShader_t        *createShader       ( );                                                                                      // ✅ Creates an empty shader.
GXShader_t        *loadShader         ( const char  path[] );                                                                   // ✅ Load a shader from a JSON file.
GXShader_t        *loadShaderAsJSON   ( char       *token );                                                                    // ✅ Load a shader from a JSON token.

// Shader parsing
GXUniform_t       *loadUniformAsJSON  ( char       *token );                                                                    // ✅ Load a uniform as a JSON token.

// Shader processing
GXShader_t        *loadCompileShader  ( const char *vertexPath, const char  *fragmentPath    , const char *shaderName );        // ✅ Loads a glsl vertex and glsl fragment shader from path and return a pointer to GXShader_t
int                compileFromText    ( GXShader_t *shader    , char        *vertexShaderText, char       *fragmentShaderText); // ✅ Compiles a shader from shader text

// Shader manipulation
int                useShader          ( GXShader_t *shader );                                                                   // ✅ Uses a shader program

void               setShaderInt       ( GXShader_t *shader, const char       *name,   int                  value );             // ✅ Replaces an int uniform
void               setShaderFloat     ( GXShader_t *shader, const char       *name,   float                value );             // ✅ Replaces a float uniform
void               setShaderVec3      ( GXShader_t *shader, const char       *name,   vec3                 vector );            // ✅ Replaces a vec3 uniform
void               setShaderMat4      ( GXShader_t *shader, const char       *name,   mat4                *matrix );            // ✅ Replaces a 4x4 matrix uniform

void               setShaderTexture   ( GXShader_t *shader, const char       *name,   GXTexture_t         *texture );           // ✅ Replaces a 4x4 matrix uniform
void               setShaderTransform ( GXShader_t *shader, GXTransform_t    *transform );
void               setShaderCamera    ( GXShader_t *shader, GXCamera_t       *camera );                                         // ✅ Replaces a 4x4 matrix uniform
void               setCameraLights    ( GXShader_t *shader, GXLight_t        *light );                                          // ✅ Replaces a 4x4 matrix uniform

// Destructors
int                unloadShader       ( GXShader_t *shader );                                                                   // ✅ Unloads a shader and deletes all its contents
