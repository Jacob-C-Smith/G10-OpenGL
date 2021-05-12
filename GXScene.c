#include <G10/GXScene.h>

GXScene_t* createScene ( )
{
	GXScene_t* ret = malloc(sizeof(GXScene_t));

	if (ret == 0)
		return ret;
	
	// Populate with 0's
	ret->entities   = 0;
	ret->cameras    = 0;
	ret->lights     = 0;
	ret->name       = 0;
	return ret;
}

GXScene_t* loadScene ( const char path[] )
{
	// Uninitialized data
	int i;
	char* data;

	// Initialized data
	GXScene_t* ret = createScene();
	size_t l = 0;
	if (ret == 0)
		return (void*)0;

	// Open file for reading
	FILE* f = fopen(path, "rb");

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
	int rootTokenCount = GXParseJSON(data, i, 0, (void*)0);
	JSONValue_t* rootContents = calloc(rootTokenCount, sizeof(JSONValue_t));
	GXParseJSON(data, i, rootTokenCount, rootContents);

	for (size_t j = 0; j < rootTokenCount; j++)
	{
		// Handle comments
		if (strcmp("comment", rootContents[j].name) == 0)
		{
			#ifndef NDEBUG
			// Print out comment
			printf("comment in file \"%s\" - \"%s\"\n\n", path, (char*)rootContents[j].content.nWhere);
			#endif
		}

		// Copy out the name of the scene
		if (strcmp("name", rootContents[j].name) == 0)
		{
			size_t len = strlen(rootContents[j].content.nWhere);
			ret->name = malloc(len);
			strncpy(ret->name, rootContents[j].content.nWhere, len);
		}

		// Create cameras
		if (strcmp("cameras", rootContents[j].name) == 0)
		{
			size_t k = 0;
			while (rootContents[j].content.aWhere[k])
			{
				// Create the camera
				GXCamera_t* camera = createCamera();

				// Parse out JSON camera data
				size_t len = strlen(rootContents[j].content.aWhere[k]), subTokenCount = GXParseJSON(rootContents[j].content.aWhere[k], len, 0, 0);
				JSONValue_t* subContents = calloc(subTokenCount, sizeof(JSONValue_t));
				GXParseJSON(rootContents[j].content.aWhere[k], len, subTokenCount, subContents);

				for (size_t l = 0; l < subTokenCount; l++)
				{
					// Parse out the camera name
					if (strcmp("name", subContents[l].name) == 0)
					{
						camera->name = malloc(strlen(subContents[l].content.nWhere));
						strcpy(camera->name, (const char*)subContents[l].content.nWhere);
						continue;
					}

					// Parse out where the camera is
					if (strcmp("where", subContents[l].name) == 0)
					{
						camera->where = (GXvec3_t){ (float)atof(subContents[l].content.aWhere[0]), (float)atof(subContents[l].content.aWhere[1]), (float)atof(subContents[l].content.aWhere[2]) };
						continue;
					}

					// Parse out target
					else if (strcmp("target", subContents[l].name) == 0)
					{
						camera->target = (GXvec3_t){ (float)atof(subContents[l].content.aWhere[0]), (float)atof(subContents[l].content.aWhere[1]), (float)atof(subContents[l].content.aWhere[2]) };
						continue;
					}

					// Parse out up
					else if (strcmp("up", subContents[l].name) == 0)
					{
						camera->up = (GXvec3_t){ (float) atof(subContents[l].content.aWhere[0]), (float)atof(subContents[l].content.aWhere[1]), (float)atof(subContents[l].content.aWhere[2]) };
						continue;
					}

					// Parse out FOV
					else if (strcmp("fov", subContents[l].name) == 0)
					{
						camera->fov = (float)atof(subContents[l].content.nWhere);
						continue;
					}

					// Parse out near clipping plane
					else if (strcmp("near", subContents[l].name) == 0)
					{
						camera->near = (float)atof(subContents[l].content.nWhere);
						continue;
					}

					// Parse out far clipping plane
					else if (strcmp("far", subContents[l].name) == 0)
					{
						camera->far = (float)atof(subContents[l].content.nWhere);
						continue;
					}

					// Parse out aspect ratio
					else if (strcmp("aspectRatio", subContents[l].name) == 0)
					{
						camera->aspectRatio = (float)atof(subContents[l].content.nWhere);
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

		// Create entities
		if (strcmp("entities", rootContents[j].name) == 0)
		{
			size_t k = 0;
			while (rootContents[j].content.aWhere[k])
			{
			#ifndef NDEBUG
				printf("Loading \"%s\"\n", (char*)rootContents[j].content.aWhere[k]);
			#endif	
				appendEntity(ret, loadEntity(rootContents[j].content.aWhere[k]));
				k++;
			}
		}

		// Set up lights
		if (strcmp("lights", rootContents[j].name) == 0)
		{
			size_t k = 0;
			while (rootContents[j].content.aWhere[k])
			{
				// Uninitialized data
				JSONValue_t* subContents;
				size_t       subTokenCount;

				// Initialized data
				GXLight_t* light = createLight();
				size_t     len   = strlen(rootContents[j].content.aWhere[k]);

				// Preparse the data
				len           = strlen(rootContents[j].content.aWhere[k]);
				subTokenCount = GXParseJSON(rootContents[j].content.aWhere[k], len, 0, 0);
				subContents   = calloc(subTokenCount, sizeof(JSONValue_t));

				// Parse out JSON light data
				GXParseJSON(rootContents[j].content.aWhere[k], len, subTokenCount, subContents);

				// Iterate through key / value pairs to find relevent information
				for (size_t l = 0; l < subTokenCount; l++)
				{
					// Parse out the light name
					if (strcmp("name", subContents[l].name) == 0)
					{
						light->name = malloc(strlen(subContents[l].content.nWhere));
						if (light->name == (void*)0)
							return 0;
						strcpy(light->name, (const char*)subContents[l].content.nWhere);
						continue;
					}

					// Parse out light color
					if (strcmp("color", subContents[l].name) == 0)
					{
						light->color = (GXvec3_t){ (float)atof(subContents[l].content.aWhere[0]), (float)atof(subContents[l].content.aWhere[1]), (float)atof(subContents[l].content.aWhere[2]) };
						continue;
					}

					// Parse out light position
					if (strcmp("position", subContents[l].name) == 0)
					{
						light->location = (GXvec3_t){ (float)atof(subContents[l].content.aWhere[0]), (float)atof(subContents[l].content.aWhere[1]), (float)atof(subContents[l].content.aWhere[2]) };
						continue;
					}
				}

				// Set next to zero
				light->next = (void*)0;
				appendLight(ret, light);

				// Free subcontents
				free(subContents);
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
	GXEntity_t* i = scene->entities;

	// Check if the head is null. If so, set the head to the entity
	if (i == 0)
	{
		scene->entities = entity;
		return 0;
	}

	// Search for the end of the linked list, and check every entity 
	while (i->next)
	{
		// Error checking
		#ifndef NDEBUG
		if (strcmp(i->name, entity->name) == 0)
			goto duplicateName;
		#endif	
		i = i->next;

	}

	// Assign next as entity
	i->next = entity;

	return 0;
	
	duplicateName:
	#ifndef NDEBUG
		printf("Entity \"%s\" can not be appended to \"%s\" because an entity with that name already exists\n", entity->name, scene->name);
	#endif
	return -1;
}

int appendCamera ( GXScene_t* scene, GXCamera_t* camera ) 
{
	// Set the pointer to the head of the linked list
	GXCamera_t* i = scene->cameras;

	// Check if the head is null. If so, set the head to the entity
	if (i == 0)
	{
		scene->cameras = camera;
		return 0;
	}

	// Search for the end of the linked list
	while (i->next)
	{
		// Error checking
		#ifndef NDEBUG
		if (strcmp(i->name, camera->name) == 0)
			goto duplicateName;
		#endif
		i = i->next;
	}

	// Assign next as entity
	i->next = camera;

	return 0;

	duplicateName:
	#ifndef NDEBUG
		printf("Camera \"%s\" can not be appended to \"%s\" because a camera with that name already exists\n", camera->name, scene->name);
	#endif
	return -1;
}

int appendLight(GXScene_t* scene, GXLight_t* light)
{
	// Set the pointer to the head of the linked list
	GXLight_t* i = scene->lights;

	// Check if the head is null. If so, set the head to the light
	if (i == 0)
	{
		scene->lights = light;
		return 0;
	}

	// Search for the end of the linked list
	while (i->next)
	{
		// Error checking
		#ifndef NDEBUG
		if (strcmp(i->name, light->name) == 0)
			goto duplicateName;
		#endif
		i = i->next;
	}
	// Assign next as light
	i->next = light;

	return 0;

	duplicateName:
	#ifndef NDEBUG
	printf("Light \"%s\" can not be appended to \"%s\" because a light with that name already exists\n", light->name, scene->name);
	#endif
	return -1;
}

int drawScene ( GXScene_t* scene )
{
	// Is the scene real?
	if (scene == 0)
		return -1;

	// Create a pointer to the head of the list
	GXEntity_t* i = scene->entities;
	
	// Iterate through list until we hit nullptr
	while (i)
	{
		// Check for a shader
		if (i->flags & GXEFShader)
		{
			// Use it
			useShader(i->shader);

			// Set some uniforms for the shader
			setShaderMat4(i->shader, "V", (const GLfloat*)&scene->cameras->view);
			setShaderMat4(i->shader, "P", (const GLfloat*)&scene->cameras->projection);

			setShaderVec3(i->shader, "camPos", scene->cameras->where);
		}

		// If a PBR material is available, set up lights to use it
		if (i->flags & GXEFMaterial && scene->lights)
		{
			GXLight_t* light = scene->lights;

			char* buffer = malloc(512);
			// TODO: Dynamically determine max lights from graphics settings and machine hardware
			for(size_t j = 0; j < GX_MAX_LIGHTS && light; j++)
			{
				// TODO: use sprintf to generate
				
				buffer[sprintf(buffer, "lightPositions[%d]\0", j) + 1] = 0;
				setShaderVec3(i->shader, buffer, light->location);
				buffer[sprintf(buffer, "lightColors[%d]\0", j) + 1] = 0;
				setShaderVec3(i->shader, buffer, light->color);
			
				light = light->next;
			}

			free(buffer);
			/*
			setShaderVec3(i->shader, "lightPositions[0]", (GXvec3_t) { 4.f, 0.f, 5.f } );
			setShaderVec3(i->shader, "lightColors[0]"   , (GXvec3_t) { 1000.f, 1000.f, 1000.f } );
			setShaderVec3(i->shader, "lightPositions[1]", (GXvec3_t) { -4.f, 0.f, 5.f } );
			setShaderVec3(i->shader, "lightColors[1]"   , (GXvec3_t) { 1000.f, 1000.f, 1000.f } );
			setShaderVec3(i->shader, "lightPositions[2]", (GXvec3_t) { 0.f, 4.f, 5.f } );
			setShaderVec3(i->shader, "lightColors[2]"   , (GXvec3_t) { 1000.f, 1000.f, 1000.f } );
			setShaderVec3(i->shader, "lightPositions[3]", (GXvec3_t) { 0.f, -4.f, 5.f } );
			setShaderVec3(i->shader, "lightColors[3]"   , (GXvec3_t) { 1000.f, 1000.f, 1000.f } );
			*/
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

int computePhysics ( GXScene_t* scene, float deltaTime )
{
	// TODO: Compute physics
	GXEntity_t* i = scene->entities;
	while (i)
	{
		// Summate forces 
		sumVecs(i->rigidbody->forcesCount, i->rigidbody->forces);

		// Calculate new acceleration, velocity, and position
		calculateDerivativesOfDisplacement(i, deltaTime);

		// TODO: Calculate angular acceleration, velocity, and rotation


		i = i->next;
	}
	return 0;
}

GXEntity_t* getEntity ( GXScene_t* scene, const char name[] )
{
	// Create a pointer to the head of the list
	GXEntity_t* i = scene->entities;

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

GXCamera_t* getCamera(GXScene_t* scene, const char name[])
{
	// Create a pointer to the head of the list
	GXCamera_t* i = scene->cameras;

	// Sanity check
	if (i == 0)
		return (void*)0;

	// Iterate through list until we hit the entity we want, or zero
	while (i)
	{
		if (strcmp(name, i->name) == 0)
			return i; // If able to locate the camera in question, return a pointer
		i = i->next;
	}

	// Unable to locate entity
	return (void*)0;
}

int setActiveCamera ( GXScene_t* scene, const char name[] )
{
	// Create a pointer to the head of the list 
	GXCamera_t* i = scene->cameras;
	
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
			j->next = scene->cameras;
			scene->cameras = j;
			return 0;
		}
		i = i->next;
	}
	return 0;
}

int removeEntity ( GXScene_t* scene, const char name[] )
{
	// Create a pointer to the head of the list
	GXEntity_t* i = scene->entities;
	
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
			#ifndef NDEBUG
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
	GXCamera_t* i = scene->cameras;

	// Quick sanity check
	if (i == 0)
		return -1;

	// Check the head
	if (strcmp(name, i->name) == 0)
	{
		GXCamera_t* j = i->next;
		destroyCamera(i);
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
			#ifdef NDEBUG
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

int removeLight(GXScene_t* scene, const char name[])
{
	return 0;
}

int destroyScene ( GXScene_t* scene )
{
	// Set sceneID to zero;
	GXEntity_t* i = scene->entities;
	GXCamera_t* k = scene->cameras;

	// Destroy all entities in the scene
	while (i)
	{
		GXEntity_t* j = i;
		i = i->next;
		destroyEntity(j);
	}

	// Zero set the head of the linked list
	scene->entities   = (void*)0;

	// Destroy the cameras
	while (k)
	{
		GXCamera_t* l = k;
		k = k->next;
		destroyCamera(l);
	}

	// Free up the scene
	free(scene);

	return 0;
}