#pragma once
#pragma once

#include <stdbool.h>

#include <G10/GXEntity.h>
#include <G10/GXQuaternion.h>

// Checks for AVX2
extern int   checkForAVX2              ( void ); 

// Intervector operations
extern float AVXHorizontalAdd          ( vec4*   a );

// Vector accumulation
extern void  AVXSumVecs                ( size_t      n, vec4* vecs );

// Vector arithmatic
extern void  AVXAddVec                 ( vec4*   a, vec4* b, vec4* r );
extern void  AVXAddVecS                ( vec4*   a, float b, vec4* r );
extern void  AVXSubVec                 ( vec4*   a, vec4* b, vec4* r );
extern void  AVXSubVecS                ( vec4*   a, float b, vec4* r );
extern void  AVXMulVec                 ( vec4*   a, vec4* b, vec4* r );
extern void  AVXMulVecS                ( vec4*   a, vec4* b, vec4* r );
extern void  AVXDivVec                 ( vec4*   a, vec4* b, vec4* r );
extern void  AVXDivVecS                ( vec4*   a, vec4* b, vec4* r );

// Compare
extern bool  AVXSameVec                ( vec4*   a, vec4* b );

// Vector functions
extern void  AVXCrossProduct           ( vec3*   a, vec3* b, vec3 *r );
extern float AVXDot                    ( vec4*   a, vec4* b );
extern void  AVXNormalize              ( vec3*   a, vec3* r );

// Matrix funcitons
extern void  AVXmat4xvec4              ( mat4*   m, vec4* v, vec4* r );
extern void  AVXmat4xmat4              ( mat4*   m, mat4* n, mat4* r );
extern void  AVXTransposeInverseMatrix ( mat4*   m, vec4* r );

// Model, View, and Projection 
extern void  AVXModelMatrix            ( vec4* location, vec4* rotation,    vec4* scale, mat4* ret );
extern void  AVXViewMatrix             ( vec4* eye,      vec4* target,      vec4* up );
extern void  AVXProjectionMatrix       ( float     fov,  float     aspectRatio, float     near,  float far );

// Quaternion operations
extern void  AVXQMul                   ( quaternion* a, quaternion* b, quaternion* r );
extern void  AVXQConjugate             ( quaternion* a, quaternion* r );