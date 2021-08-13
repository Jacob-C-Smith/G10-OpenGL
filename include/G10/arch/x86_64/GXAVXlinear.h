#pragma once

#include <G10/GXLinear.h>
#include <G10/GXQuaternion.h>

extern void AVXIdentityMat    ( GXmat4_t *r );
extern void AVXTranslationMat ( GXmat4_t *r, GXvec3_t     *loc );
extern void AVXRotationMat    ( GXmat4_t *r, quaternion_t *quaternion );
extern void AVXScaleMat       ( GXmat4_t *r, GXvec3_t     *sca );