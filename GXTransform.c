#include <G10/GXTransform.h>

GXtransform_t* createTransform(GXvec3_t location, GXvec3_t rotation, GXvec3_t scale)
{
	GXtransform_t* ret = malloc(sizeof(GXtransform_t));

	if (ret == 0)
		return (void*)0;

	ret->location = location;
	ret->rotation = rotation;
	ret->scale    = scale;
	ret->modelMatrix = mat4xmat4(rotationMatrixFromQuaternion(makeQuaternionFromEulerAngle(rotation)), translationScaleMat(location, scale));

	return ret;
}

int unloadTransform(GXtransform_t* transform)
{
	transform->location    = (GXvec3_t){ 0,0,0 };
	transform->rotation    = (GXvec3_t){ 0,0,0 };
	transform->scale       = (GXvec3_t){ 0,0,0 };
	transform->modelMatrix = (GXmat4_t){ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	free(transform);

	return 0;
}
