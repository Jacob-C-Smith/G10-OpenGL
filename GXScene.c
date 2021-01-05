#include <G10/GXScene.h>

GXscene_t* createScene()
{
	GXscene_t* ret = malloc(sizeof(GXscene_t));

	if (ret == 0)
		return ret;
	
	// Set head to 0
	ret->head = 0;

	return ret;
}

GXscene_t* loadScene( const char path[] )
{
	// Uninitialized data
	int i;
	char* data;

	// Initialized data
	GXscene_t* ret = createScene();
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

		// Create a camera
		if (strcmp("camera", rootContents[j].name) == 0)
		{
			// Initialized data
			GXvec3_t where       = { 0,0,0 };
			GXvec3_t target      = { 0,0,0 };
			GXvec3_t up          = { 0,0,0 };
			float    fov         = 0.f;
			float    near        = 0.f;
			float    far         = 0.f;
			float    aspectRatio = 0.f;

			// Parse out JSON camera data
			size_t len = strlen(rootContents[j].content.nWhere), subTokenCount = GXParseJSON(rootContents[j].content.nWhere, len, 0, 0);
			JSONValue_t* subContents = malloc(sizeof(JSONValue_t) * subTokenCount);
			GXParseJSON(rootContents[j].content.nWhere, len, subTokenCount, subContents);
			
			for (size_t k = 0; k < subTokenCount; k++)

				// Parse out where the camera is
				if (strcmp("where", subContents[k].name) == 0)
					where = (GXvec3_t){ atof(subContents[k].content.aWhere[0]), atof(subContents[k].content.aWhere[1]), atof(subContents[k].content.aWhere[2]) };

				// Parse out target
				else if (strcmp("target", subContents[k].name) == 0)
					target = (GXvec3_t){ atof(subContents[k].content.aWhere[0]), atof(subContents[k].content.aWhere[1]), atof(subContents[k].content.aWhere[2]) };

				// Parse out up
			    else if (strcmp("up", subContents[k].name) == 0)
					up = (GXvec3_t){ atof(subContents[k].content.aWhere[0]), atof(subContents[k].content.aWhere[1]), atof(subContents[k].content.aWhere[2]) };

				// Parse out FOV
			    else if (strcmp("fov", subContents[k].name) == 0)
					fov = atof(subContents[k].content.nWhere);

				// Parse out near clipping plane
				else if (strcmp("near", subContents[k].name) == 0)
					near = atof(subContents[k].content.nWhere);

				// Parse out far clipping plane
				else if (strcmp("far", subContents[k].name) == 0)
					far = atof(subContents[k].content.nWhere);

				// Parse out aspect ratio
				else if (strcmp("aspectRatio", subContents[k].name) == 0)
					aspectRatio = atof(subContents[k].content.nWhere);

			// If no aspect ratio is supplied, default to 16:9 aspect ratio if not set
			if (aspectRatio == 0.f)
				aspectRatio = 16.f/9.f;

			// Create the camera
			ret->camera = createCamera(where, target, up, fov, near, far, aspectRatio);

			// Free subcontents
			free(subContents);
		}

		// Load up entities
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

int appendEntity(GXscene_t* scene, GXEntity_t* entity)
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

int drawScene(GXscene_t* scene)
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
			setShaderVec3(i->shader, "lightPositions[0]", (GXvec3_t) { 2.f, 0.f, 0.f } );
			setShaderVec3(i->shader, "lightColors[0]"   , (GXvec3_t) { 200.f, 200.f, 200.f } );
			setShaderVec3(i->shader, "lightPositions[1]", (GXvec3_t) { -2.f, 0.f, 0.f } );
			setShaderVec3(i->shader, "lightColors[1]"   , (GXvec3_t) { 200.f, 200.f, 200.f } );
			setShaderVec3(i->shader, "lightPositions[2]", (GXvec3_t) { 0.f, 2.f, 0.f } );
			setShaderVec3(i->shader, "lightColors[2]"   , (GXvec3_t) { 200.f, 200.f, 200.f } );
			setShaderVec3(i->shader, "lightPositions[3]", (GXvec3_t) { 0.f, -2.f, 0.f } );
			setShaderVec3(i->shader, "lightColors[3]"   , (GXvec3_t) { 200.f, 200.f, 200.f } );
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

GXEntity_t* getEntity(GXscene_t* scene, const char name[])
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

int removeEntity(GXscene_t* scene, const char name[])
{
	// Create a pointer to the head of the list
	GXEntity_t* i = scene->head;
	
	// Quick sanity check
	if (i == 0)
		return -1;
	
	// Destroy the named entity
	while (i->next)
	{
		if (strcmp(name, i->next->name) == 0) 
		{
			// TODO: Handle edge cases, namely when
			GXEntity_t* j = i->next->next;
			destroyEntity(i->next);
			#ifdef GXDEBUGMODE & GXDEBUGVERBOSE
				printf("Destroyed entity %s", name);
			#endif
			i->next = j;
			return 0;
		}
		i = i->next;
	}

	return 0;
}

int destroyScene(GXscene_t* scene)
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