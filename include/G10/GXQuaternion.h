#pragma once

#include <stdio.h>
#include <math.h>

#include <G10/GXtypedef.h>
#include <G10/GXLinear.h>
#include <G10/GXilmath.h>

quaternion    identity_quaternion                 ( void );                      // ✅ Creates quaternion (1,0,0,0)

quaternion    make_quaternion_from_euler_angle    ( vec3       v );// ✅ Creates a quaternion from a vector in euler angles

vec3          euler_angle_from_quaternion         ( quaternion q );
quaternion    quaternion_inverse                  ( quaternion q );
quaternion    multiply_quaternion_quaternion_vec4 ( quaternion q1, quaternion q2 ); // ✅ Multiplies two quaternions as vectors

quaternion    multiply_quaternion_quaternion      ( quaternion q1, quaternion q2 );       // ✅ Multiplies two quaternions

mat4          rotation_mat4_from_quaternion       ( quaternion q );                       // ✅ Creates a rotation matrix from a quaternion
void          rotate_vec3_by_quaternion           ( vec3      *r , vec3       v, quaternion q );

quaternion    normalize_quaternion                ( quaternion q );
quaternion    q_slerp                             ( quaternion q0, quaternion q1, float delta_time);