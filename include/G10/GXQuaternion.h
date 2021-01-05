#pragma once

#include <stdio.h>
#include <math.h>

#include <G10/GXdebug.h>
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
inline quaternion_t makeQuaternion ()
{
	return (quaternion_t) { 0.f,0.f,0.f,0.f };
}

// ✅ Creates a quaternion from a vector in euler angles
inline quaternion_t makeQuaternionFromEulerAngle ( GXvec3_t v )
{
	float sx = sinf(toRadians(v.x*2)), sy = sinf(toRadians(v.y*2)), sz = sinf(toRadians(v.z*2)), cx = cosf(toRadians(v.x*2)), cy = cosf(toRadians(v.y*2)), cz = cosf(toRadians(v.z*2));

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
		(q1.j * q2.k - q1.k * q2.j),
		(q1.k * q2.i - q1.i * q2.k),
		(q1.i * q2.j - q1.j * q2.i)
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