#include <G10/GXRigidbody.h>

GXRigidbody_t* createRigidBody ( float mass, bool useGravity )
{
	// Initialized data
	GXRigidbody_t* ret = malloc(sizeof(GXRigidbody_t));
	if (ret == (void*)0)
		return ret;

	ret->velocity = (GXvec3_t){ 0,0,0 };
	ret->acceleration = (useGravity) ? (GXvec3_t) { 0.f, -0.f, -0.098f} : (GXvec3_t) { 0.f,0.f,0.f } ;
	ret->mass = mass;
	ret->useGravity = useGravity;

	return ret;
}

int updatePositionAndVelocity ( GXRigidbody_t* rigidbody, GXTransform_t* transform, u32 deltaTime )
{
	GXvec3_t f = applyForce(rigidbody);
	GXvec3_t a = vec3xf(f, 1 / rigidbody->mass);
	rigidbody->velocity = addVec3(rigidbody->velocity, vec3xf(a, deltaTime / 1000.f));
	transform->location = addVec3(transform->location, vec3xf(rigidbody->velocity, deltaTime / 1000.f));

	return 0;
}

int computeCollision ( GXRigidbody_t* a, GXRigidbody_t* b )
{
	;
	return 0;
}
 
int destroyRigidBody ( GXRigidbody_t* rigidbody )
{
	rigidbody->mass         = 0;
	rigidbody->acceleration = (GXvec3_t){ 0.f,0.f,0.f };
	rigidbody->useGravity   = 0;

	free(rigidbody);

	return 0;
}
