#include <G10/GXEntity.h>

GXentity_t* createEntity(GXsize_t flags)
{
	// Allocate space
	GXentity_t* ret = malloc(sizeof(GXentity_t)); 

	// Check if valid
	if (ret == 0)                                 
		return (void*)0;

	// Set flags
	ret->flags    = flags;

	// Initialize to nullptr
	ret->mesh     = (void*)0;
	ret->name     = (void*)0;
	ret->next     = (void*)0;
	ret->shader   = (void*)0;
	ret->UV       = (void*)0;

	// Initialize to null
	ret->location = (GXvec3_t){ 0,0,0 };
	ret->rotation = (GXvec3_t){ 0,0,0 };
	ret->scale    = (GXvec3_t){ 1,1,1 };
	ret->model    = identityMat4();

	return ret;
}

int drawEntity(GXentity_t* entity)
{
	// Check if its drawable
	if (!(entity->flags & GXE_drawable))
		return -1;

	// Check if we have a texture and if we are permitted to use it
	if (entity->UV && entity->flags & GXE_texture) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, entity->UV->textureID);
		assignTexture(entity->shader, "texture1");
	}

	// Set the model matrix up for the shader
	GXmat4_t m = translationScaleMat(entity->location, entity->scale);
	GXmat4_t n = rotationMatrixFromQuaternion(makeQuaternionFromEulerAngle(entity->rotation));
	m = mat4xmat4(n, m);
	glUniformMatrix4fv(glGetUniformLocation(entity->shader->shaderProgramID, "M"), 1, GL_FALSE, &m);

	// Draw the entity
	glBindVertexArray(entity->mesh->vertexArray);
	glDrawElements(GL_TRIANGLES, entity->mesh->facesCount * 3, GL_UNSIGNED_INT, 0);

	return 0;
}

GXentity_t* loadEntity(const char path[])
{
	GXentity_t* ret = createEntity(0);
	size_t l = 0;
	FILE* f = fopen(path, "rb");

	if (f == NULL)
	{
		printf("Failed to load file %s\n", path);
		return (void*)0;
	}

	// Find file size and prep for read
	fseek(f, 0, SEEK_END);
	l = ftell(f);
	fseek(f, 0, SEEK_SET);

	// Allocate data and read file into memory
	u8* data = malloc(l);
	fread(data, 1, l, f);

	// We no longer need the file
	fclose(f);

	int rootTokenCount = GXParseJSON(data, l, 0, (void*) 0);
	JSONValue_t* rootContents = malloc(sizeof(JSONValue_t) * rootTokenCount);
	GXParseJSON(data, l, rootTokenCount, rootContents);

	for (size_t j = 0; j < rootTokenCount; j++)
	{
		if (strcmp("comment", rootContents[j].name) == 0)
		#ifdef debugmode
			// Print out comment
			printf("comment in file \"%s\" - \"%s\"\n\n", path, (char*)rootContents[j].where);
		#endif
		if (strcmp("name", rootContents[j].name) == 0)
		{
			// Set name
			size_t len = strlen(rootContents[j].where);
			ret->name = malloc(len);
			strncpy(ret->name, rootContents[j].where, len);
			ret->name[len] = 0;
		}
		if (strcmp("flags", rootContents[j].name) == 0)
			// Set flags
			ret->flags = atoi(rootContents[j].where);
		if (strcmp("mesh", rootContents[j].name) == 0)
		{
			char* relativePath = 0;
			size_t len = strlen(rootContents[j].where), subTokenCount = GXParseJSON(rootContents[j].where, len, 0, 0);
			JSONValue_t* subContents = malloc(sizeof(JSONValue_t) * rootTokenCount);
			GXParseJSON(rootContents[j].where, len, subTokenCount, subContents);
			// Find relative path
			for (size_t k = 0; k < subTokenCount; k++)
				if (strcmp("relativePath", subContents[k].name) == 0)
					relativePath = subContents[k].where;
			// Find format
			for (size_t k = 0; k < subTokenCount; k++)
				if (strcmp("format", subContents[k].name) == 0)
					if (strcmp("OBJ", subContents[k].where) == 0)
						if (relativePath)
							//Use appropriate loader
							ret->mesh = loadOBJMesh(relativePath);
						else
						#ifdef debugmode
							printf("No relative path in asset file %s\n", path);
						#endif
					else if (strcmp("PLY", subContents[k].where) == 0)
						if (relativePath)
							//Use appropriate loader
							ret->mesh = loadPLYMesh(relativePath);
						else
						#ifdef debugmode
							printf("No relative path in asset file %s\n", path);
						#endif
			free(subContents);
		}
		if (strcmp("texture", rootContents[j].name) == 0)
		{
			char* relativePath = 0;
			size_t len = strlen(rootContents[j].where), subTokenCount = GXParseJSON(rootContents[j].where, len, 0, 0);
			JSONValue_t* subContents = malloc(sizeof(JSONValue_t) * rootTokenCount);
			GXParseJSON(rootContents[j].where, len, subTokenCount, subContents);
			// Find relative path
			for (size_t k = 0; k < subTokenCount; k++)
				if (strcmp("relativePath", subContents[k].name) == 0)
					relativePath = subContents[k].where;
			// Find format
			for (size_t k = 0; k < subTokenCount; k++)
				if (strcmp("format", subContents[k].name) == 0)
					if (strcmp("BMP", subContents[k].where) == 0)
						if (relativePath)
							// Use appropriate loader
							ret->UV = loadBMPImage(relativePath);
						else
						#ifdef debugmode
							printf("No relative path in asset file %s\n", path);
						#endif
					else if (strcmp("PNG", subContents[k].where) == 0)
						if (relativePath)
							// Use appropriate loader
							ret->UV = loadPNGImage((const char*)relativePath);
						else
						#ifdef debugmode
							printf("No relative path in asset file %s\n", path);
						#endif
					else if (strcmp("JPG", subContents[k].where) == 0)
						if (relativePath)
							// Use appropriate loader
							ret->UV = loadJPGImage((const char*)relativePath);
						else
						#ifdef debugmode
							printf("No relative path in asset file %s\n", path);
						#endif
			free(subContents);
		}
		if (strcmp("shader", rootContents[j].name) == 0)
		{
			char* vertexShaderRelativePath = 0;
			char* fragmentShaderRelativePath = 0;

			size_t len = strlen(rootContents[j].where), subTokenCount = GXParseJSON(rootContents[j].where, len, 0, 0);
			JSONValue_t* subContents = malloc(sizeof(JSONValue_t) * rootTokenCount);
			GXParseJSON(rootContents[j].where, len, subTokenCount, subContents);
			// Find vertex shader relative path
			for (size_t k = 0; k < subTokenCount; k++)
				if (strcmp("vertexShaderRelativePath", subContents[k].name) == 0)
					vertexShaderRelativePath = subContents[k].where;
			// Find fragment shader relative path
			for (size_t k = 0; k < subTokenCount; k++)
				if (strcmp("fragmentShaderRelativePath", subContents[k].name) == 0)
					fragmentShaderRelativePath = subContents[k].where;

			if (vertexShaderRelativePath && fragmentShaderRelativePath)
				// Load the shader
				ret->shader = loadShader(vertexShaderRelativePath, fragmentShaderRelativePath);

			free(subContents);
		}
	}
	// Finish up
	ret->next = 0;

	return ret;
}

int assignTexture(GXshader_t* shader, const char uniform[])
{
	// We can not assign a texture if there is no shader
	if (shader == (void*) 0)
		return -1;

	// Use the shader and set the uniform
	useShader(shader);
	glUniform1i(glGetUniformLocation(shader->shaderProgramID, uniform), 0);

	return 0;
}

int destroyEntity(GXentity_t* entity)
{
	// Check to see if items are set before we unload them
	if (entity->name != (void*)0)
		entity->name = (void*)0;

	if (entity->mesh != (void*)0)
		unloadMesh(entity->mesh);

	if (entity->shader != (void*)0)
		unloadShader(entity->shader);

	if (entity->UV != (void*)0)
		unloadTexture(entity->UV);
	
	entity->flags = 0;

	// Free the entity
	free(entity);

	return 0;
}
