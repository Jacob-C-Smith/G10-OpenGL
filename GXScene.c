#include <G10/GXScene.h>

GXscene_t* createScene()
{
	// Create a random ID and set head to zero
	GXscene_t* ret = malloc(sizeof(GXscene_t));
	if (ret == 0)
		return (void*)0;
	ret->head = 0;
	return ret;
}

GXscene_t* loadScene(const char path[])
{
	//
	// TODO 
	//

	// Uninitialized data
	int i;
	char* data;

	// Initialized data
	GXscene_t* ret = malloc(sizeof(GXscene_t));
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

	return ret;
}

int appendEntity(GXscene_t* scene, GXentity_t* entity)
{

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
		if (i->shader)
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
	return 0;
}