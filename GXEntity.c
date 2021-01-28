#include <G10/GXEntity.h>

GXEntity_t* createEntity ( )
{
	// Allocate space
	GXEntity_t* ret = malloc(sizeof(GXEntity_t)); 

	// Check if valid
	if (ret == 0)
		return ret;

	// Set flags
	ret->flags     = 0;

	// Initialize to nullptr
	ret->name      = (void*)0;
	ret->mesh      = (void*)0;
	ret->shader    = (void*)0;
	ret->material  = (void*)0;
	ret->transform = (void*)0;
	ret->next      = (void*)0;

	return ret;
}

int drawEntity ( GXEntity_t* entity )
{
	// If the drawable flag is not set, we can stop
	if (!(entity->flags & GXEFDrawable))
		return -1;

	// If the transform flag is set, use the transform
	if (entity->flags & GXEFTransform)
	{
		entity->transform->modelMatrix = mat4xmat4(rotationMatrixFromQuaternion(makeQuaternionFromEulerAngle(entity->transform->rotation)), translationScaleMat(entity->transform->location, entity->transform->scale));
		setShaderMat4(entity->shader, "M", &entity->transform->modelMatrix);
	}
	
	// Draw the entity
	glBindVertexArray(entity->mesh->vertexArray);
	glDrawElements(GL_TRIANGLES, entity->mesh->elementsInBuffer * 3, GL_UNSIGNED_INT, 0);

	return 0;
}

GXEntity_t* loadEntity ( const char path[] )
{
	// Initialized data
	GXEntity_t* ret = createEntity();
	size_t      l   = 0;
	FILE*       f   = fopen(path, "r");

	// Check for file
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

	// Parse the first level of the JSON entity
	int rootTokenCount = GXParseJSON(data, l, 0, (void*) 0);
	JSONValue_t* rootContents = malloc(sizeof(JSONValue_t) * rootTokenCount);
	GXParseJSON(data, l, rootTokenCount, rootContents);
	
	// Search through values and pull out relevent information
	for (size_t j = 0; j < rootTokenCount; j++)
	{
		// Handle comments
		if (strcmp("comment", rootContents[j].name) == 0)
			#ifdef GXDEBUGMODE & GXDEBUGCOMMENTS
				// Print out comment
				printf("comment in file \"%s\" - \"%s\"\n\n", path, (char*)rootContents[j].content.nWhere);
			#endif

		// Set name
		if (strcmp("name", rootContents[j].name) == 0)
		{
			size_t len = strlen(rootContents[j].content.nWhere);
			ret->name = malloc(len);
			strncpy(ret->name, rootContents[j].content.nWhere, len);
			ret->name[len] = 0;
		}

		// Load and process a mesh
		if (strcmp("mesh", rootContents[j].name) == 0)
		{
			ret->mesh  = loadMesh(rootContents[j].content.nWhere);
			ret->flags |= GXEFMesh;
		}
		
		// Process a shader
		if (strcmp("shader", rootContents[j].name) == 0)
		{
			// Process shader as path
			if (rootContents[j].type == GXJSONstring)
			{
				ret->shader = loadShaderAsJSON((const char*)rootContents[j].content.nWhere);
				goto shaderSet;
			}

			// Initialized data
			size_t       len                        = strlen(rootContents[j].content.nWhere), subTokenCount = GXParseJSON(rootContents[j].content.nWhere, len, 0, 0);
			JSONValue_t* subContents                = malloc(sizeof(JSONValue_t) * rootTokenCount);

			char*        vertexShaderPath           = 0;
			char*        fragmentShaderPath         = 0;
			char*        name                       = 0;

			// Parse JSON Values
			GXParseJSON(rootContents[j].content.nWhere, len, subTokenCount, subContents);

			// Find vertex and fragment shader path
			for (size_t k = 0; k < subTokenCount; k++)
				if      (strcmp("vertexShaderPath"  , subContents[k].name) == 0)
					vertexShaderPath = subContents[k].content.nWhere;
				else if (strcmp("fragmentShaderPath", subContents[k].name) == 0)
					fragmentShaderPath = subContents[k].content.nWhere;
				else if (strcmp("name"              , subContents[k].name) == 0)
					name = subContents[k].content.nWhere;
			// Load the shader if there are enough paths supplied
			if (vertexShaderPath && fragmentShaderPath)
				ret->shader = loadShader(vertexShaderPath, fragmentShaderPath,name);

			// Free subcontents
			free(subContents);
			shaderSet:
			// Set the shader flag
			ret->flags |= GXEFShader;
		}

		// Process a transform
		if (strcmp("transform", rootContents[j].name) == 0)
		{
			// Initialiazed data
			size_t       len         = strlen(rootContents[j].content.nWhere), subTokenCount = GXParseJSON(rootContents[j].content.nWhere, len, 0, 0);
			JSONValue_t* subContents = malloc(sizeof(JSONValue_t) * rootTokenCount);

			GXvec3_t     location    = {0,0,0};
			GXvec3_t     rotation    = {0,0,0};
			GXvec3_t     scale       = {0,0,0};
			
			// Parse JSON Values
			GXParseJSON(rootContents[j].content.nWhere, len, subTokenCount, subContents);
			
			// Find location, rotation, and scale
			for (size_t k = 0; k < subTokenCount; k++)
				if      (strcmp("location", subContents[k].name) == 0)
					location = (GXvec3_t){ atof(subContents[k].content.aWhere[0]), atof(subContents[k].content.aWhere[1]), atof(subContents[k].content.aWhere[2]) };
				else if (strcmp("rotation", subContents[k].name) == 0)
					rotation = (GXvec3_t){ atof(subContents[k].content.aWhere[0]), atof(subContents[k].content.aWhere[1]), atof(subContents[k].content.aWhere[2]) };
				else if (strcmp("scale", subContents[k].name)    == 0)
					scale = (GXvec3_t){ atof(subContents[k].content.aWhere[0]), atof(subContents[k].content.aWhere[1]), atof(subContents[k].content.aWhere[2]) };
			
			// Process transform
			ret->transform = createTransform(location, rotation, scale);

			// Free subcontents
			free(subContents);

			// Set the transform flag
			ret->flags |= GXEFTransform;
		}

		// Process a material
		if (strcmp("material", rootContents[j].name) == 0)
		{
			// Process material as a file
			if (rootContents[j].type == GXJSONstring)
			{
				GXMaterial_t* material = loadMaterial((const char*)rootContents[j].content.nWhere);
				ret->material          = material;
				goto materialSet;
			}

			// Initialized data
			size_t        len           = strlen(rootContents[j].content.nWhere),
				          subTokenCount = GXParseJSON(rootContents[j].content.nWhere, len, 0, 0);
			JSONValue_t*  subContents   = malloc(sizeof(JSONValue_t) * rootTokenCount);
			GXMaterial_t* material      = createMaterial();

			// Parse JSON Values
			GXParseJSON(rootContents[j].content.nWhere, len, subTokenCount, subContents);

			// Find and load the textures
			for (size_t k = 0; k < subTokenCount; k++)
				if      (strcmp("albedo",    subContents[k].name) == 0)
					material->albedo = loadTexture(subContents[k].content.nWhere);
				else if (strcmp("normal",    subContents[k].name) == 0)
					material->normal = loadTexture(subContents[k].content.nWhere);
				else if (strcmp("metallic",  subContents[k].name) == 0)
					material->metallic = loadTexture(subContents[k].content.nWhere);
				else if (strcmp("roughness", subContents[k].name) == 0)
					material->roughness = loadTexture(subContents[k].content.nWhere);
				else if (strcmp("AO",        subContents[k].name) == 0)
					material->AO = loadTexture(subContents[k].content.nWhere);
			
			// Free subcontents
			free(subContents);

			// Set the material and flip the flag
			ret->material       = material;
			materialSet: 
			ret->flags |= GXEFMaterial;
		}
	}

	if (ret->flags  & GXEFMesh   &&       // Its got to be something that can be drawn if there is a mesh
		ret->flags  & GXEFShader &&       // and a shader. 
		(ret->flags & GXEFDrawable) == 0) // Unless otherwise stated
		ret->flags |= GXEFDrawable;

	// Finish up
	free(rootContents);
	free(data);
	ret->next = 0;

	// Set the present flag
	ret->flags |= GXEFPresent;
	return ret;
}

int assignTexture ( GXShader_t* shader, const char uniform[] )
{
	// We can not assign a texture if there is no shader
	if (shader == (void*) 0)
		return -1;

	// Use the shader and set the uniform
	useShader(shader);
	glUniform1i(glGetUniformLocation(shader->shaderProgramID, uniform), 0);

	return 0;
}

int destroyEntity ( GXEntity_t* entity )
{
	// Unset all flags
	entity->flags = 0;

	// Check to see if items are set before we unload them
	if (entity->name != (void*)0)
		entity->name = (void*)0;

	if (entity->mesh != (void*)0)
		unloadMesh(entity->mesh);

	if (entity->shader != (void*)0)
		unloadShader(entity->shader);
	
	if (entity->transform != (void*)0)
		unloadTransform(entity->transform);

	// Free the entity
	free(entity);

	return 0;
}
