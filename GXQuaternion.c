#include <G10/GXQuaternion.h>

// ✅ Creates quaternion (1,0,0,0)
quaternion    identity_quaternion()
{
    return (quaternion) { 1.f, 0.f, 0.f, 0.f };
}

// ✅ Creates a quaternion from a vector in euler angles
quaternion    make_quaternion_from_euler_angle(vec3       v)
{
    float sx = sinf(to_radians(v.x * 2)),
        sy = sinf(to_radians(v.y * 2)),
        sz = sinf(to_radians(v.z * 2)),
        cx = cosf(to_radians(v.x * 2)),
        cy = cosf(to_radians(v.y * 2)),
        cz = cosf(to_radians(v.z * 2));

    return (quaternion) {
        (cz * cx * cy + sz * sx * sy),
            (sz * cx * cy - cz * sx * sy),
            (cz * sx * cy + sz * cx * sy),
            (cz * cx * sy - sz * sx * cy)
    };
}

vec3          euler_angle_from_quaternion(quaternion q)
{
    float wD = acosf(q.u / 2),
          iD = sin(q.i / wD),
          jD = sin(q.j / wD),
          kD = sin(q.k / wD);
    return (vec3) { iD, jD, kD };
}

quaternion    quaternion_inverse(quaternion q)
{
    return (quaternion) { q.u, -q.i, -q.j, -q.k };
}

// ✅ Multiplies two quaternions as vectors
quaternion    multiply_quaternion_quaternion_vec4(quaternion q1, quaternion q2)
{
    return (quaternion) {
        (-q1.i * q2.i - q1.j * q2.j - q1.k * q2.k),
            (q1.j * q2.k - q1.k * q2.j),
            (q1.k * q2.i - q1.i * q2.k),
            (q1.i * q2.j - q1.j * q2.i)
    };
}

// ✅ Multiplies two quaternions
quaternion    multiply_quaternion_quaternion(quaternion q1, quaternion q2)
{
    return (quaternion) {
        (q1.u * q2.u - q1.i * q2.i - q1.j * q2.j - q1.k * q2.k),
            (q1.u * q2.i + q1.i * q2.u + q1.j * q2.k - q1.k * q2.j),
            (q1.u * q2.j + q1.j * q2.u + q1.k * q2.i - q1.i * q2.k),
            (q1.u * q2.k + q1.k * q2.u + q1.i * q2.j - q1.j * q2.i)
    };
}

// ✅ Creates a rotation matrix from a quaternion
mat4          rotation_mat4_from_quaternion(quaternion q)
{
    return (mat4) {
        (q.u * q.u + q.i * q.i - q.j * q.j - q.k * q.k), (2 * q.i * q.j - 2 * q.k * q.u)                , (2 * q.i * q.k + 2 * q.j * q.u)                , 0,
        (2 * q.i * q.j + 2 * q.k * q.u)                , (q.u * q.u - q.i * q.i + q.j * q.j - q.k * q.k), (2 * q.j * q.k - 2 * q.i * q.u)                , 0,
        (2 * q.i * q.k - 2 * q.j * q.u)                , (2 * q.j * q.k + 2 * q.i * q.u)                , (q.u * q.u - q.i * q.i - q.j * q.j + q.k * q.k), 0,
        0                                              , 0                                              , 0                                              , 1
    };
}

void          rotate_vec3_by_quaternion(vec3* r, vec3 v, quaternion q)
{
    quaternion p  = (quaternion){ 0.f, v.x, v.y, v.z };
    quaternion pP = normalize_quaternion(multiply_quaternion_quaternion(multiply_quaternion_quaternion(q, p), quaternion_inverse(q)));

    r->x = pP.i,
    r->y = pP.j,
    r->z = pP.k;
}

quaternion    normalize_quaternion(quaternion q)
{
    quaternion ret = { 0.f, 0.f, 0.f, 0.f };
    float vl = sqrtf((q.i * q.i) + (q.j * q.j) + (q.k * q.k));
    return (quaternion) { 0.f, q.i / vl, q.j / vl, q.k / vl };
}

quaternion    q_slerp(quaternion q0, quaternion q1, float delta_time)
{
    // Uninitialized data
    float sinht,
          ht,
          rA,
          rB;

    // Initialized data
    // The cosine of the half angle of the quaternions
    float cosht = q0.u * q1.u + q0.i * q1.i + q0.j * q1.j + q0.k * q1.k;

    // TODO: Argument check?
    // If the half angle is zero, do nothing
    if (fabsf(cosht) >= 1.0)
        return q0;

    // Compute the half angle and the sin of the half angle
    ht    = acosf(cosht);
    sinht = sqrtf(1.f - cosht * cosht);

    // If theta = 180, rotate around any axis
    if (fabsf(sinht) < 0.001f)
        return (quaternion) {
        (q0.u * 0.5f + q1.u * 0.5f),
        (q0.i * 0.5f + q1.i * 0.5f),
        (q0.j * 0.5f + q1.j * 0.5f),
        (q0.k * 0.5f + q1.k * 0.5f)
    };

    // Compute the ratios to step by
    rA = sinf((1 - delta_time) * ht) / sinht,
    rB = sinf(delta_time * ht) / sinht;

    return (quaternion) {
        (q0.u * rA + q1.u * rB),
        (q0.i * rA + q1.i * rB),
        (q0.j * rA + q1.j * rB),
        (q0.k * rA + q1.k * rB)
    };

}