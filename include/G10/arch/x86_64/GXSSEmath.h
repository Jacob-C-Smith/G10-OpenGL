#pragma once

#include <G10/GXEntity.h>

extern void sumVecs                            ( size_t      n,      GXvec4_t* forces );
extern void calculateDerivativesOfDisplacement ( void*       entity, float     deltaTime );

extern void SSEAddVec3                         ( GXvec4_t*   a,      GXvec4_t* b,       GXvec4_t* r );
extern void SSEAddVec3S                        ( GXvec4_t*   a,      float     b,       GXvec4_t* r );
extern void SSESubVec3                         ( GXvec4_t*   a,      GXvec4_t* b,       GXvec4_t* r );
extern void SSESubVec3S                        ( GXvec4_t*   a,      float     b,       GXvec4_t* r );
extern void SSEXProduct                        ( GXvec3_t*   a,      GXvec3_t* b,       GXvec3_t* r );
extern void SSENormalize                       ( GXvec3_t*   a,      GXvec3_t* r );
extern void SSEmat4xvec4                       ( GXmat4_t*   m,      GXvec4_t* v,       GXvec4_t* r );
extern void SSEMat4xMat4                       ( GXmat4_t*   m,      GXmat4_t* n,       GXmat4_t* r );
extern void SSETransposeInverseMatrix          ( GXmat4_t*   m,      GXmat4_t* r );
extern void SSETranslationScaleMat             ( GXvec3_t*   loc,    GXvec3_t* scale, GXmat4_t* r);
extern void SSErotationMatrixFromVec           ( GXvec3_t*   rot,    GXmat4_t* r);
extern void SSEgenerateModelMatrixFromVec      ( GXvec3_t*   loc,    GXvec3_t* rot,   GXvec3_t* scale, GXmat4_t* r );

