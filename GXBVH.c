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

	return ret;

	// Error handling
	{

		// Standard l
		noMem:
			#ifndef NDEBUG
				g_print_error("[G10] [BVH] Out of memory\n");
			#endif
		return 0;
	}
}

GXBV_t* create_bv_from_entity(GXEntity_t* entity)
{
	// Argument check
	{
		if (entity == (void *)0)
			goto noEntity;
	}

	// Initialized data
	GXBV_t *ret   = create_bv();

	// Make a collider if there isn't one
	if (entity->collider == (void*)0)
	{
		entity->collider = create_collider();

		ret->location = &entity->transform->location;
		ret->dimensions = &entity->transform->scale;
	}

	// Copy location and scale
	else
	{
		ret->location = &entity->transform->location;
		ret->dimensions = &entity->collider->aabb_dimensions;
	}

	ret->entity   = entity;

	no_collider:
	
	// Set the collider if there isn't one
	if (entity->collider->bv == (void*)0)
		entity->collider->bv = ret;

	// Entity nodes are by definition leafs
	ret->left     = (void*)0;
	ret->right    = (void*)0;

	return ret;

	// Error handling
	{
		noEntity:
		#ifndef NDEBUG
			g_print_error("[G10] [BVH] Null pointer provided for \"entity\" in call to function \"%s\"\n", __FUNCSIG__);
		#endif
		return 0;
	}
}

GXBV_t* create_bv_from_bvs(GXBV_t* a, GXBV_t* b)
{

	// Argument check
	{
		#ifndef NDEBUG
			if (a == (void*)0)
				goto no_a;
			if (b == (void*)0)
				goto no_b;
		#endif
	}

	GXBV_t* ret = create_bv();

	// Allocate for location and scale
	ret->location = calloc(1, sizeof(vec3));
	ret->dimensions = calloc(1, sizeof(vec3));

	// This is not a terminal node
	ret->entity   = 0;

	// Set each bounding volumes
	ret->left     = a,
	ret->right    = b;
	
	// TODO: Refactor with AVX
	// Compute the scale
	resize_bv(ret);

	return ret;

	// Error handling
	{
		
		// Argument errors
		{
			no_a:
			#ifndef NDEBUG
				g_print_error("[G10] [BVH] Null pointer provided for \"a\" in call to function \"%s\"\n",__FUNCSIG__);
			#endif
			return 0;
			no_b:
			#ifndef NDEBUG
				g_print_error("[G10] [BVH] Null pointer provided for \"b\" in call to function \"%s\"\n",__FUNCSIG__);
			#endif
			return 0;
		}
	}
}

float distance  (GXBV_t *a, GXBV_t *b)
{
	// Argument check
	{
		#ifndef NDEBUG
			if (a == (void*)0)
				goto no_a;
			if (b == (void*)0)
				goto no_b;
		#endif
	}

	// Initialized data
	vec3 rV = (vec3) {0.f, 0.f, 0.f};

	// Subtract a from b. Store in rV.
	sub_vec3(&rV, *a->location, *b->location);

	// Square each vector component
	mul_vec3_vec3(&rV, rV, rV);

	// Return the square root of the sum of each component
	return sqrtf(rV.x + rV.y + rV.z);

	// Error handling
	{
		
		// Argument errors
		{
			no_a:
			#ifndef NDEBUG
				g_print_error("[G10] [BVH] Null pointer provided for \"a\" in call to function \"%s\"\n",__FUNCSIG__);
			#endif
			return 0;
			no_b:
			#ifndef NDEBUG
				g_print_error("[G10] [BVH] Null pointer provided for \"b\" in call to function \"%s\"\n",__FUNCSIG__);
			#endif
			return 0;
		}
	}
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
	size_t       entitiesInScene = 0,                // How many entities are in the scene?
	             i               = 0,                // Iterators      ...
		         j               = 0,                //                ...
		         bI              = 0,                // Best indicies  ...
		         bJ              = 0;                //                ...
	GXBV_t      *ret             = 0;
	GXEntity_t  *e               = scene->entities;  // The list of entities

	// Count up all the entities in the scene
	while(e)
	{
		entitiesInScene++;
		e = e->next;
	}

	// Allocate a double pointer list
	BVList = calloc(entitiesInScene + 1, sizeof(void*));

	// Error checking 
	{
		#ifndef NDEBUG
			if(BVList == (void *)0)
				goto no_mem;
		#endif
	}

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
	
	// Iterate through the list until there are no more bounding volumes to combine
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
				// Make sure that i and j are not the same. If they are, set the test distance to float max so it will always fail
				float pD = (BVList[i] != BVList[j]) ? distance(BVList[i], BVList[j]) : FLT_MAX;

				// If there is a better distance, record the distance and the indicies to the bounding volume list
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

	ret = (BVList) ? *BVList : 0;

	free(BVList);

	// Don't want to dereference a null pointer
	return ret;

	// Error handling
	{

		// Standard library errors
		{
			no_mem:
			#ifndef NDEBUG
				g_print_error("[Standard Library] Failed to allocate memory in call to function \"%s\"\n", __FUNCSIG__);
			#endif

			return 0;

		}

		// Argument errors
		{
			noScene:
			#ifndef NDEBUG
				g_print_error("[G10] [BVH] Null pointer provided for \"scene\" in call to function %s\n", __FUNCSIG__);
			#endif

			return 0;
		}
	}
}

GXBV_t* find_closest_bv ( GXBV_t *bvh, GXBV_t* bv ) 
{

	// Argument check
	{
		#ifndef NDEBUG
			if(bvh == (void*)0)
				goto no_bvh;
			if(bv == (void*)0)
				goto no_bv;
		#endif
	}

	GXBV_t* ret = 0;

	if (checkIntersection(bvh, bv))
	{
		if (bvh->left == 0 && bvh->right == 0)
			return bvh;
		if (distance(bvh->left, bv) < distance(bv->right, bv))
		{
			ret = find_closest_bv(bvh->left, bv);
			if (ret)
				return bvh->left;
		}
		else
		{
			ret = find_closest_bv(bvh->right, bv);
			if (ret)
				return bvh->right;
		}

	}

	return ret;

	// Error handling
	{
	
		// Argument errors
		{
			no_bvh:
			#ifndef NDEBUG
				g_print_error("[G10] [BVH] Null pointer provided for \"bvh\" in call to function \"%s\"\n", __FUNCSIG__);
			#endif
			return 0;
			
			no_bv:
			#ifndef NDEBUG
				g_print_error("[G10] [BVH] Null pointer provided for \"bv\" in call to function \"%s\"\n", __FUNCSIG__);
			#endif
			return 0;
		}
	}
}

GXBV_t* find_parent_bv ( GXBV_t *bvh, GXBV_t *bv )
{
	// Argument check
	{
		if (bv == 0)
			return 0;
		if (bv == bvh)
			return 0;
	}

	GXBV_t* ret = 0;

	if (checkIntersection(bvh, bv))
	{
		if (bvh->left == bv)
			return bvh;

		if (bvh->right == bv)
			return bvh;

		if (bvh->entity)
			return bvh;

		if (distance(bvh->left, bvh) < distance(bv->right, bvh))
		{
			ret = find_parent_bv(bvh->left, bvh);
			if (ret)
				return ret;
		}
		else
		{
			ret = find_parent_bv(bvh->right, bv);
			if (ret)
				return ret;
		}
		

	}

	return ret;

}

int resize_bv(GXBV_t* bv)
{
	// Argument check
	{
		if (bv == 0)
			goto no_bv;

	}

	GXBV_t *a = bv->left,
	       *b = bv->right;
	GXInstance_t *instance = g_get_active_instance();
	if (bv->entity)
		goto no_bv_entity;
	{
		vec3 maxA = *b->dimensions,
			 maxB = *b->dimensions,
			 minA = mul_vec3_f(*a->dimensions, -1),
			 minB = mul_vec3_f(*b->dimensions, -1),
			 maxBound,
			 minBound;


		if (a->entity)
		{
			mul_vec3_vec3(&maxA, a->entity->transform->scale, maxB);
			mul_vec3_vec3(&minA, a->entity->transform->scale, minB);
		}

		if (b->entity)
		{
			mul_vec3_vec3(&minB, b->entity->transform->scale, minB);
			mul_vec3_vec3(&maxB, b->entity->transform->scale, maxB);
		}

		add_vec3(&maxA, *a->location, maxA);
		sub_vec3(&minA, *a->location, minA);
		add_vec3(&maxB, *b->location, maxB);
		sub_vec3(&minB, *b->location, minB);

		maxBound.x = ((max(max(maxA.x, minA.x), max(maxB.x, minB.x)))),
		maxBound.y = ((max(max(maxA.y, minA.y), max(maxB.y, minB.y)))),
		maxBound.z = ((max(max(maxA.z, minA.z), max(maxB.z, minB.z))));

		minBound.x = ((min(min(maxA.x, minA.x), min(maxB.x, minB.x)))),
		minBound.y = ((min(min(maxA.y, minA.y), min(maxB.y, minB.y)))),
		minBound.z = ((min(min(maxA.z, minA.z), min(maxB.z, minB.z))));

		bv->location->x = fabsf(maxBound.x - minBound.x) / 2,
		bv->location->y = fabsf(maxBound.y - minBound.y) / 2,
		bv->location->z = fabsf(maxBound.z - minBound.z) / 2;
		
		bv->dimensions->x = maxBound.x - bv->location->x,
		bv->dimensions->y = maxBound.y - bv->location->y,
		bv->dimensions->z = maxBound.z - bv->location->z;
		no_bv_entity:;
	}
	return 0;

	// Error handling
	{
		no_bv:
		#ifndef NDEBUG
			g_print_error("[G10] [BVH] Null pointer provided for \"bv\" in call to function \"%s\"\n", __FUNCSIG__);
		#endif
		return 0;
	}
}

int draw_scene_bv ( GXScene_t *scene, GXPart_t *cube, GXShader_t *shader, int depth )
{
	// Argument check
	{
		if (scene == (void*)0)
			goto no_scene;
		if (cube == (void*)0)
			goto no_cube;
		if (shader == (void*)0)
			goto no_shader;
	}

	// Draw the bounding volumes in wireframe mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	draw_bv(scene->bvh, cube, shader, scene->cameras, depth);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	return 0;

	// Error handling
	{
		// Argument errors
		{
			
			no_scene:
				#ifndef NDEBUG
			        g_print_error("[G10] [BVH] Null pointer provided for \"scene\" in call to function \"%s\"\n", __FUNCSIG__);
				#endif
				return 0;

			no_cube:
				#ifndef NDEBUG
					g_print_error("[G10] [BVH] Null pointer provided for \"cube\" in call to function \"%s\"\n", __FUNCSIG__);
				#endif
				return 0;

			no_shader:
				#ifndef NDEBUG
					g_print_error("[G10] [BVH] Null pointer provided for \"shader\" in call to function \"%s\"\n", __FUNCSIG__);
				#endif
				return 0;
		}
	}
}

int draw_bv ( GXBV_t *bv, GXPart_t *cube, GXShader_t *shader, GXCamera_t *camera, int depth )
{
	// Argument check
	{
		// TODO
	}

	if (depth == 0)
		return 0;

	// Initialized data
	const vec3 colors[8] = {
					(vec3) { 1.f   , 0.f   , 0.f },    // 0 - Red
					(vec3) { 1.f   , 0.214f, 0.f },    // 1 - Orange
					(vec3) { 1.f   , 1.f   , 0.0f },   // 2 - Yellow
					(vec3) { 0.f   , 1.f   , 0.0f },   // 3 - Green
					(vec3) { 0.f   , 1.f   , 1.f },    // 4 - Cyan
					(vec3) { 0.f   , 0.f   , 1.f },    // 5 - Blue
					(vec3) { 0.214f, 0.f   , 1.f },    // 6 - Purple
					(vec3) { 1.f   , 0.f   , 0.214f }, // 7 - Pink
				   };

	// Use the provided shader, set the camera, model matrix, and color
	use_shader(shader);
	set_shader_camera(shader, camera);
	if (bv->entity == (void*)0)
	{
		mat4 r = model_mat4_from_vec3(*bv->location, euler_angle_from_quaternion((quaternion) { 1.f, 0.f, 0.f, 1.f }), *bv->dimensions);
		
		set_shader_mat4(shader, "M", &r);

	}
	else
	{
		mat4 r = model_mat4_from_vec3(*bv->location, euler_angle_from_quaternion((quaternion) { 1.f, 0.f, 0.f, 1.f }), *bv->dimensions);

		set_shader_mat4(shader, "M", &r);
	}
	// The color is set from accessing the list of colors at the depth modulo 8, so the colors will cycle. 
	// The intuition is that as hue increases, the bounding volumes are deeper, and as the hue decreases, the bounding volumes are larger
	set_shader_vec3(shader, "color", colors[depth % 8]);

	// Draw the cube
	draw_part(cube);

	// Draw the left box if there is one
	if (bv->left)
		draw_bv(bv->left, cube, shader, camera, depth - 1);

	// Draw the right box if there is one
	if (bv->right)
		draw_bv(bv->right, cube, shader, camera, depth - 1);

	return 0;
	// TODO: Error handling

}

int print_bv ( FILE *f, GXBV_t* bv, size_t d )
{

	// Argument check
	{
		if (bv == (void*)0)
			goto no_bv;
	}

	// Base case, print out a header
	if (d == 0)
		fprintf(f, "[G10] [BVH] Bounding volume heierarchy\n[ entity name ] - < location > - < dimension >\n\n");
		
	// Indent proportional to the deapth of the BVH
	for (size_t i = 0; i < d * 4; i++)
		putc(' ', f);

	// If the bounding volume is an entity, print the entities name
	if (bv->entity)
	{
		fprintf(f, "\"%s\"", bv->entity->name); 
		fprintf(f, "%s - < %.2f %.2f %.2f > - < %.2f %.2f %.2f >\n", (bv->entity) ? "" : "Volume", bv->location->x, bv->location->y, bv->location->z, bv->dimensions->x * bv->entity->transform->scale.x, bv->dimensions->y * bv->entity->transform->scale.y, bv->dimensions->z * bv->entity->transform->scale.z);
	}
	else

		// Print the location and scale of the bounding volume
		fprintf(f, "%s - < %.2f %.2f %.2f > - < %.2f %.2f %.2f >\n", (bv->entity) ? "" : "Volume", bv->location->x, bv->location->y, bv->location->z, bv->dimensions->x, bv->dimensions->y, bv->dimensions->z);

	// Print the left node
	if (bv->left)
		print_bv(f, bv->left, d+1);

	// Print the right node
	if (bv->right)
		print_bv(f, bv->right, d+1);
	
	// If there is nothing else to print, print a few newlines
	if (d == 0)
		fprintf(f,"\n\n");

	return 0;

	// Error handling
	{
		no_bv:
		#ifndef NDEBUG
			g_print_error("[G10] [BVH] Null pointer provided for \"bv\" in call to function \"%s\"\n", __FUNCSIG__);
		#endif
		return 0;
	}
}

size_t get_entities_from_bv ( GXBV_t* bv, GXEntity_t** entities, size_t count)
{
	// Argument check
	{
		if (entities == 0)
			goto countingMode;
		if (bv == 0)
			goto no_bv;
	}

	size_t i = count;

	if (bv->entity)
		entities[(i--) - 1] = bv->entity;

	if (bv->left)
		if (bv->left->entity)
			entities[(i--) - 1] = bv->left->entity;
		else
			i = get_entities_from_bv(bv->left, entities, i);

	if (bv->right)
		if (bv->right->entity)
			entities[(i--) - 1] = bv->right->entity;
		else
			i = get_entities_from_bv(bv->right, entities, i);



	return i;

	return 0;

	// Counting mode counts how many entities are present in a bounding volume, 
	// and returns the count so you can calloc() some ram for the entity pointers
	countingMode:
	{
		// Initialized data
		size_t ret = 0;

		// FIX
		if (bv)
		{
			if (bv->entity)
				return ret+1;

			// Check if our left is valid
			if (bv->left)
			{
				// Check if our left is an entity. If so, increment
				if (bv->left->entity)
					ret++;

				// Otherwise, parse the left volume
				else
					ret += get_entities_from_bv(bv->left, entities, count);
			}

			// Same as above, but for right
			if (bv->right)
			{
				if (bv->right->entity)
					ret++;
				else
					ret += get_entities_from_bv(bv->right, entities, count);
			}
		}
		return ret;
	}

	// Error handling
	{
		no_bv:
		#ifndef NDEBUG
			g_print_error("[G10] [BVH] Null poiner provided for \"bv\" in call to function \"%s\"\n", __FUNCSIG__);
		#endif
		return 0;
	}
}

bool checkIntersection(GXBV_t *a, GXBV_t *b)
{

	// Argument check
	{
		#ifndef NDEBUG
			if(a==(void*)0)
				goto no_a;
			if (b == (void*)0)
				goto no_b;
		#endif
	}

	return ((a->location->x - a->dimensions->x) <= (b->location->x + b->dimensions->x) && (a->location->x + a->dimensions->x) >= (b->location->x - b->dimensions->x) &&
            (a->location->y - a->dimensions->y) <= (b->location->y + b->dimensions->y) && (a->location->y + a->dimensions->y) >= (b->location->y - b->dimensions->y) &&
            (a->location->z - a->dimensions->z) <= (b->location->z + b->dimensions->z) && (a->location->z + a->dimensions->z) >= (b->location->z - b->dimensions->z));
	
	// Error handling
	{
		no_a:
		#ifndef NDEBUG
			g_print_error("[G10] [BVH] Null pointer provided for \"a\" in call to function \"%s\"\n", __FUNCSIG__);
		#endif
		return 0;
		no_b:
		#ifndef NDEBUG
			g_print_error("[G10] [BVH] Null pointer provided for \"b\" in call to function \"%s\"\n", __FUNCSIG__);
		#endif
		return 0;
	}
}

int destroy_bv(GXBV_t* bv)
{

	// Argument check
	{
		if (bv == (void*)0)
			goto no_bv;
	}

	if (bv->entity == (void*)0)
	{
		free(bv->location);
		free(bv->dimensions);
	}

	if(bv->left)
		destroy_bv(bv->left);

	if(bv->right)
		destroy_bv(bv->right);
	
	free(bv);

	return 0;

	// Error handling
	{
		// Argument errors
		{
			no_bv:
			#ifndef NDEBUG
				g_print_error("[G10] [BVH] Null pointer provided for \"bv\" in call to function \"%s\"\n",__FUNCSIG__);
			#endif
			return 0;
		}
	}
}
