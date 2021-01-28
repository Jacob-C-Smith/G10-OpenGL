#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glad/glad.h>

#include <G10/GXDebug.h>
#include <G10/GXtypedef.h>

#include <G10/GXLinear.h>
#include <G10/GXJSON.h>

// Contains information about a shader
struct GXShader_s
{
	char*        name;
	unsigned int shaderProgramID;
};
typedef struct GXShader_s GXShader_t;

// Constructors
GXShader_t* loadShader       ( const char vertexShaderPath[], const char fragmentShaderPath[], const char shaderName[] ); // ✅ Loads a glsl vertex and glsl fragment shader from path and return a pointer to GXShader_t
GXShader_t* loadShaderAsJSON ( const char shaderPath[] );                                                                 // ✅ Loads a shader from a JSON file that points to the vertex and fragment shader source

// Shader manipulation
int         useShader        ( GXShader_t* shader );                                                                      // ✅ Uses a shader program
void        setShaderInt     ( GXShader_t* shader, const char name[], int value );                                        // ✅ Replaces an int uniform
void        setShaderFloat   ( GXShader_t* shader, const char name[], float value );                                      // ✅ Replaces a float uniform
void        setShaderVec3    ( GXShader_t* shader, const char name[], GXvec3_t vector );                                  // ✅ Replaces a vec3 uniform
void        setShaderMat4    ( GXShader_t* shader, const char name[], GXmat4_t* m );                                      // ✅ Replaces a 4x4 matrix uniform

// Destructors
int         unloadShader     ( GXShader_t* shader );                                                                      // ✅ Unloads a shader and deletes all its contents
