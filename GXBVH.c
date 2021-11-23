#include <G10/GXBVH.h>

GXBV_t* createBV ( void )
{
	// Initialized data
	GXBV_t *ret = calloc(1, sizeof(GXBV_t));

	// Check if the memory was allocated 
	{
		#ifndef NDEBUG
			if(ret == (void*)0)
				goto noMem;
		#endif
	}

	return ret;

	// Error handling
	{
		noMem:
			#ifndef NDEBUG
				gPrintError("[G10] [BVH] Out of memory\n");
			#endif
		return 0;
	}
}

GXBV_t* createBVFromEntity(GXEntity_t* entity)
{
	// Initialized data
	GXBV_t *ret   = createBV();

	// Copy location and scale
	ret->location = &entity->transform->location;
	ret->scale    = &entity->transform->scale;
	ret->entity   = entity;

	// We are already at the bottom of the tree, so we won't have any nodes after these
	ret->left     = (void*)0;
	ret->right    = (void*)0;

	return ret;
}

GXBV_t* createBVFromBVs(GXBV_t* a, GXBV_t* b)
{
	GXBV_t* ret = createBV();

	// Allocate for location and scale
	ret->location = calloc(1, sizeof(vec3));
	ret->scale    = calloc(1, sizeof(vec3));

	// This is not a terminal node
	ret->entity   = 0;

	// Set each bounding volume
	ret->left     = a,
	ret->right    = b;

	// Average the location
	{
		addVec3(ret->location, *a->location, *b->location);
		divVec3f(ret->location, *ret->location, 2.f);
	}
	
	// Compute the scale
	{
		vec3 maxA,
			 maxB,
			 minA,
			 minB;

		addVec3(&maxA, *a->location, *a->scale);
		subVec3(&minA, *a->location, *a->scale);
		addVec3(&maxB, *b->location, *b->scale);
		subVec3(&minB, *b->location, *b->scale);

		ret->scale->x = ((max(max(maxA.x, minA.x), max(maxB.x, minB.x))) - (min(min(maxA.x, minA.x), min(maxB.x, minB.x))))/2.f,
		ret->scale->y = ((max(max(maxA.y, minA.y), max(maxB.y, minB.y))) - (min(min(maxA.y, minA.y), min(maxB.y, minB.y))))/2.f,
		ret->scale->z = ((max(max(maxA.z, minA.z), max(maxB.z, minB.z))) - (min(min(maxA.z, minA.z), min(maxB.z, minB.z))))/2.f;

	}

	return ret;
}

float distance  (GXBV_t *a, GXBV_t *b)
{
	// Initialized data
	vec3 rV = (vec3) {0.f, 0.f, 0.f};

	// Subtract a from b. Store in rV.
	subVec3(&rV, *a->location, *b->location);

	// Square each vector component
	vec3xvec3(&rV, rV, rV);

	// Return the square root of the sum of each component
	return sqrtf(rV.x + rV.y + rV.z);
}

GXBV_t* createBVHFromScene(GXScene_t* scene)
{
	// Commentary
	{
		/*
		 * This function constructs a bounding volume heierarchy from a scene. The algorithm is
		 * relatively simple, and constructs the tree on O(n^3) time. The algorithm creates the
		 * tree in a bottum up fashion.
		 *
		 * The algorithm starts by generating a list of bounding volume heierarchies from the list
		 * of entities from the scene. The algorithm then compute which two bounding volumes are
		 * the closest together. This is an O(n^2) operation. The closest objects are removed from 
		 * the list, and are combined to form a new bounding volume that is concometently 
		 * reinserted to the list. Removing two and insterting one. The algorithm does this for as
		 * many bounding boxes are in the list, until only one bounding volume is left. This is the
		 * bounding volume of the scene, and contains every entity in the scene.
		 */
	}

	// Argument check
	{
		#ifndef NDEBUG
		if (scene == (void*)0)
			goto noScene;
		#endif
	}

	// Uninitialized data
	GXBV_t      **BVList;                            // List of references to bounding volumes 

	// Initialized data
	size_t       entitiesInScene = 0;                // How many entities are in the scene?
	size_t       i               = 0,                // Iterators      ...
		         j               = 0,                //                ...
		         bI              = 0,                // Best indicies  ...
		         bJ              = 0;                //                ...
	GXEntity_t  *e               = scene->entities;  // The list of entities

	// Count up all the entities in the scene
	while(e)
	{
		entitiesInScene++;
		e = e->next;
	}

	// Allocate a double pointer list
	BVList = calloc(entitiesInScene+1, sizeof(void*)),

	// Set the reference back to the head
	e = scene->entities;

	// Populate the double pointer list with bounding volumes
	for (i = 0; i < entitiesInScene; i++)
	{
		BVList[i] = createBVFromEntity(e);
		e = e->next;
	}
	
	// Iterate through the list until we have no more bounding volumes to combine
	while (entitiesInScene > 1)
	{
		float   best = FLT_MAX;

		GXBV_t *a = 0, 
			   *b = 0;

		// Iterate over bounding volumes with i
		for (i = 0; i < entitiesInScene; i++)

			// Iterate over bounding volumes with j
			for (j = 0; j < entitiesInScene; j++)
			{
				// Make sure that i and j are not the same. If they are, we just set the test distance to float max so it will always fail
				float pD = (BVList[i] != BVList[j]) ? distance(BVList[i], BVList[j]) : FLT_MAX;

				// If we have a better distance, we record the distance and the indicies to the bounding volume list
				if ((BVList[i] != BVList[j]) && pD < best)
					best = pD,
					bJ   = j,
					bI   = i;
					
			}


		// Reset the best distance to float max
		best = FLT_MAX;

		GXBV_t *insert = createBVFromBVs(BVList[bI], BVList[bJ]),
			   *t      = BVList[entitiesInScene - 1];

		BVList[bI] = insert;

		// XOR swap the best 
		(size_t)BVList[bJ]                  ^= (size_t)BVList[entitiesInScene - 1],
		(size_t)BVList[entitiesInScene - 1] ^= (size_t)BVList[bJ],
		(size_t)BVList[bJ]                  ^= (size_t)BVList[entitiesInScene - 1];

		entitiesInScene--;
		bI = 0,
		bJ = 0;
	}

	// If something goes wrong, we don't want to dereference a null pointer
	return (BVList) ? *BVList : 0;

	// Error handling
	{
		noScene:
		#ifndef NDEBUG
			gPrintError("[G10] [BVH] Null pointer provided for \"scene\" in call to function %s\n", __FUNCSIG__);
		#endif
		return 0;
	}
}

GXBV_t* findClosestBV(GXBV_t *bvh, GXBV_t* bv)
{
	// Argument check
	{
		// TODO
	}
	GXBV_t *ret = 0;
	
	if (checkIntersection(bvh, bv))
	{
		ret = bvh;
		if (bvh->left)
			if (checkIntersection(bvh->left, bv))
				ret = findClosestBV(bvh->left, bv);

		if(bvh->right)
			if (checkIntersection(bvh->right, bv))
				ret = findClosestBV(bvh->right, bv);

		return ret;
	}
	return ret;
}

int drawSceneBV ( GXScene_t *scene, GXPart_t *cube, GXShader_t *shader, int depth )
{
	// Argument check
	{
		// TODO
	}

	// Draw the bounding volumes in wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	drawBV(scene->BVH, cube, shader, scene->cameras, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	return 0;

	// Error handling
	{
		// TODO:
	}
}

int drawBV ( GXBV_t *bv, GXPart_t *cube, GXShader_t *shader, GXCamera_t *camera, int depth )
{
	// Argument check
	{
		// TODO
	}

	// Uninitialized data
	mat4 m;

	// Initialized data
	const vec3 colors[8] = {
					(vec3) { 1.f   , 0.f   , 0.f },   // 0 - Red
					(vec3) { 1.f   , 0.75f , 0.f },   // 1 - Orange
					(vec3) { 0.5f  , 1.f   , 0.f },   // 2 - Yellow
					(vec3) { 0.f   , 1.f   , 0.25f }, // 3 - Green
					(vec3) { 0.f   , 1.f   , 1.f },   // 4 - Cyan
					(vec3) { 0.f   , 0.25f , 1.f },   // 5 - Blue
					(vec3) { 0.75f , 0.f   , 1.f },   // 6 - Purple
					(vec3) { 1.f   , 0.f   , 0.75f }, // 7 - Pink
				   };

	// Use the provided shader, set the camera, model matrix, and color
	useShader(shader);
	setShaderCamera(shader, camera);
	m = mat4xmat4(rotationMatrixFromQuaternion((quaternion) { 1.f, 0.f, 0.f, 0.f }), translationScaleMat(*bv->location, *bv->scale));
	setShaderMat4(shader, "M", &m);

	// The color is set from accessing the list of colors at the depth modulo 8, so the colors will cycle. 
	// The intuition is that as hue increases, the bounding volumes are deeper, and as the hue decreases, the bounding volumes are larger
	setShaderVec3(shader, "color", colors[depth%8]);

	// Draw the cube
	drawPart(cube);

	// Draw the left box if there is one
	if (bv->left)
		drawBV(bv->left, cube, shader, camera, depth + 1);

	// Draw the right box if there is one
	if (bv->right)
		drawBV(bv->right, cube, shader, camera, depth + 1);

	return 0;
}

int printBV ( GXBV_t* bv, size_t d )
{
	// Argument check
	{
		// TODO
	}

	// If we are at the start of the print, we print out a header
	if (d == 0)
		gPrintLog("[G10] [BVH] Bounding volume heierarchy\n\n");

	// Indent proportional to the deapth of the BVH
	for (size_t i = 0; i < d * 4; i++)
		putchar(' ');

	// If the bounding volume is an entity, we take note of the entities name
	if (bv->entity)
		gPrintLog("\"%s\" - ", bv->entity->name);

	// In any case, we print the location and scale of the bounding volume
	gPrintLog("< %.2f %.2f %.2f > - < %.2f %.2f %.2f >\n", bv->location->x, bv->location->y, bv->location->z, bv->scale->x, bv->scale->y, bv->scale->z);

	// Print the left node
	if (bv->left)
		printBV(bv->left, d+1);

	// Print the right node
	if (bv->right)
		printBV(bv->right, d+1);
	
	// If we are at the end of the print, we print a few newlines
	if (d == 0)
		gPrintLog("\n\n");

	return 0;

	// Error handling
	{
		// TODO
	}
}

bool checkIntersection(GXBV_t *a, GXBV_t *b)
{
	return ((a->location->x - a->scale->x) <= (b->location->x + b->scale->x) && (a->location->x + a->scale->x) >= (b->location->x - b->scale->x) &&
            (a->location->y - a->scale->y) <= (b->location->y + b->scale->y) && (a->location->y + a->scale->y) >= (b->location->y - b->scale->y) && 
            (a->location->z - a->scale->z) <= (b->location->z + b->scale->z) && (a->location->z + a->scale->z) >= (b->location->z - b->scale->z));
}
