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
	ret->rigidbody = (void*)0;
	ret->collider  = (void*)0;

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
	u8* data = malloc(l+1);
	fread(data, 1, l, f);

	// We no longer need the file
	fclose(f);

	// Parse the first level of the JSON entity
	int rootTokenCount = GXParseJSON(data, l, 0, (void*) 0);
	JSONValue_t* rootContents = calloc(rootTokenCount,sizeof(JSONValue_t));
	GXParseJSON(data, l, rootTokenCount, rootContents);
	
	// Search through values and pull out relevent information
	for (size_t j = 0; j < rootTokenCount; j++)
	{
		// Handle comments
		if (strcmp("comment", rootContents[j].name) == 0)
		{
			#ifndef NDEBUG
			// Print out comment
			printf("comment in file \"%s\" - \"%s\"\n", path, (char*)rootContents[j].content.nWhere);
			#endif
		}

		// If flags are set, then we will use the provided flags. If flags are not set,
		// we will detect what flags can be set automatically
		else if (strcmp("flags", rootContents[j].name) == 0)
		{

		}

		// Set name
		else if (strcmp("name", rootContents[j].name) == 0)
		{
			size_t len = strlen(rootContents[j].content.nWhere);
			ret->name  = malloc(len+1);
			strncpy(ret->name, rootContents[j].content.nWhere, len);
			ret->name[len]   = 0;
		}

		// Load and process a mesh
		else if (strcmp("mesh", rootContents[j].name) == 0)
		{
			ret->mesh  = loadMesh(rootContents[j].content.nWhere);
			ret->flags |= GXEFMesh;
		}
		
		// Process a shader
		else if (strcmp("shader", rootContents[j].name) == 0)
		{
			// Process shader as path
			if (rootContents[j].type == GXJSONstring)
			{
				ret->shader = loadShaderAsJSON((const char*)rootContents[j].content.nWhere);
				goto shaderSet;
			}

			// Initialized data
			size_t       len                        = strlen(rootContents[j].content.nWhere), subTokenCount = GXParseJSON(rootContents[j].content.nWhere, len, 0, 0);
			JSONValue_t* subContents                = calloc(rootTokenCount,sizeof(JSONValue_t));

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

			// Set the shader flag
			shaderSet:
			ret->flags |= GXEFShader;
		}
		
		// Process a material
		else if (strcmp("material", rootContents[j].name) == 0)
		{
			// TODO: Process multiple albedos
			// Process material as a path
			if (rootContents[j].type == GXJSONstring)
			{
				ret->material = loadMaterial((const char*)rootContents[j].content.nWhere);
				goto materialSet;
			}

			// Initialized data
			size_t        len           = strlen(rootContents[j].content.nWhere),
				          subTokenCount = GXParseJSON(rootContents[j].content.nWhere, len, 0, 0);
			JSONValue_t*  subContents   = calloc(rootTokenCount, sizeof(JSONValue_t));
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

		// Process a transform
		else if (strcmp("transform", rootContents[j].name) == 0)
		{

			// Process transform as a file
			if (rootContents[j].type == GXJSONstring)
			{
				GXTransform_t* transform = loadTransform((const char*)rootContents[j].content.nWhere);
				ret->transform = transform;
				goto transformSet;
			}
			
			// Initialiazed data
			size_t       len         = strlen(rootContents[j].content.nWhere), subTokenCount = GXParseJSON(rootContents[j].content.nWhere, len, 0, 0);
			JSONValue_t* subContents = calloc(rootTokenCount,sizeof(JSONValue_t));

			GXvec3_t     location    = { 0,0,0 };
			GXvec3_t     rotation    = { 0,0,0 };
			GXvec3_t     scale       = { 0,0,0 };
			
			// Parse JSON Values
			GXParseJSON(rootContents[j].content.nWhere, len, subTokenCount, subContents);
			
			// Find location, rotation, and scale
			for (size_t k = 0; k < subTokenCount; k++)
				if      (strcmp("location", subContents[k].name) == 0)
					location = ( GXvec3_t ) { (float)atof( subContents[k].content.aWhere[0]), (float)atof(subContents[k].content.aWhere[1]), (float)atof(subContents[k].content.aWhere[2]) };
				else if (strcmp("rotation", subContents[k].name) == 0)
					rotation = (GXvec3_t) { (float)atof( subContents[k].content.aWhere[0]), (float)atof(subContents[k].content.aWhere[1]), (float)atof(subContents[k].content.aWhere[2]) };
				else if (strcmp("scale", subContents[k].name)    == 0)
					scale = (GXvec3_t) { (float)atof( subContents[k].content.aWhere[0]), (float)atof(subContents[k].content.aWhere[1]), (float)atof(subContents[k].content.aWhere[2]) };
			
			// Process transform
			ret->transform = createTransform(location, rotation, scale);

			// Free subcontents
			free(subContents);

			transformSet:
			// Set the transform flag
			ret->flags |= GXEFTransform;
		}

		// Process a rigidbody
		else if (strcmp("rigidbody", rootContents[j].name) == 0)
		{
			// TODO: Parse as path
			
			// Parse as object
			
			// Parse the first level of the JSON entity
			int subTokenCount = GXParseJSON(rootContents[j].content.nWhere, l, 0, (void*)0);
			JSONValue_t* subContents = calloc(subTokenCount, sizeof(JSONValue_t));
			GXParseJSON(rootContents[j].content.nWhere, l, subTokenCount, subContents);
			// TODO: Finish parsing rigidbodies
/*
			for (size_t k = 0; k < subTokenCount; k++);
				if (strcmp("mass", subContents[k].name) == 0)
				{
					ret->rigidbody = createRigidbody((float)atof(subContents[k].content.nWhere), 1.f,true);
				}*/
			free(subContents);
		}

		// Process a collider
		else if (strcmp("collider", rootContents[j].name) == 0)
		{
			
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
		free(entity->name); entity->name = (void*)0;

	if (entity->mesh != (void*)0)
		unloadMesh(entity->mesh);

	if (entity->shader != (void*)0)
		entity->shader = (void*)0;

	if (entity->material != (void*)0)
		entity->material = (void*)0;

	if (entity->transform != (void*)0)
		entity->transform = (void*)0;

	if (entity->rigidbody != (void*)0)
		entity->rigidbody = (void*)0;

	if (entity->collider != (void*)0)
		entity->collider = (void*)0;

	entity->next = (void*)0;

	// Free the entity
	free(entity);

	return 0;
}
