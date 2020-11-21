#include <G10/GXScene.h>

GXscene_t* createScene()
{
	// Allocate space for a scene
	GXscene_t* ret = malloc(sizeof(GXscene_t));

	if (ret == 0)
		return (void*)0;
	
	// Set head to 0
	ret->head = 0;

	return ret;
}

// TODO: Fully document function
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

	int rootTokenCount = GXParseJSON(data, l, 0, (void*)0);
	JSONValue_t* rootContents = malloc(sizeof(JSONValue_t) * rootTokenCount);
	GXParseJSON(data, l, rootTokenCount, rootContents);

	for (size_t j = 0; j < rootTokenCount; j++)
	{
		if (strcmp("comment", rootContents[j].name) == 0)
		#ifdef debugmode
			// Print out comment
			printf("comment in file \"%s\" - \"%s\"\n\n", path, (char*)rootContents[j].content.nWhere);
		#endif
		if (strcmp("camera", rootContents[j].name) == 0)
		{
			GXvec3_t where       = { 0,0,0 };
			GXvec3_t target      = { 0,0,0 };
			GXvec3_t up          = { 0,0,0 };
			float    fov         = 0.f;
			float    near        = 0.f;
			float    far         = 0.f;
			float    aspectRatio = 0.f;           

			char* relativePath = 0;

			size_t len = strlen(rootContents[j].content.nWhere), subTokenCount = GXParseJSON(rootContents[j].content.nWhere, len, 0, 0);
			JSONValue_t* subContents = malloc(sizeof(JSONValue_t) * subTokenCount);
			GXParseJSON(rootContents[j].content.nWhere, len, subTokenCount, subContents);
			
			for (size_t k = 0; k < subTokenCount; k++)
				if (strcmp("where", subContents[k].name) == 0)
					where = (GXvec3_t){ atof(subContents[k].content.aWhere[0]), atof(subContents[k].content.aWhere[1]), atof(subContents[k].content.aWhere[2]) };
			
			for (size_t k = 0; k < subTokenCount; k++)
				if (strcmp("target", subContents[k].name) == 0)
					target = (GXvec3_t){ atof(subContents[k].content.aWhere[0]), atof(subContents[k].content.aWhere[1]), atof(subContents[k].content.aWhere[2]) };
			
			for (size_t k = 0; k < subTokenCount; k++)
				if (strcmp("up", subContents[k].name) == 0)
					up = (GXvec3_t){ atof(subContents[k].content.aWhere[0]), atof(subContents[k].content.aWhere[1]), atof(subContents[k].content.aWhere[2]) };
			
			for (size_t k = 0; k < subTokenCount; k++)
				if (strcmp("fov", subContents[k].name) == 0)
					fov = atof(subContents[k].content.nWhere);

			for (size_t k = 0; k < subTokenCount; k++)
				if (strcmp("near", subContents[k].name) == 0)
					near = atof(subContents[k].content.nWhere);

			for (size_t k = 0; k < subTokenCount; k++)
				if (strcmp("far", subContents[k].name) == 0)
					far = atof(subContents[k].content.nWhere);

			for (size_t k = 0; k < subTokenCount; k++)
				if (strcmp("aspectRatio", subContents[k].name) == 0)
					aspectRatio = atof(subContents[k].content.nWhere);

			if (aspectRatio == 0.f)
				aspectRatio = 16.f/9.f; // Default to 16:9 aspect ratio if not set

			ret->camera = createCamera(where, target, up, fov, near, far, aspectRatio);
			computeProjectionMatrix(ret->camera);

			free(subContents);
		}
		if (strcmp("entities", rootContents[j].name) == 0)
		{
			char** paths = 0;
			size_t count = 0;

			size_t len = strlen(rootContents[j].content.nWhere), subTokenCount = GXParseJSON(rootContents[j].content.nWhere, len, 0, 0);
			JSONValue_t* subContents = malloc(sizeof(JSONValue_t) * rootTokenCount);
			GXParseJSON(rootContents[j].content.nWhere, len, subTokenCount, subContents);

			for (size_t k = 0; k < subTokenCount; k++)
				if (strcmp("entities", subContents[k].name) == 0)
					paths = subContents[k].content.aWhere;

			for (size_t k = 0; k < subTokenCount; k++)
				if (strcmp("entityCount", subContents[k].name) == 0)
					count = atoi(subContents[k].content.nWhere);

			if(paths)
				for (size_t k = 0; k < count; k++)
					appendEntity(ret,loadEntity(paths[k]));

			free(subContents);
		}
	}
	// Finish up
	free(rootContents);

	return ret;
}

int appendEntity(GXscene_t* scene, GXentity_t* entity)
{
	// Create a pointer to the head of the list
	GXentity_t* i = scene->head;

	// Check if the head is null
	if (i == 0)
	{
		scene->head = entity;
		return 0;
	}

	// Search for the end of the LL
	while (i->next)
		i = i->next;

	// Assign next as entity
	i->next = entity;

	return 0;
}

int drawScene(GXscene_t* scene)
{
	// Create a pointer to the head of the list
	GXentity_t* i = scene->head;

	// Is the scene real?
	if (i == 0)
		return -1;
	
	// Iterate through list until we hit nullptr
	while (i)
	{
		// Check for a shader
		if (i->flags & GXE_rShader)
		{
			// Use it
			useShader(i->shader);
	
			// Compute a new view matrix because it has almost certainly changed
			computeViewMatrix(scene->camera);
			
			// Set some uniforms for the shader
			glUniformMatrix4fv(glGetUniformLocation(i->shader->shaderProgramID, "V"), 1, GL_FALSE, (const GLfloat*)&scene->camera->view);
			glUniformMatrix4fv(glGetUniformLocation(i->shader->shaderProgramID, "P"), 1, GL_FALSE, (const GLfloat*)&scene->camera->projection);
		}
		
		// Actually draw the entity
		drawEntity(i);
		
		// Rinse and repeat
		i = i->next;
	}

	return 0;
}

GXentity_t* getEntity(GXscene_t* scene, const char name[])
{
	// Create a pointer to the head of the list
	GXentity_t* i = scene->head;

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
	GXentity_t* i = scene->head;
	
	// Quick sanity check
	if (i == 0)
		return -1;
	
	// Destroy the named entity
	while (i->next)
	{
		if (strcmp(name, i->next->name) == 0) 
		{
			// TODO: Handle edge cases
			GXentity_t* j = i->next->next;
			destroyEntity(i->next);
			#ifdef debugmode
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
	GXentity_t* i = scene->head;
	
	// Destroy all assets in the scene
	while (i)
	{
		GXentity_t* j = i;
		i = i->next;
		destroyEntity(j);
	}
	//TODO: Destroy Camera, free scene
	return 0;
}