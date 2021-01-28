#include <G10/GXScene.h>

GXScene_t* createScene ( )
{
	GXScene_t* ret = malloc(sizeof(GXScene_t));

	if (ret == 0)
		return ret;
	
	// Populate with 0's
	ret->head   = 0;
	ret->camera = 0;
	return ret;
}

GXScene_t* loadScene ( const char path[] )
{
	// Uninitialized data
	int i;
	char* data;

	// Initialized data
	GXScene_t* ret = createScene();
	GXsize_t l = 0;
	if (ret == 0)
		return (void*)0;

	// Open file for reading
	FILE* f = fopen(path, "r");

	// Check if file is valid
	if (f == NULL)
	{
		printf("Failed to load file %s\n", path);
		return (void*)0;
	}

	// Find file size and prep for read
	fseek(f, 0, SEEK_END);
	i = ftell(f);
	fseek(f, 0, SEEK_SET);

	// Allocate data and read file into memory
	data = malloc(i);
	if (data == 0)
		return (void*)0;
	fread(data, 1, i, f);

	// We no longer need the file
	fclose(f);

	l = strlen(data);

	// Parse the first level of the JSON entity
	int rootTokenCount = GXParseJSON(data, l, 0, (void*)0);
	JSONValue_t* rootContents = malloc(sizeof(JSONValue_t) * rootTokenCount);
	GXParseJSON(data, l, rootTokenCount, rootContents);

	for (size_t j = 0; j < rootTokenCount; j++)
	{
		// Handle comments
		if (strcmp("comment", rootContents[j].name) == 0)
			#ifdef GXDEBUGMODE
				// Print out comment
				printf("comment in file \"%s\" - \"%s\"\n\n", path, (char*)rootContents[j].content.nWhere);
			#endif

		// Create a cameras
		if (strcmp("cameras", rootContents[j].name) == 0)
		{
			size_t k = 0;
			while (rootContents[j].content.aWhere[k])
			{
				// Create the camera
				GXCamera_t* camera = createCamera();

				// Parse out JSON camera data
				size_t len = strlen(rootContents[j].content.aWhere[k]), subTokenCount = GXParseJSON(rootContents[j].content.aWhere[k], len, 0, 0);
				JSONValue_t* subContents = malloc(sizeof(JSONValue_t) * subTokenCount);
				GXParseJSON(rootContents[j].content.aWhere[k], len, subTokenCount, subContents);

				for (size_t l = 0; l < subTokenCount; l++)
				{
					// Parse out the camera name
					if (strcmp("name", subContents[l].name) == 0)
					{
						camera->name = malloc(strlen(subContents[l].content.nWhere));
						strcpy(camera->name, subContents[l].content.nWhere);
						continue;
					}

					// Parse out where the camera is
					if (strcmp("where", subContents[l].name) == 0)
					{
						camera->where = (GXvec3_t){ atof(subContents[l].content.aWhere[0]), atof(subContents[l].content.aWhere[1]), atof(subContents[l].content.aWhere[2]) };
						continue;
					}
					// Parse out target
					else if (strcmp("target", subContents[l].name) == 0)
					{
						camera->target = (GXvec3_t){ atof(subContents[l].content.aWhere[0]), atof(subContents[l].content.aWhere[1]), atof(subContents[l].content.aWhere[2]) };
						continue;
					}
					// Parse out up
					else if (strcmp("up", subContents[l].name) == 0)
					{
						camera->up = (GXvec3_t){ atof(subContents[l].content.aWhere[0]), atof(subContents[l].content.aWhere[1]), atof(subContents[l].content.aWhere[2]) };
						continue;
					}
					// Parse out FOV
					else if (strcmp("fov", subContents[l].name) == 0)
					{
						camera->fov = atof(subContents[l].content.nWhere);
						continue;
					}
					// Parse out near clipping plane
					else if (strcmp("near", subContents[l].name) == 0)
					{
						camera->near = atof(subContents[l].content.nWhere);
						continue;
					}
					// Parse out far clipping plane
					else if (strcmp("far", subContents[l].name) == 0)
					{
						camera->far = atof(subContents[l].content.nWhere);
						continue;
					}
					// Parse out aspect ratio
					else if (strcmp("aspectRatio", subContents[l].name) == 0)
					{
						camera->aspectRatio = atof(subContents[l].content.nWhere);
						continue;
					}
				}
				// If no aspect ratio is supplied, default to 16:9 aspect ratio if not set
				if (camera->aspectRatio == 0.f)
					camera->aspectRatio = 16.f / 9.f;

				// Calculate perspective projection
				camera->projection = perspective(camera->fov, camera->aspectRatio, camera->near, camera->far);
				camera->view = lookAt(camera->where, camera->target, camera->up);

				// Set next to zero
				camera->next = (void*)0;

				appendCamera(ret, camera);
				// Free subcontents
				free(subContents);
				k++;
			}
			
		}

		// Spin up entities
		if (strcmp("entities", rootContents[j].name) == 0)
		{
			size_t k = 0;
			while (rootContents[j].content.aWhere[k])
			{
				#if GXDEBUGMODE & GXDEBUGCOMMENTS
					printf("Loading \"%s\"\n",rootContents[j].content.aWhere[k]);
				#endif	
				appendEntity(ret, loadEntity(rootContents[j].content.aWhere[k]));
				k++;
			}
		}
	}

	// Finish up
	free(rootContents);

	return ret;
}

int appendEntity ( GXScene_t* scene, GXEntity_t* entity )
{
	// Set the pointer to the head of the linked list
	GXEntity_t* i = scene->head;

	// Check if the head is null. If so, set the head to the entity
	if (i == 0)
	{
		scene->head = entity;
		return 0;
	}

	// Search for the end of the linked list
	while (i->next)
		i = i->next;

	// Assign next as entity
	i->next = entity;

	return 0;
}

int appendCamera ( GXScene_t* scene, GXCamera_t* camera ) 
{
	// Set the pointer to the head of the linked list
	GXCamera_t* i = scene->camera;

	// Check if the head is null. If so, set the head to the entity
	if (i == 0)
	{
		scene->camera = camera;
		return 0;
	}

	// Search for the end of the linked list
	while (i->next)
		i = i->next;

	// Assign next as entity
	i->next = camera;

	return 0;
}

int drawScene ( GXScene_t* scene )
{
	// Create a pointer to the head of the list
	GXEntity_t* i = scene->head;

	// Is the scene real?
	if (i == 0)
		return -1;
	
	// Iterate through list until we hit nullptr
	while (i)
	{
		// Check for a shader
		if (i->flags & GXEFShader)
		{
			// Use it
			useShader(i->shader);

			// Set some uniforms for the shader
			setShaderMat4(i->shader, "V", (const GLfloat*)&scene->camera->view);
			setShaderMat4(i->shader, "P", (const GLfloat*)&scene->camera->projection);

			setShaderVec3(i->shader, "camPos", scene->camera->where);
		}

		// If a PBR material is available, set up lights to use it
		if (i->flags & GXEFMaterial)
		{
			// TODO: Implement a system for linearly assigning lights
			setShaderVec3(i->shader, "lightPositions[0]", (GXvec3_t) { 4.f, 0.f, 5.f } );
			setShaderVec3(i->shader, "lightColors[0]"   , (GXvec3_t) { 1000.f, 1000.f, 1000.f } );
			setShaderVec3(i->shader, "lightPositions[1]", (GXvec3_t) { -4.f, 0.f, 5.f } );
			setShaderVec3(i->shader, "lightColors[1]"   , (GXvec3_t) { 1000.f, 1000.f, 1000.f } );
			setShaderVec3(i->shader, "lightPositions[2]", (GXvec3_t) { 0.f, 4.f, 5.f } );
			setShaderVec3(i->shader, "lightColors[2]"   , (GXvec3_t) { 1000.f, 1000.f, 1000.f } );
			setShaderVec3(i->shader, "lightPositions[3]", (GXvec3_t) { 0.f, -4.f, 5.f } );
			setShaderVec3(i->shader, "lightColors[3]"   , (GXvec3_t) { 1000.f, 1000.f, 1000.f } );
		}

		// Assign a material
		assignMaterial(i->material, i->shader);

		// Actually draw the entity
		drawEntity(i);
		
		// Rinse and repeat
		i = i->next;
	}

	return 0;
}

GXEntity_t* getEntity ( GXScene_t* scene, const char name[] )
{
	// Create a pointer to the head of the list
	GXEntity_t* i = scene->head;

	// Sanity check
	if (i == 0)
		return (void*)0;
	
	// Iterate through list until we hit the entity we want, or zero
	while (i)
	{
		if (strcmp(name, i->name) == 0)
			return i; // If able to locate the entity in question, return a pointer
		i = i->next;
	}
	
	// Unable to locate entity
	return (void*) 0;
}

int setActiveCamera ( GXScene_t* scene, const char name[] )
{
	// Create a pointer to the head of the list 
	GXCamera_t* i = scene->camera;
	
	// Quick sanity check
	if (i == 0)
		return -1;

	// Check the head
	if (strcmp(name, i->name) == 0)
		return 0;

	// Find the named camera
	while (i->next)
	{
		if (strcmp(name, i->next->name) == 0)
		{
			GXCamera_t* j = i->next;
			GXCamera_t* k = i->next->next;
	
			// Stitch up the linked list 
			i->next = k;
			j->next = scene->camera;
			scene->camera = j;
			return 0;
		}
		i = i->next;
	}
	return 0;
}

int removeEntity ( GXScene_t* scene, const char name[] )
{
	// Create a pointer to the head of the list
	GXEntity_t* i = scene->head;
	
	// Quick sanity check
	if (i == 0)
		return -1;
	
	// Check the head
	if (strcmp(name, i->name) == 0)
	{
		GXEntity_t* j = i->next;
		destroyEntity(i);
		i = j;
		return 0;
	}

	// Destroy the named entity
	while (i->next)
	{
		if (strcmp(name, i->next->name) == 0) 
		{
			// Make a copy of the entity that is 2 positions ahead of the current entity
			GXEntity_t* j = i->next->next;
			
			// Delete the entity
			destroyEntity(i->next);

			// Verbose logging
			#ifdef GXDEBUGMODE & GXDEBUGVERBOSE
				printf("Destroyed Entity \"%s\"\n", name);
			#endif
			
			// Stitch up the linked list 
			i->next = j;
			return 0;
		}
		i = i->next;
	}

	return 0;
}

int removeCamera ( GXScene_t* scene, const char name[] )
{
	// Create a pointer to the head of the list
	GXCamera_t* i = scene->camera;

	// Quick sanity check
	if (i == 0)
		return -1;

	// Check the head
	if (strcmp(name, i->name) == 0)
	{
		GXEntity_t* j = i->next;
		destroyEntity(i);
		i = j;
		return 0;
	}

	// Destroy the named entity
	while (i->next)
	{
		if (strcmp(name, i->next->name) == 0)
		{
			// Make a copy of the entity that is 2 positions ahead of the current entity
			GXCamera_t* j = i->next->next;

			// Delete the entity
			destroyCamera(i->next);

			// Verbose logging
			#ifdef GXDEBUGMODE & GXDEBUGVERBOSE
				printf("Destroyed Camera \"%s\"\n", name);
			#endif		

			// Stitch up the linked list 
			i->next = j;
			return 0;
		}
		i = i->next;
	}

	return 0;
}

int destroyScene ( GXScene_t* scene )
{
	// Set sceneID to zero;
	GXEntity_t* i = scene->head;
	
	// Destroy all entities in the scene
	while (i)
	{
		GXEntity_t* j = i;
		i = i->next;
		destroyEntity(j);
	}

	// Zero set the head of the linked list
	scene->head   = (void*)0;

	// Destroy the camera
	scene->camera = destroyCamera(scene->camera);

	// Free up the scene
	free(scene);

	return 0;
}