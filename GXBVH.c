#include <G10/GXBVH.h>

GXBV_t* create_bv ( void )
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

	// Allocate for location and scale
	ret->location = calloc(1, sizeof(vec3));
	ret->scale    = calloc(1, sizeof(vec3));

	return ret;

	// Error handling
	{
		noMem:
			#ifndef NDEBUG
				g_print_error("[G10] [BVH] Out of memory\n");
			#endif
		return 0;
	}
}

GXBV_t* create_bv_from_entity(GXEntity_t* entity)
{
	// TODO: Argument check
	{
		// 
	}

	// Initialized data
	GXBV_t *ret   = create_bv();

	// Copy location and scale
	free(ret->location);
	ret->location = &entity->transform->location;
	memcpy(ret->scale, &entity->transform->scale, sizeof(vec3));

	ret->entity   = entity;

	// Make a collider if there isn't one
	if (entity->collider == (void*)0)
		entity->collider     = create_collider();

	if (strcmp(entity->name, "Tank") == 0)
		ret->scale->x *= 4,
		ret->scale->y *= 4,
		ret->scale->z *= 4;

	// Set the collider if there isn't one
	if (entity->collider->bv == (void*)0)
		entity->collider->bv = ret;

	// We are already at the bottom of the tree, so we won't have any nodes after these
	ret->left     = (void*)0;
	ret->right    = (void*)0;

	return ret;

	// TODO: Error handling
	{

	}
}

GXBV_t* create_bv_from_bvs(GXBV_t* a, GXBV_t* b)
{
	// TODO: Argument check
	GXBV_t* ret = create_bv();

	// This is not a terminal node
	ret->entity   = 0;

	// Set each bounding volumes
	ret->left     = a,
	ret->right    = b;
	
	// TODO: Refactor with AVX
	
	// Compute the scale
	{
		vec3 maxA,
			 maxB,
			 minA,
			 minB,
			 maxBound,
			 minBound;

		add_vec3(&maxA, *a->location, *a->scale);
		sub_vec3(&minA, *a->location, *a->scale);
		add_vec3(&maxB, *b->location, *b->scale);
		sub_vec3(&minB, *b->location, *b->scale);

		ret->scale->x = ((max(max(maxA.x, minA.x), max(maxB.x, minB.x))) - (min(min(maxA.x, minA.x), min(maxB.x, minB.x))))/2.f,
		ret->scale->y = ((max(max(maxA.y, minA.y), max(maxB.y, minB.y))) - (min(min(maxA.y, minA.y), min(maxB.y, minB.y))))/2.f,
		ret->scale->z = ((max(max(maxA.z, minA.z), max(maxB.z, minB.z))) - (min(min(maxA.z, minA.z), min(maxB.z, minB.z))))/2.f;

		maxBound      = (vec3){ max(maxA.x, maxB.x), max(maxA.y, maxB.y), max(maxA.z, maxB.z) },
		minBound      = (vec3){ min(minA.x, minB.x), min(minA.y, minB.y), min(minA.z, minB.z) };

		add_vec3(ret->location, minBound, maxBound);
		ret->location->x /= 2.f,
		ret->location->y /= 2.f,
		ret->location->z /= 2.f;
	}

	return ret;
	// TODO: Error handling
}

float distance  (GXBV_t *a, GXBV_t *b)
{
	// TODO: Argument check
	// Initialized data
	vec3 rV = (vec3) {0.f, 0.f, 0.f};

	// Subtract a from b. Store in rV.
	sub_vec3(&rV, *a->location, *b->location);

	// Square each vector component
	mul_vec3_vec3(&rV, rV, rV);

	// Return the square root of the sum of each component
	return sqrtf(rV.x + rV.y + rV.z);

	// TODO: Error handling
}

GXBV_t* create_bvh_from_scene(GXScene_t* scene)
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
		if (e->collider)
			if (e->collider->bv)
			{
				BVList[i] = e->collider->bv;
				e = e->next;
				continue;
			}
		BVList[i] = create_bv_from_entity(e);
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

		GXBV_t *insert = create_bv_from_bvs(BVList[bI], BVList[bJ]),
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
			g_print_error("[G10] [BVH] Null pointer provided for \"scene\" in call to function %s\n", __FUNCSIG__);
		#endif
		return 0;
	}
}

GXBV_t* find_closest_bv ( GXBV_t *bvh, GXBV_t* bv ) 
{
	// TODO: Argument check
	{
		// 
	}
	GXBV_t *ret = 0;
	
	return ret;
	// TODO: Error handling

}

GXBV_t* find_parent_bv ( GXBV_t *bvh, GXBV_t *bv )
{
	// TODO: Argument check
	{
		
	}

	GXBV_t* ret = 0;
	u32     path = 0;
	if (checkIntersection(bvh, bv))
	{
		if(bvh->left)
			if (checkIntersection(bvh->left,bv))
			{
			
			}
		if(bvh->right)
			if (checkIntersection(bvh->right, bv));
			{
	
			}
	
	}

	return ret;
	// TODO: Error handling

}

bool testCollision(GXEntity_t* entity, GXBV_t* bvh)
{
	// TODO: Argument check
	GXBV_t *e = entity->collider->bv;
	e = find_closest_bv(bvh, e);
	return false;
	// TODO: Error handling

}

int draw_scene_bv ( GXScene_t *scene, GXPart_t *cube, GXShader_t *shader, int depth )
{
	// TODO: Argument check
	{
		// 
	}

	// Draw the bounding volumes in wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
	draw_bv(scene->bvh, cube, shader, scene->cameras, 0);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	return 0;

	// TODO:Error handling
	{
		// 
	}
}

int draw_bv ( GXBV_t *bv, GXPart_t *cube, GXShader_t *shader, GXCamera_t *camera, int depth )
{
	// TODO: Argument check
	{
		// 
	}

	// Uninitialized data
	mat4 m;

	// Initialized data
	const vec3 colors[8] = {
					(vec3) { 1.f   , 0.f   , 0.f },   // 0 - Red
					(vec3) { 8.f   , 0.65f , 0.f },   // 1 - Orange
					(vec3) { 0.75f , 0.82f , 0.4f },  // 2 - Yellow
					(vec3) { 0.f   , 0.75f , 0.25f }, // 3 - Green
					(vec3) { 0.f   , 1.f   , 1.f },   // 4 - Cyan
					(vec3) { 0.f   , 0.25f , 1.f },   // 5 - Blue
					(vec3) { 0.75f , 0.f   , 1.f },   // 6 - Purple
					(vec3) { 1.f   , 0.f   , 0.75f }, // 7 - Pink
				   };

	// Use the provided shader, set the camera, model matrix, and color
	use_shader(shader);
	set_shader_camera(shader, camera);
	m = mul_mat4_mat4(mul_mat4_mat4(scale_mat4(*bv->scale), rotation_mat4_from_quaternion((quaternion) { 1.f, 0.f, 0.f, 0.f })), translation_mat4(*bv->location));
	set_shader_mat4(shader, "M", &m);

	// The color is set from accessing the list of colors at the depth modulo 8, so the colors will cycle. 
	// The intuition is that as hue increases, the bounding volumes are deeper, and as the hue decreases, the bounding volumes are larger
	set_shader_vec3(shader, "color", colors[depth%8]);

	// Draw the cube
	draw_part(cube);

	// Draw the left box if there is one
	if (bv->left)
		draw_bv(bv->left, cube, shader, camera, depth + 1);

	// Draw the right box if there is one
	if (bv->right)
		draw_bv(bv->right, cube, shader, camera, depth + 1);

	return 0;
	// TODO: Error handling

}

int print_bv ( GXBV_t* bv, size_t d )
{
	// TODO: Argument check
	{
		// 
		if(bv == (void*) 0)
			return 0;
	}

	// If we are at the start of the print, we print out a header
	if (d == 0)
		g_print_log("[G10] [BVH] Bounding volume heierarchy\n[ entity name ] - < location > - < scale >\n\n");

	// Indent proportional to the deapth of the BVH
	for (size_t i = 0; i < d * 4; i++)
		putchar(' ');

	// If the bounding volume is an entity, we take note of the entities name
	if (bv->entity)
		g_print_log("\"%s\" ", bv->entity->name);

	// In any case, we print the location and scale of the bounding volume
	g_print_log("- < %.2f %.2f %.2f > - < %.2f %.2f %.2f >\n", bv->location->x, bv->location->y, bv->location->z, bv->scale->x, bv->scale->y, bv->scale->z);

	// Print the left node
	if (bv->left)
		print_bv(bv->left, d+1);

	// Print the right node
	if (bv->right)
		print_bv(bv->right, d+1);
	
	// If we are at the end of the print, we print a few newlines
	if (d == 0)
		g_print_log("\n\n");

	return 0;

	// TODO: Error handling
	{
		// 
	}
}

size_t get_entities_from_bv(GXBV_t* bv, GXEntity_t** entities, size_t count)
{
	// Argument check
	{
		// TODO
		if (entities == 0)
			goto countingMode;
	}

	// Increment if we are on a bv
	if (bv->entity)
		entities[--count] = bv->entity;

	if (bv->left)
		if (bv->left->entity)
			entities[--count] = bv->left->entity;
		else
			count = get_entities_from_bv(bv->left, entities, count);

	if (bv->right)
		if (bv->right->entity)
			entities[--count] = bv->right->entity;
		else
			count = get_entities_from_bv(bv->right, entities, count);



	return count;

	return 0;

	// Counting mode counts how many entities are present in a bounding volume, 
	// and returns the count so you can calloc() some ram for the entity pointers
	countingMode:
	{
		// Initialized data
		size_t ret = 0;

		// Increment if we are on a bv
		if (bv)
		{
			if (bv->entity)
				ret++;

			// Check if our left is valid
			if (bv->left)

				// Check if our left is an entity. If so, increment
				if (bv->left->entity)
					ret++;

			// Otherwise, parse the left volume
				else
					ret += get_entities_from_bv(bv->left, entities, count);

			// Same as above, but for right
			if (bv->right)
				ret = (bv->right->entity) ? ret + 1 : ret + get_entities_from_bv(bv->right, entities, count);
		}
		return ret;
	}
	// TODO: Error handling

}

bool checkIntersection(GXBV_t *a, GXBV_t *b)
{
	// TODO: Argument check
	return ((a->location->x - a->scale->x) <= (b->location->x + b->scale->x) && (a->location->x + a->scale->x) >= (b->location->x - b->scale->x) &&
            (a->location->y - a->scale->y) <= (b->location->y + b->scale->y) && (a->location->y + a->scale->y) >= (b->location->y - b->scale->y) && 
            (a->location->z - a->scale->z) <= (b->location->z + b->scale->z) && (a->location->z + a->scale->z) >= (b->location->z - b->scale->z));
	// TODO: Error handling

}
