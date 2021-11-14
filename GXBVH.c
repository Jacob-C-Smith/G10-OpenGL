#include <G10/GXBVH.h>

GXBV_t* createBV ( void )
{
	// Initialized data
	GXBV_t *ret = calloc(1, sizeof(ret));

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
	ret->location = calloc(2, sizeof(vec3));
	ret->scale    = ret->location + sizeof(vec3);

	// This is not a terminal node
	ret->entity   = 0;

	// Set each bounding volume
	ret->left     = a,
	ret->right    = b;

	// Average the location
	addVec3(ret->location, *a->location, *b->location);
	divVec3f(ret->location, *ret->location, 2.f);
	
	return ret;
}

float distance  (GXEntity_t *a, GXEntity_t *b)
{
	// Initialized data
	vec3 rV = (vec3) {0.f, 0.f, 0.f};

	// Subtract a from b. Store in rV.
	subVec3(&rV, a->transform->location, b->transform->location);

	// Square each vector component
	vec3xvec3(&rV, rV, rV);

	// Return the square root of the sum of each component
	return sqrtf(rV.x + rV.y + rV.z);
}

void swap (size_t *x, size_t *y)
{
	if (x != y)
	{
		*(size_t*)x ^= *(size_t*)y;
		*(size_t*)y ^= *(size_t*)x;
		*(size_t*)x ^= *(size_t*)y;
	}
}

GXBV_t* createBVHFromScene(GXScene_t* scene)
{
	// Commentary
	{
		/*
		 * This function constructs a bounding volume heierarchy from a scene. The algorithm is
		 * relatively simple, and constructs the tree on O(n^3) time. The algorithm creates the
		 * tree in a top down fashion.
		 *
		 * We start out with a list of bounding volume heierarchies, corresponding to each entity.
		 * We then compute which two bounding volumes are the closest together, and create a new 
		 * bounding volume. We then remove both of the entity bounding boxes from the list, and 
		 * insert the new bounding volume. From the list, we remove two volumes, and add one. We 
		 * keep on going until we are left with one bounding volume. This is the bounding volume
		 * of the scene. 
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
	size_t       iC,
		         jC;
	GXEntity_t **i,
		       **j;

	// Initialized data
	size_t       entitiesInScene = 0;
	GXEntity_t  *e               = scene->entities;

	// Count up all the entities in the scene
	while(e)
	{
		entitiesInScene++;
		e = e->next;
	}

	// Allocate two double pointer list
	i = calloc(entitiesInScene, sizeof(void*)),
	j = calloc(entitiesInScene, sizeof(void*));

	// Set the reference back to the head
	e = scene->entities;

	// Populate the double pointer lists
	for (size_t iter = 0; iter < entitiesInScene; iter++)
	{
		i[iter] = e;
	    j[iter] = e;
		e = e->next;
	}
	
	// Iterate through the list until we have no more objects
	while (entitiesInScene > 1)
	{
		float   best = FLT_MAX;

		GXBV_t *a = 0, 
			   *b = 0;

		// Iterate over entities in i
		for (iC = 0; iC < entitiesInScene; iC++)
		{
			
			// Iterate over entites in j
			for (jC = 0; jC < entitiesInScene; jC++)
			{
				// Make sure that i and j are not the same
				float pD = (i[iC] != j[jC]) ? distance(i[iC], j[jC]) : FLT_MAX;

				if ((i[iC] != j[jC]) && pD < best)
				{
					best = pD;
					gPrintLog("[G10] [BVH] Distance between \"%s\" and \"%s\" is %50f\n", i[iC]->name, j[jC]->name, best);
			
					// Left and right nodes
					a = i[iC],
					b = j[jC];
				}
				
			}
			best = FLT_MAX;
		}


		GXBV_t *aB = createBVFromEntity(a),
			   *bB = createBVFromEntity(b),
			   *cB = createBVFromBVs(a,b);
			   
	}


	return 0;
	// Error handling
	{
		noScene:
		#ifndef NDEBUG
			gPrintError("[G10] [BVH] Null pointer provided for \"scene\" in call to function %s\n", __FUNCSIG__);
		#endif
		return 0;
	}
}

bool checkIntersection(GXBV_t* a, GXBV_t* b)
{
	return false;
}
