#include <G10/GXShader.h>

GXShader_t* loadShader ( const char vertexShaderPath[], const char fragmentShaderPath[], const char shaderName[] )
{
	// Uninitialized data
	char*        vfdata ,* ffdata;
	int          vfi    ,  ffi;
	unsigned int vShader, fShader;
	int          status;

	// Initialized data
	FILE* vf        = fopen(vertexShaderPath, "r");
	FILE* ff        = fopen(fragmentShaderPath, "r");
	
	GXShader_t* ret = malloc(sizeof(GXShader_t));
	if (ret == 0)
		return (void*)0;

	// Check files
	if (vf == NULL)
	{
		printf("Failed to load file %s\n", vertexShaderPath);
		return (void*)0;
	}

	if (ff == NULL)
	{
		printf("Failed to load file %s\n", fragmentShaderPath);
		return (void*)0;
	}

	// Find file size and prep for read
	fseek(vf, 0, SEEK_END);
	fseek(ff, 0, SEEK_END);

	vfi = ftell(vf);
	ffi = ftell(ff);

	fseek(vf, 0, SEEK_SET);
	fseek(ff, 0, SEEK_SET);

	// Allocate data and read file into memory
	vfdata = malloc(vfi);
	ffdata = malloc(ffi);
	if (vfdata == 0)
		return (void*)0;
	if (ffdata == 0)
		return (void*)0;

	fread(vfdata, 1, vfi, vf);
	fread(ffdata, 1, ffi, ff);

	// We no longer need the file
	fclose(vf);
	fclose(ff);

	vfdata[vfi-1] = '\0';
	ffdata[ffi-1] = '\0';

	// Compile, attach, and link shaders
	vShader = glCreateShader(GL_VERTEX_SHADER);
	fShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vShader, 1, &vfdata, NULL);
	glShaderSource(fShader, 1, &ffdata, NULL);

	glCompileShader(vShader);
	glCompileShader(fShader);

	glGetShaderiv(vShader, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		char* log = malloc(512);
		if (log == 0)
			return (void*)0;
		glGetShaderInfoLog(vShader, 512, NULL, log);
		printf(log);
		free(log);
	}

	status = 0;
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &status);
	if (!status)
	{
		char* log = malloc(512);
		if (log == 0)
			return (void*)0;
		glGetShaderInfoLog(fShader, 512, NULL, log);
		printf(log);
		free(log);
	}

	ret->shaderProgramID = glCreateProgram();

	glAttachShader(ret->shaderProgramID, vShader);
	glAttachShader(ret->shaderProgramID, fShader);

	glLinkProgram(ret->shaderProgramID);

	status = 0;

	glGetProgramiv(ret->shaderProgramID, GL_LINK_STATUS, &status);
	if (!status)
	{
		char* log = malloc(512);
		if (log == 0)
			return (void*)0;
		glGetProgramInfoLog(ret->shaderProgramID, 512, NULL, log);
		printf(log);
		free(log);
	}
	glDetachShader(ret->shaderProgramID, vShader);
	glDetachShader(ret->shaderProgramID, fShader);

	// Destroy the shader programs we don't need anymore
	glDeleteShader(vShader);
	glDeleteShader(fShader);

	// We no longer need the shader text
	free(vfdata);
	free(ffdata);

	return ret;

}

GXShader_t* loadShaderAsJSON ( const char shaderPath[] )
{
	// Uninitialized data
	int         i;
	char*       data;

	// Initialized data
	GXShader_t* ret;
	FILE*       f                  = fopen(shaderPath, "rb");
	char*       vertexShaderPath   = 0,
		*       fragmentShaderPath = 0,
	    *       shaderName         = 0;

	// Check if file is valid
	if (f == NULL)
	{
		printf("Failed to load file %s\n", shaderPath);
		return (void*)0;
	}

	// Find file size and prep for read
	fseek(f, 0, SEEK_END);
	i = ftell(f);
	fseek(f, 0, SEEK_SET);

	// Allocate data and read file into memory
	data = malloc(i);

	// Check if data is valid
	if (data == 0)
		return (void*)0;

	// Read to data
	fread(data, 1, i, f);

	// We no longer need the file
	fclose(f);

	data[i] = '\0';

	// Initialized data
	size_t        len            = strlen(data), 
		          rootTokenCount = GXParseJSON(data, len, 0, 0);
	JSONValue_t*  rootContents   = calloc(rootTokenCount,sizeof(JSONValue_t));

	// Parse JSON Values
	GXParseJSON(data, len, rootTokenCount, rootContents);
	
	// Find and load the textures
	for (size_t k = 0; k < rootTokenCount; k++)

		// Point to the vertex shader
		if (strcmp("vertexShaderPath", rootContents[k].name) == 0)
			vertexShaderPath = rootContents[k].content.nWhere;

		// Point to the fragment shader
		else if (strcmp("fragmentShaderPath", rootContents[k].name) == 0)
			fragmentShaderPath = rootContents[k].content.nWhere;

		// Copy out the name of the shader
		else if (strcmp("name", rootContents[k].name) == 0)
			shaderName = rootContents[k].content.nWhere;
		
		// Copy out requested data
		else if (strcmp("requestData", rootContents[k].name) == 0)
		{
			// TODO: Copy out key/value pairs for requested data
		}
		
	// Spin up the shader and set the name
	ret = loadShader(vertexShaderPath, fragmentShaderPath, shaderName);
	
	// Free subcontents
	free(rootContents);
	
	if (vertexShaderPath == 0 || fragmentShaderPath == 0)
		return vertexShaderPath;

	// Set the material and flip the flag
	return ret;
}

int useShader ( GXShader_t* shader )
{
	if(shader)
		glUseProgram(shader->shaderProgramID);

	return 0;
}

void setShaderInt ( GXShader_t* shader, const char name[], int value )
{
	glUniform1i(glGetUniformLocation(shader->shaderProgramID, name),value);
}

void setShaderFloat ( GXShader_t* shader, const char name[], float value )
{
	glUniform1f(glGetUniformLocation(shader->shaderProgramID, name),value);
}

void setShaderVec3 ( GXShader_t* shader, const char name[], GXvec3_t vector )
{
	glUniform3f(glGetUniformLocation(shader->shaderProgramID, name), vector.x, vector.y, vector.z);
}

void setShaderMat4 ( GXShader_t* shader, const char name[], GXmat4_t* m )
{
	glUniformMatrix4fv(glGetUniformLocation(shader->shaderProgramID, name), 1, GL_FALSE, m);
}

int unloadShader ( GXShader_t* shader )
{
	// Deallocate associated data
	glDeleteProgram(shader->shaderProgramID);
	shader->shaderProgramID = 0;
	free(shader);

	return 0;
}