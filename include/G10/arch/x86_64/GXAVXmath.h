#pragma once
#pragma once

#include <stdbool.h>

#include <G10/GXEntity.h>
#include <G10/GXQuaternion.h>

// Checks for AVX2
extern int   checkForAVX2              ( void ); 

// Intervector operations
extern float AVXHorizontalAdd          ( GXvec4_t*   a );

// Vector accumulation
extern void  AVXSumVecs                ( size_t      n, GXvec4_t* vecs );

// Vector arithmatic
extern void  AVXAddVec                 ( GXvec4_t*   a, GXvec4_t* b, GXvec4_t* r );
extern void  AVXAddVecS                ( GXvec4_t*   a, float     b, GXvec4_t* r );
extern void  AVXSubVec                 ( GXvec4_t*   a, GXvec4_t* b, GXvec4_t* r );
extern void  AVXSubVecS                ( GXvec4_t*   a, float     b, GXvec4_t* r );
extern void  AVXMulVec                 ( GXvec4_t*   a, GXvec4_t* b, GXvec4_t* r );
extern void  AVXMulVecS                ( GXvec4_t*   a, GXvec4_t* b, GXvec4_t* r );
extern void  AVXDivVec                 ( GXvec4_t*   a, GXvec4_t* b, GXvec4_t* r );
extern void  AVXDivVecS                ( GXvec4_t*   a, GXvec4_t* b, GXvec4_t* r );

// Compare
extern bool  AVXSameVec                ( GXvec4_t*   a, GXvec4_t* b );

// Vector functions
extern void  AVXCrossProduct           ( GXvec3_t*   a, GXvec3_t* b, GXvec3_t* r );
extern float AVXDot                    ( GXvec4_t*   a, GXvec4_t* b );
extern void  AVXNormalize              ( GXvec3_t*   a, GXvec3_t* r );

// Matrix funcitons
extern void  AVXmat4xvec4              ( GXmat4_t*   m, GXvec4_t* v, GXvec4_t* r );
extern void  AVXmat4xmat4              ( GXmat4_t*   m, GXmat4_t* n, GXmat4_t* r );
extern void  AVXTransposeInverseMatrix ( GXmat4_t*   m, GXvec4_t* r );

// Model, View, and Projection 
extern void  AVXModelMatrix            ( GXvec4_t* location, GXvec4_t* rotation,    GXvec4_t* scale, GXmat4_t* ret );
extern void  AVXViewMatrix             ( GXvec4_t* eye,      GXvec4_t* target,      GXvec4_t* up );
extern void  AVXProjectionMatrix       ( float     fov,      float     aspectRatio, float     near,  float far );

// Quaternion operations
extern void AVXQMul                    ( quaternion_t* a, quaternion_t* b, quaternion_t* r );