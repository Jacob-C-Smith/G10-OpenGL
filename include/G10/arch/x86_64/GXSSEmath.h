#pragma once

#include <G10/GXEntity.h>

// Vector accumulation
extern void SSESumVecs                         ( size_t      n, GXvec4_t* forces );

// Vector arithmatic
extern void  SSEAddVec                          ( GXvec4_t*   a, GXvec4_t* b, GXvec4_t* r );
extern void  SSEAddVecS                         ( GXvec4_t*   a, float     b, GXvec4_t* r );
extern void  SSESubVec                          ( GXvec4_t*   a, GXvec4_t* b, GXvec4_t* r );
extern void  SSESubVecS                         ( GXvec4_t*   a, float     b, GXvec4_t* r );
extern void  SSEMulVec                          ( GXvec4_t*   a, GXvec4_t* b, GXvec4_t* r );
extern void  SSEMulVecS                         ( GXvec4_t*   a, GXvec4_t* b, GXvec4_t* r );
extern void  SSEDivVec                          ( GXvec4_t*   a, GXvec4_t* b, GXvec4_t* r );
extern void  SSEDivVecS                         ( GXvec4_t*   a, GXvec4_t* b, GXvec4_t* r );

// Vector functions
extern void  SSECrossProduct                    ( GXvec3_t*   a, GXvec3_t* b, GXvec3_t* r );
extern float SSEDotProduct                      ( GXvec4_t*   a, GXvec4_t* b ); 
extern void  SSENormalize                       ( GXvec3_t*   a, GXvec3_t* r );

// Matrix funcitons
extern void  SSEmat4xvec4                       ( GXmat4_t*   m, GXvec4_t* v, GXvec4_t* r );
extern void  SSEmat4xmat4                       ( GXmat4_t*   m, GXmat4_t* n, GXmat4_t* r );
extern void  SSETransposeInverseMatrix          ( GXmat4_t*   m, GXvec4_t* r );