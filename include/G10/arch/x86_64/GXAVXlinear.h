#pragma once

#include <G10/GXLinear.h>
#include <G10/GXQuaternion.h>

extern void AVXIdentityMat    ( mat4 *r );
extern void AVXTranslationMat ( mat4 *r, vec3         *loc );
extern void AVXRotationMat    ( mat4 *r, quaternion *quaternion );
extern void AVXScaleMat       ( mat4 *r, vec3         *sca );