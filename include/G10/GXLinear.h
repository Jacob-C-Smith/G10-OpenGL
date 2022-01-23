#pragma once

#include <math.h>

// TODO: Phase out all of these functions in favor of SIMD accelerated funcitons.

#include <G10/GXtypedef.h>

// ✅ Adds vector a to vector b. Returns new vector
static void add_vec3 ( vec3 *r, vec3 a, vec3 b )
{
    r->x = a.x + b.x,
    r->y = a.y + b.y,
    r->z = a.z + b.z;
    
}

// ✅ Subtracts vector a from vector b. Returns new vector
static void sub_vec3 ( vec3 *r, vec3 a, vec3 b )
{
    r->x = a.x - b.x,
    r->y = a.y - b.y,
    r->z = a.z - b.z;
}

// ✅ Computes the dot product of 2 3D vectors
static float length ( vec3 a )
{
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

static float dot_product_vec3(vec3 a, vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// ✅ Computes the cross product of 2 3D vectors
static vec3 cross_product_vec3 ( vec3 a, vec3 b )
{
    return (vec3) {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

// ✅ Multiplies the components of one vector by the components of another
static void mul_vec3_vec3 ( vec3 *r, vec3 a, vec3 b )
{
    r->x = a.x * b.x,
    r->y = a.y * b.y,
    r->z = a.z * b.z;

}

// ✅ Divides the components of a vector by a scalar
static void div_vec3_f( vec3 *r, vec3 a, float s )
{
    r->x = a.x / s,
    r->y = a.y / s,
    r->z = a.z / s;
}


// ✅ Multiplies a vector by a scalar value
static vec3 mul_vec3_f ( vec3 v, float s )
{
    return (vec3) {
        v.x * s,
        v.y * s,
        v.z * s,
        v.w * s
    };
}

// ✅ Normailizes a vector
static vec3 normalize ( vec3 v )
{
    float vl = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z) + (v.w * v.w));
    return (vec3) { v.x / vl, v.y / vl, v.z / vl, v.w / vl };
}

// ✅ Multiplies a matrix by a vector
static vec2 mul_mat2_vec2 ( mat2 m, vec2 v)
{
    return (vec2) {
        m.a*v.x+m.b*v.y,
        m.c*v.x+m.d*v.y
    };
}

// ✅ Multiplies a matrix by a vector
static vec4 mul_mat4_vec4 ( mat4 m, vec4 v )
{
    return (vec4) {
        m.a* v.x + m.b * v.y + m.c * v.z + m.d * v.w,
        m.e* v.x + m.f * v.y + m.g * v.z + m.h * v.w,
        m.i* v.x + m.j * v.y + m.k * v.z + m.l * v.w,
        m.m* v.x + m.n * v.y + m.o * v.z + m.p * v.w
    };
}

// ✅ Multiplies a matrix by a matrix
static mat2 mul_mat2_mat2 ( mat2 m, mat2 n )
{
    return (mat2) {
        m.a* n.a + m.b * n.c, m.a* n.b + m.b * n.d,
        m.c* n.a + m.d * n.c, m.c* n.b + m.d * n.d
    };
}

// ✅ Multiplies a matrix by a matrix
static mat4 mul_mat4_mat4 ( mat4 m, mat4 n )
{
    return (mat4) {
        (m.a * n.a + m.b * n.e + m.c * n.i + m.d * n.m), (m.a * n.b + m.b * n.f + m.c * n.j + m.d * n.n), (m.a * n.c + m.b * n.g + m.c * n.k + m.d * n.o), (m.a * n.d + m.b * n.h + m.c * n.l + m.d * n.p),
        (m.e * n.a + m.f * n.e + m.g * n.i + m.h * n.m), (m.e * n.b + m.f * n.f + m.g * n.j + m.h * n.n), (m.e * n.c + m.f * n.g + m.g * n.k + m.h * n.o), (m.e * n.d + m.f * n.h + m.g * n.l + m.h * n.p),
        (m.i * n.a + m.j * n.e + m.k * n.i + m.l * n.m), (m.i * n.b + m.j * n.f + m.k * n.j + m.l * n.n), (m.i * n.c + m.j * n.g + m.k * n.k + m.l * n.o), (m.i * n.d + m.j * n.h + m.k * n.l + m.l * n.p),
        (m.m * n.a + m.n * n.e + m.o * n.i + m.p * n.m), (m.m * n.b + m.n * n.f + m.o * n.j + m.p * n.n), (m.m * n.c + m.n * n.g + m.o * n.k + m.p * n.o), (m.m * n.d + m.n * n.h + m.o * n.l + m.p * n.p)
    };
}

// ✅ Computes the inverse of a matrix
static mat2 rcp_mat2(mat2 m)
{
    return (mat2) {
        m.a, m.c,
            m.b, m.d
    };
}

// ✅ Computes the inverse of a matrix
static mat4 rcp_mat4(mat4 m)
{
    return (mat4) {
        (m.a), (m.e), (m.i), (m.m),
        (m.b), (m.f), (m.j), (m.n),
        (m.c), (m.g), (m.k), (m.o),
        (m.d), (m.h), (m.l), (m.p)
    };
}

// ✅ Returns the identity matrix
static mat2 identity_mat2()
{
    return (mat2) {
        1, 0,
        0, 1
    };
}

// ✅ Returns the identity matrix
static mat4 identity_mat4 ( )
{
    return (mat4) {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

// ✅ Computes a translation matrix from a translation vector
static mat4 translation_mat4 ( vec3 v )
{
    return (mat4) {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        v.x, v.y, v.z, 1
    };
}

static mat4 scale_mat4 ( vec3 scale )
{
    return (mat4) {
        scale.x, 0, 0, 0,
        0, scale.y, 0, 0,
        0, 0, scale.z, 0,
        0, 0, 0      , 1
    };
};

// ✅ Computes a rotation matrix from a rotation vector
static mat4 rotation_mat4_from_vec3 ( vec3 rotation )
{
    return (mat4) {
        cosf(rotation.x) + powf(rotation.x, 2) * (1 - cosf(rotation.x))                 , rotation.x* rotation.y* (1 - cosf(rotation.y)) - rotation.z * sinf(rotation.y)  , rotation.x* rotation.z* (1 - cosf(rotation.z)) + rotation.y * sinf(rotation.z), 0,
        rotation.y * rotation.x * (1 - cosf(rotation.x)) + rotation.z * sinf(rotation.x), cosf(rotation.y) + powf(rotation.y, 2) * (1 - cosf(rotation.y))                 , rotation.y* rotation.z* (1 - cosf(rotation.z)) - rotation.x * sinf(rotation.z), 0,
        rotation.y * rotation.x * (1 - cosf(rotation.x)) + rotation.z * sinf(rotation.x), rotation.y * rotation.x * (1 - cosf(rotation.x)) + rotation.z * sinf(rotation.x), cosf(rotation.x) + powf(rotation.x, 2) * (1 - cosf(rotation.x))               , 0,
        0                                                                               , 0                                                                               , 0                                                                             , 1
    };
}

// ✅ Computes a model matrix from a location, rotation, and scale vector.
static mat4 model_mat4_from_vec3 ( vec3 location, vec3 rotation, vec3 scale )
{
    return (translationScaleMat(location,scale),rotation_mat4_from_vec3(rotation));
}