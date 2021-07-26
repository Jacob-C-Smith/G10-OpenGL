#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <G10/GXtypedef.h>

#include <G10/GXLinear.h>
#include <G10/GXJSON.h>

#include <G10/GXKeyValue.h>

#define GXSP_CameraPosition 0x0000000000000001
#define GXSP_CameraRotation 0x0000000000000002
#define GXSP_Model          0x0000000000000004
#define GXSP_View           0x0000000000000008
#define GXSP_Projection     0x0000000000000010
#define GXSP_LightPosition  0x0000000000000020
#define GXSP_LightColor     0x0000000000000040
#define GXSP_LightCount     0x0000000000000080
#define GXSP_Albedo         0x0000000000000100
#define GXSP_Normal         0x0000000000000200
#define GXSP_Metal          0x0000000000000400
#define GXSP_Rough          0x0000000000000800
#define GXSP_AO             0x0000000000001000

// Contains information about a shader
struct GXShader_s
{
    char         *name;
    unsigned int  shaderProgramID;
    size_t        requestedDataFlags;
    size_t        requestedDataCount;
    GXKeyValue_t *requestedData;
};
typedef struct GXShader_s GXShader_t;

// Constructors
GXShader_t *loadShader        ( const char  shaderPath[] );
GXShader_t *loadShaderAsJSON  ( char       *token );                                                                       // ✅ Loads a shader from a JSON file that points to the vertex and fragment shader source

// Shader processing
GXShader_t *loadCompileShader ( const char vertexShaderPath[], const char fragmentShaderPath[], const char shaderName[] ); // ✅ Loads a glsl vertex and glsl fragment shader from path and return a pointer to GXShader_t

// Shader manipulation
int         useShader         ( GXShader_t *shader );                                                                      // ✅ Uses a shader program
void        setShaderInt      ( GXShader_t *shader, const char name[], int       value );                                  // ✅ Replaces an int uniform
void        setShaderFloat    ( GXShader_t *shader, const char name[], float     value );                                  // ✅ Replaces a float uniform
void        setShaderVec3     ( GXShader_t *shader, const char name[], GXvec3_t  vector );                                 // ✅ Replaces a vec3 uniform
void        setShaderMat4     ( GXShader_t *shader, const char name[], GXmat4_t *m );                                      // ✅ Replaces a 4x4 matrix uniform

// Destructors
int         unloadShader      ( GXShader_t *shader );                                                                      // ✅ Unloads a shader and deletes all its contents
