#include <G10/GXTransform.h>

GXtransform_t* createTransform( GXvec3_t location, GXvec3_t rotation, GXvec3_t scale )
{
	// Initialized data
	GXtransform_t* ret = malloc(sizeof(GXtransform_t));

	// Check if ret is valid
	if (ret == 0)
		return (void*)0;

	// Set vectors
	ret->location = location;
	ret->rotation = rotation;
	ret->scale    = scale;

	// Compute a model matrix from the translation, rotation, and scale matricies
	ret->modelMatrix = mat4xmat4(rotationMatrixFromQuaternion(makeQuaternionFromEulerAngle(rotation)), translationScaleMat(location, scale));

	// Return the transform
	return ret;
}

int unloadTransform( GXtransform_t* transform )
{
	// Nullify vectors
	transform->location    = (GXvec3_t){ 0,0,0 };
	transform->rotation    = (GXvec3_t){ 0,0,0 };
	transform->scale       = (GXvec3_t){ 0,0,0 };

	// Nullify model matrix
	transform->modelMatrix = (GXmat4_t){ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	// Free the transform
	free(transform);

	// Return nullptr
	return 0;
}
