#pragma once

#include <stdio.h>
#include <math.h>

#include <G10/GXLinear.h>
#include <G10/GXilmath.h>

// Quaternion
struct quaternion_s
{
    float u;
    float i;
    float j;
    float k;
};
typedef struct quaternion_s quaternion_t;

// ✅ Creates quaternion (0,0,0,0)
inline quaternion_t makeQuaternion ( )
{
    return (quaternion_t) { 0.f,0.f,0.f,0.f };
}

// ✅ Creates a quaternion from a vector in euler angles
inline quaternion_t makeQuaternionFromEulerAngle ( GXvec3_t v )
{
    float sx = sinf(toRadians(v.x*2)), 
          sy = sinf(toRadians(v.y*2)), 
          sz = sinf(toRadians(v.z*2)),
          cx = cosf(toRadians(v.x*2)), 
          cy = cosf(toRadians(v.y*2)),
          cz = cosf(toRadians(v.z*2));

    return (quaternion_t) {
        (cz * cx * cy + sz * sx * sy),
        (sz * cx * cy - cz * sx * sy),
        (cz * sx * cy + sz * cx * sy),
        (cz * cx * sy - sz * sx * cy)
    };
}

// ✅ Multiplys two quaternions as vectors
inline quaternion_t multiplyQuaternionVector ( quaternion_t q1, quaternion_t q2 )
{
    return (quaternion_t) {
        (-q1.i * q2.i - q1.j * q2.j - q1.k * q2.k),
        (q1.j  * q2.k - q1.k * q2.j),
        (q1.k  * q2.i - q1.i * q2.k),
        (q1.i  * q2.j - q1.j * q2.i)
    };
}

// ✅ Multiplys two quaternions
inline quaternion_t multiplyQuaternion ( quaternion_t q1, quaternion_t q2 )
{
    return (quaternion_t) {
        (q1.u * q2.u - q1.i * q2.i - q1.j * q2.j - q1.k * q2.k),
        (q1.u * q2.i + q1.i * q2.u + q1.j * q2.k - q1.k * q2.j),
        (q1.u * q2.j + q1.j * q2.u + q1.k * q2.i - q1.i * q2.k),
        (q1.u * q2.k + q1.k * q2.u + q1.i * q2.j - q1.j * q2.i)
    };
}

// ✅ Creates a rotation matrix from a quaternion
inline GXmat4_t     rotationMatrixFromQuaternion ( quaternion_t q )
{
    return (GXmat4_t) {
        (q.u * q.u + q.i * q.i - q.j * q.j - q.k * q.k), (2 * q.i * q.j - 2 * q.k * q.u)                , (2 * q.i * q.k + 2 * q.j * q.u)                , 0,
        (2 * q.i * q.j + 2 * q.k * q.u)                , (q.u * q.u - q.i * q.i + q.j * q.j - q.k * q.k), (2 * q.j * q.k - 2 * q.i * q.u)                , 0,
        (2 * q.i * q.k - 2 * q.j * q.u)                , (2 * q.j * q.k + 2 * q.i * q.u)                , (q.u * q.u - q.i * q.i - q.j * q.j + q.k * q.k), 0,
        0                                              , 0                                              , 0                                              , 1
    };
}

inline quaternion_t qSlerp(quaternion_t q0, quaternion_t q1, float deltaTime)
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
    if(abs(cosht) >= 1.0)
        return q0;
    
    // Compute the half angle and the sin of the half angle
    ht    = acosf(cosht);
    sinht = sqrtf(1.0 - cosht * cosht);

    // If theta = 180, we can rotate around any axis
    if (fabs(sinht) < 0.001)
        return (quaternion_t) { 
            (q0.u * 0.5 + q1.u * 0.5),
            (q0.i * 0.5 + q1.i * 0.5),
            (q0.j * 0.5 + q1.j * 0.5),
            (q0.k * 0.5 + q1.k * 0.5)
        };

    // Compute the ratios to step by
    rA = sinf((1 - deltaTime) * ht) / sinht,
    rB = sinf(deltaTime*ht) / sinht;

    return (quaternion_t) { 
        (q0.u * rA + q1.u * rB),
        (q0.i * rA + q1.i * rB),
        (q0.j * rA + q1.j * rB),
        (q0.k * rA + q1.k * rB)
    };

}