#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <G10/GXDebug.h>
#include <G10/GXtypedef.h>

#include <G10/GXLinear.h>

// Contains information about a shader
struct GXShader_s
{
	unsigned int shaderProgramID;
};
typedef struct GXShader_s GXshader_t;

GXshader_t* loadShader     ( const char vertexShaderPath[], const char fragmentShaderPath[] ); // ✅ Loads a glsl vertex and glsl fragment shader from path and return a pointer to GXshader_t
int         useShader      ( GXshader_t* shader );                                             // ✅ Uses a shader program
void        setShaderInt   ( GXshader_t* shader, const char name[], int value );               // ✅ Replaces an int uniform
void        setShaderFloat ( GXshader_t* shader, const char name[], float value );             // ✅ Replaces a float uniform
void        setShaderVec3  ( GXshader_t* shader, const char name[], GXvec3_t vector );         // ✅ Replaces a vec3 uniform
void        setShaderMat4  ( GXshader_t* shader, const char name[], GXmat4_t* m );             // ✅ Replaces a 4x4 matrix uniform
int         unloadShader   ( GXshader_t* shader );                                             // ✅ Unloads a shader and deletes all its contents
