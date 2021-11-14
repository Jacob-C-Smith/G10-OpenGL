#pragma once

#include <stdio.h>
#include <math.h>

#include <G10/GXtypedef.h>
#include <G10/GXLinear.h>
#include <G10/GXilmath.h>

// ✅ Creates quaternion (0,0,0,0)
inline quaternion    makeQuaternion               ( )
{
    return (quaternion) { 0.f,0.f,0.f,0.f };
}

// ✅ Creates a quaternion from a vector in euler angles
inline quaternion    makeQuaternionFromEulerAngle ( vec3       v )
{
    float sx = sinf(toRadians(v.x*2)), 
          sy = sinf(toRadians(v.y*2)), 
          sz = sinf(toRadians(v.z*2)),
          cx = cosf(toRadians(v.x*2)), 
          cy = cosf(toRadians(v.y*2)),
          cz = cosf(toRadians(v.z*2));

    return (quaternion) {
        (cz * cx * cy + sz * sx * sy),
        (sz * cx * cy - cz * sx * sy),
        (cz * sx * cy + sz * cx * sy),
        (cz * cx * sy - sz * sx * cy)
    };
}

// ✅ Multiplies two quaternions as vectors
inline quaternion    multiplyQuaternionVector     ( quaternion q1, quaternion q2 )
{
    return (quaternion) {
        (-q1.i * q2.i - q1.j * q2.j - q1.k * q2.k),
        (q1.j  * q2.k - q1.k * q2.j),
        (q1.k  * q2.i - q1.i * q2.k),
        (q1.i  * q2.j - q1.j * q2.i)
    };
}

// ✅ Multiplies two quaternions
inline quaternion    multiplyQuaternion           ( quaternion q1, quaternion q2 )
{
    return (quaternion) {
        (q1.u * q2.u - q1.i * q2.i - q1.j * q2.j - q1.k * q2.k),
        (q1.u * q2.i + q1.i * q2.u + q1.j * q2.k - q1.k * q2.j),
        (q1.u * q2.j + q1.j * q2.u + q1.k * q2.i - q1.i * q2.k),
        (q1.u * q2.k + q1.k * q2.u + q1.i * q2.j - q1.j * q2.i)
    };
}

// ✅ Creates a rotation matrix from a quaternion
inline mat4          rotationMatrixFromQuaternion ( quaternion q )
{
    return (mat4) {
        (q.u * q.u + q.i * q.i - q.j * q.j - q.k * q.k), (2 * q.i * q.j - 2 * q.k * q.u)                , (2 * q.i * q.k + 2 * q.j * q.u)                , 0,
        (2 * q.i * q.j + 2 * q.k * q.u)                , (q.u * q.u - q.i * q.i + q.j * q.j - q.k * q.k), (2 * q.j * q.k - 2 * q.i * q.u)                , 0,
        (2 * q.i * q.k - 2 * q.j * q.u)                , (2 * q.j * q.k + 2 * q.i * q.u)                , (q.u * q.u - q.i * q.i - q.j * q.j + q.k * q.k), 0,
        0                                              , 0                                              , 0                                              , 1
    };
}

inline quaternion    normalizeQuaternion          ( quaternion q )
{
    quaternion ret = { 0.f, 0.f, 0.f, 0.f };
    float vl = sqrtf((q.i * q.i) + (q.j * q.j) + (q.k * q.k));
    return (quaternion) { 0.f, q.i / vl, q.j / vl, q.k / vl };
}

inline quaternion    qSlerp                       ( quaternion q0, quaternion q1, float deltaTime )
{
    // Uninitialized data
    float sinht,
          ht,
          rA,
          rB;

    // Initialized data
    // The cosine of the half angle of the quaternions
    float cosht = q0.u * q1.u + q0.i * q1.i + q0.j * q1.j + q0.k * q1.k;
    
    // If the half angle is zero, we have nothing to do.
    if(fabsf(cosht) >= 1.0)
        return q0;
    
    // Compute the half angle and the sin of the half angle
    ht    = acosf(cosht);
    sinht = sqrtf(1.f - cosht * cosht);

    // If theta = 180, we can rotate around any axis
    if (fabsf(sinht) < 0.001f)
        return (quaternion) { 
            (q0.u * 0.5f + q1.u * 0.5f),
            (q0.i * 0.5f + q1.i * 0.5f),
            (q0.j * 0.5f + q1.j * 0.5f),
            (q0.k * 0.5f + q1.k * 0.5f)
        };

    // Compute the ratios to step by
    rA = sinf((1 - deltaTime) * ht) / sinht,
    rB = sinf(deltaTime*ht) / sinht;

    return (quaternion) { 
        (q0.u * rA + q1.u * rB),
        (q0.i * rA + q1.i * rB),
        (q0.j * rA + q1.j * rB),
        (q0.k * rA + q1.k * rB)
    };

}