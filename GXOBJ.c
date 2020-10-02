#include <G10/GXMesh.h>

GXmesh_t* loadOBJMesh(const char path[])
{
	// Uninitialized data
	int       i;
	char*     data;
	char*     backup;
	// Temporaries
	point2_t* tempTextureCoordinates;
	point3_t* tempVertexNormals;

	point3_t* geometricVertices;
	point2_t* textureCoordinates;
	point3_t* vertexNormals;

	// Initialized data
	GXmesh_t* ret    = malloc(sizeof(GXmesh_t));
	FILE*     f       = fopen(path, "r");
	GXsize_t  a       = 0;
	GXsize_t  vcount  = 0;
	GXsize_t  vncount = 0;
	GXsize_t  vtcount = 0;
	GXsize_t  fcount  = 0;

	if (ret == 0)
		return (void*)0;

	if (f == NULL)
	{
		printf("failed to load file %s\n", path);
		return (void*)0;
	}

	// Find file size and prep for read
	fseek(f, 0, SEEK_END);
	i = ftell(f);
	fseek(f, 0, SEEK_SET);

	// Allocate data and read file into memory
	data = malloc(i);
	backup = data;
	fread(data, 1, i, f);

	// We no longer need the file
	fclose(f);

	// Debugger logging
	#ifdef debugmode
		printf("Loaded file %s\n", path);
	#endif

	// Preparse the file
	while (data[a])
	{
		if (data[a] == '#')
		{
			// Debugger logging
				#ifdef debugmode
					printf("Comment: ");
						while (data[a] != '\n')
							putchar(data[++a]);
				#else
						while (data[a] != '\n')
							data[++a];
				#endif
		}
		// Find out how many v, vt, vn, and f we have
		else if (data[a] == 'v')
			if (data[a + 1] == 't')
				vtcount++;
			else if (data[a + 1] == 'n')
				vncount++;
			else
				vcount++;
		else if (data[a] == 'f')
			fcount++;

		a++;

	}

	// Debugger logging
	#ifdef debugmode
		printf("Geometric vertices   %i\n"	  \
				"Texture coordinates  %i\n"   \
				"Vertex normals       %i\n"   \
				"Faces                %i\n\n" \
			, vcount, vtcount, vncount, fcount);
	#endif

	// Allocate space for elements
	tempTextureCoordinates = malloc(sizeof(point2_t) * vtcount);
	tempVertexNormals      = malloc(sizeof(point3_t) * vncount);
	geometricVertices      = malloc(sizeof(point3_t) * vcount);
	textureCoordinates     = malloc(sizeof(point2_t) * vtcount);
	vertexNormals          = malloc(sizeof(point3_t) * vncount);
	
	ret->faces.v  = malloc(sizeof(int3_t) * fcount);
	ret->faces.vt = malloc(sizeof(int3_t) * fcount);
	ret->faces.vn = malloc(sizeof(int3_t) * fcount);

	// Fill out counts before they get trashed
	ret->geometricVerticesCount  = vcount;
	ret->textureCoordinatesCount = vtcount;
	ret->vertexNormalsCount      = vncount;
	ret->facesCount              = fcount;

	// Trash local counts
	vcount  = 0;
	vtcount = 0;
	vncount = 0;
	fcount  = 0;

	// Actually parse the data
	while (*data != '\0')
	{
		// Ignore comments, as we have already handled them
		if (*data == '#')
			while (*data != '\n')
				data++;
		else if (*data == 'v')
			// Parse out texture coordinates
			if (data[1] == 't')
			{
				sscanf(data, "vt %f %f", &tempTextureCoordinates[vtcount].u, &tempTextureCoordinates[vtcount].v);
				vtcount++;
				while (*data != '\n')
					data++;
			}
			// Parse out normals
			else if (data[1] == 'n')
			{
				sscanf(data, "vn %f %f %f", &tempVertexNormals[vncount].x, &tempVertexNormals[vncount].y, &tempVertexNormals[vncount].z);
				vncount++;
				while (*data != '\n')
					data++;
			}
			// Parse out geometric vertices
			else {
				sscanf(data, "v %f %f %f", &geometricVertices[vcount].x, &geometricVertices[vcount].y, &geometricVertices[vcount].z);
				vcount++;
				while (*data != '\n')
					data++;
			}
		// Parse out faces
		else if (*data == 'f')
		{
			int3_t v =  { 0,0,0 };
			int3_t vt = { 0,0,0 };
			int3_t vn = { 0,0,0 };

			sscanf(data, "f %i/%i/%i %i/%i/%i %i/%i/%i", &v.x, &vt.x, &vn.x, &v.y, &vt.y, &vn.y, &v.z, &vt.z, &vn.z);
			
			// Arrays start at 1 in OBJ file format, so dec every int
			v.x--, v.y--, v.z--;
			vt.x--, vt.y--, vt.z--;
			vn.x--, vn.y--, vn.z--;
			
			// Assign to faces
			ret->faces.v[fcount]  = v;
			ret->faces.vt[fcount] = vt;
			ret->faces.vn[fcount] = vn;

			// Set the specific texture coordinate at v to the arbitrary texture coordinate at vt
			textureCoordinates[v.x] = tempTextureCoordinates[vt.x];
			textureCoordinates[v.y] = tempTextureCoordinates[vt.y];
			textureCoordinates[v.z] = tempTextureCoordinates[vt.z];

			// TODO: Uncomment before implementing normals
			// vertexNormals[v.x]      = tempVertexNormals[vn.x];
			// vertexNormals[v.y]      = tempVertexNormals[vn.y];
			// vertexNormals[v.z]      = tempVertexNormals[vn.z];

			fcount++;

			while (*data != '\n')
				data++;
		}
		data++;
	}

	// Generate the vertex array and all of its contents, as well as the element buffer
	glGenVertexArrays(1, &ret->vertexArray);
	glGenBuffers(1, &ret->vertexBuffer);
	glGenBuffers(1, &ret->elementBuffer);
	glGenBuffers(1, &ret->textureBuffer);

	glBindVertexArray(ret->vertexArray);
	
	// Populate and enable the vertex buffer, element buffer, and UV coordinates
	glBindBuffer(GL_ARRAY_BUFFER, ret->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(point3_t) * ret->geometricVerticesCount, geometricVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ret->elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int3_t) * ret->facesCount, ret->faces.v, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, ret->textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(point2_t) * ret->textureCoordinatesCount, textureCoordinates, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	// Free temporary coordinates
	free(tempTextureCoordinates);
	free(tempVertexNormals);

	// Free actual coordinates
	free(geometricVertices);
	free(textureCoordinates);
	free(vertexNormals);

	// Free data.
	free(backup);

	return ret;
}
