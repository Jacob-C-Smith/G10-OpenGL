#include <G10/GXPLY.h>

GXmesh_t* loadPLYMesh(const char path[])
{
	// Uninitialized data
	GXsize_t     l;
	char*        data;
	char*        backup;

	// Initialized data
	GXmesh_t*    ret                = malloc(sizeof(GXmesh_t));
	GXsize_t     geometricVertices  = 0;
	GXsize_t     textureCoordinates = 0;
	GXsize_t     i                  = 0;
	GXsize_t     j                  = 0;
	GXsize_t     k                  = 0;
	FILE*        f                  = fopen(path, "r");
	GXPLYfile_t* plyFile            = malloc(sizeof(GXPLYfile_t));

	plyFile->nElements = 0;
	plyFile->elements  = (void*)0;

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
	data = malloc(l);

	// Check to make sure we got the RAM
	if (data == 0)
		return (void*)0;
	if (ret == 0)
		return (void*)0;
	if (l < 3)
		return (void*)0;
	// Read in the data
	fread(data, 1, l, f);

	// We no longer need the file
	fclose(f);

	// Debugger logging
	#ifdef debugmode
		printf("Loaded file %s\n", path);
	#endif

	//Check signature
	if ((data[0] == 'p' && data[1] == 'l' && data[2] == 'y') == 0)
		return (void*)0;

	//Count up elements
	while (strncmp(&data[i], "end_header", 10))
	{
		if (strncmp(&data[i], "element", 7) == 0)
			plyFile->nElements++;
		while (data[i++] != '\n');
	}

	//Allocate space for elements, zero out i.
	plyFile->elements = malloc(sizeof(GXPLYelement_t) * plyFile->nElements);
	i ^= i;
	//Fill out names, counts, and property counts of elements
	while (strncmp(&data[i], "end_header", 10))
	{
		if (strncmp(&data[i], "element", 7) == 0)
		{
			GXsize_t m = 0;
			i += 8;

			while (data[i + m] != ' ')
				m++;
			plyFile->elements[j].name = malloc(m);
			plyFile->elements[j].name[m] = '\0';
			strncpy(plyFile->elements[j].name, &data[i], m);

			plyFile->elements[j].nProperties = 0;

			i += m++;
			plyFile->elements[j].nCount = atoi(&data[i]);
			while (data[i++] != '\n');
			while (strncmp(&data[i], "property", 8) == 0)
			{
				plyFile->elements[j].nProperties++;
				while (data[i++] != '\n');
			}
			plyFile->elements[j].properties = malloc(sizeof(GXPLYproperty_t) * plyFile->elements[j].nProperties);
			j++, i--;
		}
		while (data[i++] != '\n');
	}
	i ^= i;
	j ^= j;
	//Fill out names, sizes, etc of properties
	while (strncmp(&data[i], "end_header", 10))
	{
		if (strncmp(&data[i], "element", 7) == 0)
		{
			GXsize_t m = 0;
			i += 8;

			while (data[i + m] != ' ')
				m++;

			plyFile->elements[j].name = malloc(m);
			strncpy(plyFile->elements[j].name, &data[i], m);
			plyFile->elements[j].name[m] = '\0';

			plyFile->elements[j].nProperties = 0;
			i += m++;
			plyFile->elements[j].nCount = atoi(&data[i]);
			while (data[i++] != '\n');
			while (strncmp(&data[i], "property", 8) == 0)
			{
				i += 9;
				if (strncmp(&data[i], "float", 5) == 0)
				{
					i += 6;
					plyFile->elements[j].properties[k].typeSize = sizeof(float);
				}
				m = 0;
				while (data[i+m] != '\n' && data[i+m] != ' ')
					m++;

				plyFile->elements[j].properties[k].name = malloc(m);
				strncpy(plyFile->elements[j].properties[k].name, &data[i], m);
				plyFile->elements[j].properties[k].name[m] = '\0';
				k++;
				while (data[i++] != '\n');
			}
			j++, i--;
			plyFile->elements[j].properties = malloc(sizeof(GXPLYproperty_t) * plyFile->elements[j].nProperties);
		}
		while (data[i++] != '\n');
	}
	i ^= i;


	while (strncmp(&data[i], "end_header", 10))
	{
		if (strncmp(&data[i], "element", 7) == 0)
		{
			i += 8;
			size_t namelen = 0;
			size_t y = 0;
			size_t x = 0;
			size_t listItems = 0;

			while (data[i+++namelen] != ' ');
			y = atoi(&data[i]);
			while (data[i++] != '\n');
			while (strncmp(&data[i], "property", 8) == 0)
			{
				i += 9;
				if (strncmp(&data[i], "char", 4 == 0))
					x += sizeof(s8);
				else if (strncmp(&data[i], "uchar" , 5) == 0)
					x += sizeof(u8);
				else if (strncmp(&data[i], "short" , 5) == 0)
					x += sizeof(s16);
				else if (strncmp(&data[i], "ushort", 6) == 0)
					x += sizeof(u16);
				else if (strncmp(&data[i], "int"   , 3) == 0)
					x += sizeof(s32);
				else if (strncmp(&data[i], "uint"  , 4) == 0)
					x += sizeof(u32);
				else if (strncmp(&data[i], "float" , 5) == 0)
					x += sizeof(float);
				else if (strncmp(&data[i], "double", 6) == 0)
					x += sizeof(double);
				else if (strncmp(&data[i], "list"  , 4) == 0)
				{
					i += 5;
					if (strncmp(&data[i], "char"       , 4 == 0))
						listItems = sizeof(s8);
					else if (strncmp(&data[i], "uchar" , 5) == 0)
						listItems = sizeof(u8);
					else if (strncmp(&data[i], "short" , 5) == 0)
						listItems = sizeof(s16);
					else if (strncmp(&data[i], "ushort", 6) == 0)
						listItems = sizeof(u16);
					else if (strncmp(&data[i], "int"   , 3) == 0)
						listItems = sizeof(s32);
					else if (strncmp(&data[i], "uint"  , 4) == 0)
						listItems = sizeof(u32);
				}
				while (data[i++] != '\n');
			}
			i--;
			//if
		}
		else if (strncmp(&data[i], "comment", 7) == 0)
		{
			i += 7;
			#ifdef debugmode
				//print comment
			#endif
		}
		else if (strncmp(&data[i], "format", 6) == 0)
		{
			i += 6;

		}
		while (data[i++] != '\n');
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

	return ret;
}
