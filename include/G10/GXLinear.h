#pragma once

#include <math.h>

// TODO: Phase out all of these functions in favor of SIMD accelerated funcitons.

#include <G10/GXtypedef.h>

// ✅ Adds vector a to vector b. Returns new vector
static inline void addVec3 ( vec3 *r, vec3 a, vec3 b )
{
    r->x = a.x + b.x,
    r->y = a.y + b.y,
    r->z = a.z + b.z;
    
}

// ✅ Subtracts vector a from vector b. Returns new vector
static inline vec3 subVec3 ( vec3 a, vec3 b )
{
    return (vec3) {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

// ✅ Computes the dot product of 2 3D vectors
static inline float length ( vec3 a )
{
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

static inline float dotProductVec3(vec3 a, vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// ✅ Computes the cross product of 2 3D vectors
static inline vec3 crossProductVec3 ( vec3 a, vec3 b )
{
    return (vec3) {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// ✅ Multiplies the coordinates of one vector by the coordinates of another
static inline vec3 vec3xvec3 ( vec3 a, vec3 b )
{
    return (vec3) {
        a.x * b.x,
        a.y * b.y,
        a.z * b.z
    };
}

// ✅ Multiplies a vector by a scalar value
static inline vec3 vec3xf ( vec3 v, float s )
{
    return (vec3) {
        v.x * s,
        v.y * s,
        v.z * s,
        v.w * s
    };
}

// ✅ Normailizes a vector
static inline vec3 normalize ( vec3 v )
{
    float vl = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
    return (vec3) { v.x / vl, v.y / vl, v.z / vl, v.w / vl };
}

// ✅ Multiplies a matrix by a vector
static inline vec4 mat4xvec4 ( mat4 m, vec4 v )
{
    return (vec4) {
        m.a* v.x + m.b * v.y + m.c * v.z + m.d * v.w,
        m.e* v.x + m.f * v.y + m.g * v.z + m.h * v.w,
        m.i* v.x + m.j * v.y + m.k * v.z + m.l * v.w,
        m.m* v.x + m.n * v.y + m.o * v.z + m.p * v.w
    };
}

// ✅ Multiplies a matrix by a matrix
static inline mat4 mat4xmat4 ( mat4 m, mat4 n )
{
    return (mat4) {
        (m.a * n.a + m.b * n.e + m.c * n.i + m.d * n.m), (m.a * n.b + m.b * n.f + m.c * n.j + m.d * n.n), (m.a * n.c + m.b * n.g + m.c * n.k + m.d * n.o), (m.a * n.d + m.b * n.h + m.c * n.l + m.d * n.p),
        (m.e * n.a + m.f * n.e + m.g * n.i + m.h * n.m), (m.e * n.b + m.f * n.f + m.g * n.j + m.h * n.n), (m.e * n.c + m.f * n.g + m.g * n.k + m.h * n.o), (m.e * n.d + m.f * n.h + m.g * n.l + m.h * n.p),
        (m.i * n.a + m.j * n.e + m.k * n.i + m.l * n.m), (m.i * n.b + m.j * n.f + m.k * n.j + m.l * n.n), (m.i * n.c + m.j * n.g + m.k * n.k + m.l * n.o), (m.i * n.d + m.j * n.h + m.k * n.l + m.l * n.p),
        (m.m * n.a + m.n * n.e + m.o * n.i + m.p * n.m), (m.m * n.b + m.n * n.f + m.o * n.j + m.p * n.n), (m.m * n.c + m.n * n.g + m.o * n.k + m.p * n.o), (m.m * n.d + m.n * n.h + m.o * n.l + m.p * n.p)
    };
}

static inline mat4 mat4rcp(mat4 m)
{
    return (mat4) {
        (m.a), (m.e), (m.i), (m.m),
        (m.b), (m.f), (m.j), (m.n),
        (m.c), (m.g), (m.k), (m.o),
        (m.d), (m.h), (m.l), (m.p)
    };
}

// ✅ Returns the identity matrix
static inline mat4 identityMat4 ( )
{
    return (mat4) {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

// ✅ Computes a translation matrix from a translation vector
static inline mat4 translationMat ( vec3 v )
{
    return (mat4) {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        v.x, v.y, v.z, 1
    };
}

// ✅ Computes scale matrix from a scale vector
static inline mat4 scaleMat ( vec3 v )
{
    return (mat4) {
        v.x, 0, 0, 0,
        0, v.y, 0, 0,
        0, 0, v.z, 0,
        0, 0, 0, 1
    };
}

// ✅ Computes a translationScale matrix from a translation vector and a scale vector
static inline mat4 translationScaleMat ( vec3 translation, vec3 scale )
{
    return (mat4) {
        scale.x, 0, 0, 0,
        0, scale.y, 0, 0,
        0, 0, scale.z, 0,
        translation.x, translation.y, translation.z, 1
    };
}

// ✅ Computes a rotation matrix from a rotation vector
static inline mat4 rotationMatrixFromVec ( vec3 rotation )
{
    return (mat4) {
        cosf(rotation.x) + powf(rotation.x, 2) * (1 - cosf(rotation.x))                 , rotation.x* rotation.y* (1 - cosf(rotation.y)) - rotation.z * sinf(rotation.y)  , rotation.x* rotation.z* (1 - cosf(rotation.z)) + rotation.y * sinf(rotation.z), 0,
        rotation.y * rotation.x * (1 - cosf(rotation.x)) + rotation.z * sinf(rotation.x), cosf(rotation.y) + powf(rotation.y, 2) * (1 - cosf(rotation.y))                 , rotation.y* rotation.z* (1 - cosf(rotation.z)) - rotation.x * sinf(rotation.z), 0,
        rotation.y * rotation.x * (1 - cosf(rotation.x)) + rotation.z * sinf(rotation.x), rotation.y * rotation.x * (1 - cosf(rotation.x)) + rotation.z * sinf(rotation.x), cosf(rotation.x) + powf(rotation.x, 2) * (1 - cosf(rotation.x))               , 0,
        0                                                                               , 0                                                                               , 0                                                                             , 1
    };
}

// ✅ Computes a model matrix from a location, rotation, and scale vector.
static inline mat4 generateModelMatrixFromVec ( vec3 location, vec3 rotation, vec3 scale )
{
    return (translationScaleMat(location,scale),rotationMatrixFromVec(rotation));
}