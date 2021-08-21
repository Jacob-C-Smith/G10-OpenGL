#include <G10/GXPLY.h>

GXPart_t* loadPLY( const char path[], GXPart_t *part ) {
    
    // Commentary
    {
        /* 
         * The PLY loaders job is essentially to turn a PLY file header into a 64 bit metadata 
         * value. It is written to recognize vertex properties and parse them into common vertex
         * groups. Each vertex property in a vertex group should appear concurrently. Each 
         * group has an associated code. The vertex groups metadata value is used to create a 
         * vertex array object, vertex buffer object, and element buffer object.  
         * 
         * ┌─────────────────────┬──────────────────────────────────────────────────┬──────┐
         * │ Vertex group name   │ Vertex properties as they appear in the PLY file │ Code │                                                   
         * ├─────────────────────┼──────────────────────────────────────────────────┼──────┤
         * │ Geometric vertices  │ ( x, y, z )                                      │ 0x01 │
         * │ Texture coordinates │ ( u, v ) or ( s, t )                             │ 0x02 │
         * │ Vertex normals      │ ( nx, ny, nz )                                   │ 0x04 │
         * │ Vertex normals      │ ( bx, by, bz )                                   │ 0x08 │
         * │ Vertex colors       │ ( red, green, blue, alpha ) or ( r, g, b, a )    │ 0x10 │
         * │ Bone groups         │ ( b0, b1, b2, b3 )                               │ 0x20 │
         * │ Bone weights        │ ( w0, w1, w2, w3 )                               │ 0x40 │
         * └─────────────────────┴──────────────────────────────────────────────────┴──────┘
         * 
         * NOTE: The function can be modified to read two more vertex groups, supporting a 
         * maximum of 8 vertex groups.
         * 
         * The parser makes several passes through the header, populating more and more data each 
         * time. Each pass and its purpose is listed below
         * 
         * Pass 1.
         *     On pass 1, the function counts all the elements in the header. It will also print
         *     any comments it encounters. After this pass, the elements are allocated for.
         * 
         * Pass 2.
         *     On pass 2, the elements are populated and the properties are counted up and 
         *     allocated for.
         * 
         * Pass 3. 
         *     On pass 3, the properties are populated and the stride of the element is computed.
         *     Each property is kept track of using bitflags and a temporary value. The temporary 
         *     value is then tested with various AND masks to deduce what vertex groups are 
         *     present and which are absent. 
         * 
         *     All this work to determine the vertex groups is integrated when the function creates
         *     the vertexGroups variable. This 64 bit number is an especially compact way to 
         *     represent both the type and position of each vertex group we have encountered. Each 
         *     code is packed into  the 64 bit number, such that the last vertex group occupies 
         *     the eight least significant bits. For comprehension and completeness, here are a 
         *     few examples and how they should be interpreted.
         * 
         *        63......................................0
         *     A. | 00 | 00 | 00 | 00 | 00 | 01 | 04 | 08 |
         *     B. | 00 | 00 | 00 | 00 | 00 | 04 | 02 | 01 |
         *     C. | 00 | 00 | 01 | 02 | 04 | 08 | 10 | 20 |
         * 
         *     A. First is geometric, second is normals, third is colors 
         *     B. First is normals, second is texture coordinates, third is geometric
         *     C. First is geometric, second is texture coordinates, third is normals, fourth is 
         *        colors, fifth is bone groups, sixth is bone weights.
         * 
         * After the third pass, we have extracted all of the valuable information we can from the
         * header. We are now prepared to start making the VAO, and VBO. The last meaningful byte
         * of the vertex groups variable is masked off, and switch()ed against known vertex groups. 
         * Vertex attribute pointers are created for the specific offset and stride of the vertex
         * group automatically. The first vertex group will be at layout position 0, the second at
         * 1, the third at 2, soforth. 
         */
    }

    // Argument checking
    {
        #ifndef NDEBUG
            if (part == 0)
                goto noPart;
            if (path == 0)
                goto noPath;
        #endif
    }

    // Uninitialized data
    size_t         verticesInBuffer;
    char          *data;
    char          *cData;
    float         *vertexArray;
    GXPLYindex_t  *indices;
    u32           *correctedIndicies;

    // Initialized data
    size_t         i                  = 0,
                   j                  = 0,
                   k                  = 0,
                   vertexGroupCount   = 0,
                   vertexAttribOffset = 0;
    FILE          *f                  = fopen(path, "rb");
    GXPLYfile_t   *plyFile            = calloc(1, sizeof(GXPLYfile_t));

    // Load the file
    i = gLoadFile(path, 0);
    data = calloc(i, sizeof(u8));
    gLoadFile(path, data);

    i ^= i;

    // Populate the PLY file
    {
        // Make sure the header isn't corrupted
        if (*(u32*)data != GXPLY_HSignature)
            goto invalidHeader;

        // Copy of data pointer
        cData = data;


        // Pass 1
        while (*(u32*)cData != GXPLY_HEnd)
        {
            // Check the first four bytes of the line
            if(*(u32*)cData == GXPLY_HElement)
                plyFile->nElements++;

            // Here is as good a place as any to look for comments
            #ifndef NDEBUG
                if(*(u32*)cData == GXPLY_HComment)
                {
                    i = 0;
                    while (cData[++i] != '\n');
                    gPrintLog("[G10] [PLY] Comment in file \"%s\" : %.*s\n", path, i-8, &cData[8]);
                }
            #endif

            // Zero set the index
            i = 0;
            while (cData[++i] != '\n'); // Skip to the end of the line
            cData = &cData[i + 1];      // Set the pointer
        }

        // Allocate for PLY file elements
        plyFile->elements = calloc(plyFile->nElements+1, sizeof(GXPLYelement_t));

        // Copy the data pointer again
        cData = data;

        // Pass 2
        {
            while (*(u32*)cData != GXPLY_HEnd)
            {

                // Check if we are on an element
                if (*(u32*)cData == GXPLY_HElement)
                {
                    // TODO: Dynamically determine size. 

                    plyFile->elements[j].name = calloc(65, sizeof(u8));
                    sscanf(cData, "element %s %d\n", plyFile->elements[j].name, &plyFile->elements[j].nCount);


                    i = 0;
                    while (cData[++i] != '\n'); // Skip to the end of the line
                    cData = &cData[i + 1];      // Set the pointer

                    // Check if we are on a property
                    while (*(u32*)cData == GXPLY_HProperty)
                    {
                        // Increment properties
                        plyFile->elements[j].nProperties += 1;

                        // Zero set the index
                        i = 0;
                        while (cData[++i] != '\n'); // Skip to the end of the line
                        cData = &cData[i + 1];      // Set the pointer
                    }
                    goto p2propertyExit;

                    // TODO: Copy out the name, count, and veretx count
                }

                // Zero set the index
                i = 0;
                while (cData[++i] != '\n'); // Skip to the end of the line
                cData = &cData[i + 1];      // Set the pointer
                continue;

                // Allocate the space we need for the properties on the way out
            p2propertyExit:
                plyFile->elements[j].properties = calloc(plyFile->elements[j].nProperties, sizeof(GXPLYproperty_t));
                j++;
            }
        }
        
        // Zero set j
        j ^= j;
        
        // Copy data pointer again
        cData = data;

        // Pass 3
        {
            while (*(u32*)cData != GXPLY_HEnd)
            {

                // Check if we are on an element
                if (*(u32*)cData == GXPLY_HElement)
                {
                    // TODO: Dynamically determine size. 

                    i = 0;
                    while (cData[++i] != '\n'); // Skip to the end of the line
                    cData = &cData[i + 1];      // Set the pointer

                    // Check if we are on a property
                    while (*(u32*)cData == GXPLY_HProperty)
                    {
                        // Increment properties
                        cData += 9;

                        // Compute stride and type size for each element and property respectively
                        switch (*(u32*)cData)
                        {
                        case GXPLY_char:
                        case GXPLY_uchar:
                            plyFile->elements[j].sStride                += 1;
                            plyFile->elements[j].properties[k].typeSize =  1;
                            break;
                        case GXPLY_short:
                        case GXPLY_ushort:
                            plyFile->elements[j].sStride                += 2;
                            plyFile->elements[j].properties[k].typeSize =  2;
                            break;
                        case GXPLY_int:
                        case GXPLY_uint:
                        case GXPLY_float:
                            plyFile->elements[j].sStride                += 4;
                            plyFile->elements[j].properties[k].typeSize =  4;
                            break;
                        case GXPLY_list:
                            goto p3propertyExit;
                            break;
                        case GXPLY_double:
                            goto noDoubleSupport;
                            break;
                        default:
                            goto unrecognizedPropertyType;
                            break;

                        }

                        cData                 = strchr(cData, ' ') + 1;
                        size_t propertyLength = strchr(cData, '\n') - cData;

                        plyFile->elements[j].properties[k].name = calloc(propertyLength + 1, sizeof(u8));
                        strncpy(plyFile->elements[j].properties[k].name, cData, propertyLength);

                        // Zero set the index
                        i = 0;
                        while (cData[++i] != '\n'); // Skip to the end of the line
                        cData = &cData[i + 1];      // Set the pointer
                        k++;
                    }
                    goto p3propertyExit;

                }

                // Zero set the index
                i = 0;
                while (cData[++i] != '\n'); // Skip to the end of the line
                cData = &cData[i + 1];      // Set the pointer
                continue;

                p3propertyExit:j++;
            }
        }
    }

    // Create flags
    {
        int tflags = 0;
        // Determine what properties are in the file
        {
            for (int a = 0; a < 1; a++)
                for (int b = 0; b < plyFile->elements[a].nProperties; b++)
                    if (*plyFile->elements[a].properties[b].name == 'x')
                    {
                        plyFile->flags <<= 8;
                        plyFile->flags |= (GXPLY_Geometric);
                        tflags |= GXPLY_X;
                        vertexGroupCount++;
                    }
                    else if (*plyFile->elements[a].properties[b].name == 'y')
                        tflags |= GXPLY_Y;
                    else if (*plyFile->elements[a].properties[b].name == 'z')
                        tflags |= GXPLY_Z;
                    else if (*plyFile->elements[a].properties[b].name == 's')
                    {
                        plyFile->flags <<= 8;
                        plyFile->flags |= (GXPLY_Texture);
                        tflags |= GXPLY_S;
                        vertexGroupCount++;
                    }
                    else if (*plyFile->elements[a].properties[b].name == 't')
                        tflags |= GXPLY_T;
                    else if (strncmp(plyFile->elements[a].properties[b].name, "nx", 2) == 0)
                    {
                        plyFile->flags <<= 8;
                        plyFile->flags |= (GXPLY_Normal);
                        tflags |= GXPLY_NX;
                        vertexGroupCount++;
                    }
                    else if (strncmp(plyFile->elements[a].properties[b].name, "ny", 2) == 0)
                        tflags |= GXPLY_NY;
                    else if (strncmp(plyFile->elements[a].properties[b].name, "nz", 2) == 0)
                        tflags |= GXPLY_NZ;
                    else if (strncmp(plyFile->elements[a].properties[b].name, "bx", 2) == 0)
                    {
                        plyFile->flags <<= 8;
                        plyFile->flags |= (GXPLY_Texture);
                        tflags |= (GXPLY_BX);
                        vertexGroupCount++;
                    }
                    else if (strncmp(plyFile->elements[a].properties[b].name, "by", 2) == 0)
                        tflags |= GXPLY_BY;
                    else if (strncmp(plyFile->elements[a].properties[b].name, "bz", 2) == 0)
                        tflags |= GXPLY_BZ;
                    else if (strncmp(plyFile->elements[a].properties[b].name, "red", 3) == 0)
                    {
                        plyFile->flags <<= 8;
                        plyFile->flags  |= (GXPLY_Color);
                        tflags          |= GXPLY_R;
                        vertexGroupCount++;
                    }
                    else if (strncmp(plyFile->elements[a].properties[b].name, "green", 5) == 0)
                        tflags |= GXPLY_G;
                    else if (strncmp(plyFile->elements[a].properties[b].name, "blue", 4) == 0)
                        tflags |= GXPLY_B;
                    else if (strncmp(plyFile->elements[a].properties[b].name, "alpha", 5) == 0)
                        tflags |= GXPLY_A;
                    else if (strncmp(plyFile->elements[a].properties[b].name, "b0", 2) == 0)
                    {
                        if (tflags & GXPLY_B1 || tflags & GXPLY_B2 || tflags & GXPLY_B3)
                            goto irregularVertices;
                        plyFile->flags <<= 8;
                        plyFile->flags  |= (GXPLY_Bones);
                        tflags          |= GXPLY_B0;
                        vertexGroupCount++;
                    }
                    else if (strncmp(plyFile->elements[a].properties[b].name, "b1", 2) == 0)
                        tflags |= GXPLY_B1;
                    else if (strncmp(plyFile->elements[a].properties[b].name, "b2", 2) == 0)
                        tflags |= GXPLY_B2;
                    else if (strncmp(plyFile->elements[a].properties[b].name, "b3", 2) == 0)
                        tflags |= GXPLY_B3;
                    else if (strncmp(plyFile->elements[a].properties[b].name, "w0", 2) == 0)
                    {
                        plyFile->flags <<= 8;
                        plyFile->flags  |= (GXPLY_Weights);
                        tflags          |= GXPLY_BW1;
                        vertexGroupCount++;
                    }
                    else if (strncmp(plyFile->elements[a].properties[b].name, "w1", 2) == 0)
                        tflags |= GXPLY_BW1;
                    else if (strncmp(plyFile->elements[a].properties[b].name, "w2", 2) == 0)
                        tflags |= GXPLY_BW2;
                    else if (strncmp(plyFile->elements[a].properties[b].name, "w3", 2) == 0)
                        tflags |= GXPLY_BW3;
        }

                // Check the integrity of the mesh
        #ifndef NDEBUG
        {
            if (plyFile->flags & GXPLY_Geometric &&
                !( tflags & GXPLY_X &&
                   tflags & GXPLY_Y &&
                   tflags & GXPLY_Z ))
                goto missingVerts;

            if (plyFile->flags & GXPLY_Texture &&
                !( tflags & GXPLY_S &&
                   tflags & GXPLY_T ))
                goto missingVerts;

            if (plyFile->flags & GXPLY_Normal &&
                !( tflags & GXPLY_NX &&
                   tflags & GXPLY_NY &&
                   tflags & GXPLY_NZ ))
                goto missingVerts;

            if (plyFile->flags & GXPLY_Bitangent &&
                !(tflags & GXPLY_BX &&
                    tflags & GXPLY_BY &&
                    tflags & GXPLY_BZ))
                goto missingVerts;

            if (plyFile->flags & GXPLY_Color &&
                !( tflags & GXPLY_R &&
                   tflags & GXPLY_G &&
                   tflags & GXPLY_B ))
                goto missingVerts;

            if (plyFile->flags & GXPLY_Bones &&
                !( tflags & GXPLY_B0 &&
                   tflags & GXPLY_B1 &&
                   tflags & GXPLY_B2 &&
                   tflags & GXPLY_B3 ))
                goto missingVerts;

            if (plyFile->flags & GXPLY_Weights &&
                !( tflags & GXPLY_BW0 &&
                   tflags & GXPLY_BW1 &&
                   tflags & GXPLY_BW2 &&
                   tflags & GXPLY_BW3 ))
                goto missingVerts;
        }
        #endif
        goto processVAO;
        missingVerts:
            gPrintLog("[G10] [PLY] Missing vertex attributes detected in \"%s\"\n",path);
            goto processVAO;
        irregularVertices:
            gPrintLog("[G10] [PLY] Detected irregular vertex attribute grouping in \"%s\"\n", path);
            goto processVAO;
    }
    processVAO:
    while (strncmp(&data[++i], "end_header", 10));
    i += 11;

    // Here we set a few variables
    vertexArray            = (float*)&data[i];
    verticesInBuffer       = plyFile->elements[0].nCount * plyFile->elements[0].sStride;
    indices                = (void*)&data[i + verticesInBuffer];
    part->elementsInBuffer = ((GLuint)plyFile->elements[1].nCount * (GLuint)3);

    correctedIndicies      = calloc((size_t)part->elementsInBuffer+2, sizeof(float));

    if (correctedIndicies == (void*)0)
        return correctedIndicies;
    
    size_t indcnt = 0;

    // Fixes the indices
    for (i = 0; i < plyFile->elements[1].nCount; i++)
    {
        correctedIndicies[i * 3 + 0] = indices[i].a;
        correctedIndicies[i * 3 + 1] = indices[i].b;
        correctedIndicies[i * 3 + 2] = indices[i].c;
        #ifndef NDEBUG
            indcnt += indices[0].count;
        #endif
    }

    #ifndef NDEBUG
        if (indcnt % 3 != 0)
            goto nonTriangulated;
    #endif

    // Create buffers
    {
        // Generate the vertex array and all of its contents, as well as the element buffer
        glGenVertexArrays(1, &part->vertexArray);
        glGenBuffers(1, &part->vertexBuffer);
        glGenBuffers(1, &part->elementBuffer);

        glBindVertexArray(part->vertexArray);

        // Populate and enable the vertex buffer, element buffer, and UV coordinates
        glBindBuffer(GL_ARRAY_BUFFER, part->vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, verticesInBuffer, vertexArray, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, part->elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, part->elementsInBuffer * sizeof(u32), correctedIndicies, GL_STATIC_DRAW);
    }
    
    for (size_t i = 0; vertexGroupCount > i; i++)
    {
        size_t g = (0xFF << (8*(vertexGroupCount - 1)));
        char   h = (plyFile->flags & g) >> (8 * (vertexGroupCount - 1));
        plyFile->flags <<= 8;
        switch (h)
        {
        case GXPLY_Geometric:
        case GXPLY_Normal:
        case GXPLY_Bitangent:
        case GXPLY_Color:
            glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, plyFile->elements[0].sStride, vertexAttribOffset * sizeof(float));
            vertexAttribOffset += 3;
            glEnableVertexAttribArray(i);
            break;

        case GXPLY_Texture:
            glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, plyFile->elements[0].sStride, vertexAttribOffset * sizeof(float));
            vertexAttribOffset += 2;
            glEnableVertexAttribArray(i);
            break;

        case GXPLY_Bones:
        case GXPLY_Weights:
            glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, plyFile->elements[0].sStride, vertexAttribOffset * sizeof(float));
            vertexAttribOffset += 4;
            glEnableVertexAttribArray(i);
            break;

        }
    }
    i ^= i;

    // Destroy the PLY File
    {
        // Depopulate all elements
        for (size_t i = 0; i < plyFile->nElements; i++)
        {
            // Depopulate all properties of an element
            for (size_t j = 0; j < plyFile->elements[i].nProperties; j++)
            {
               // Free the name of the property
               free(plyFile->elements[i].properties[j].name);

               // Zero set the typesize
               plyFile->elements[i].properties[j].typeSize = 0;
            }

            // Free the properties
            free(plyFile->elements[i].properties);
            
            // Free the name of the element
            free(plyFile->elements[i].name);

            // Zero set all the primatives
            plyFile->elements[i].nCount = 0;
            plyFile->elements[i].nProperties = 0;
            plyFile->elements[i].sStride = 0;
        }

        // Free the elements
        free(plyFile->elements);

        // Zero set all the primatives
        plyFile->flags = 0;
        plyFile->format = 0;
        plyFile->nElements = 0;

        // Free the plyFile
        free(plyFile);
    }

    free(correctedIndicies);
    free(data);

    // Count up properties
    return part;

    // Error handling
    {
        noFile:
            gPrintError("[G10] [PLY] Failed to load file %s\n", path);
            return 0;
        noPart:
            gPrintError("[G10] [PLY] Null pointer provided for parameter part in call to %s\n", __FUNCSIG__);
            return 0;
        noPath:
            gPrintError("[G10] [PLY] Null pointer provided for parameter path in call to %s\n", __FUNCSIG__);
            return 0;
        invalidHeader:
            gPrintError("[G10] [PLY] Invalid header detected in file \"%s\"\n",path);
            return 0;
        noDoubleSupport:
            gPrintError("[G10] [PLY] Double detected in element \"%s\" in file \"%s\"\n", plyFile->elements[j].name, path);
            return 0;
        unrecognizedPropertyType:
            gPrintError("[G10] [PLY] Unrecognized property type detected in file \"%s\"\n", path);
            return 0;
        nonTriangulated:
            gPrintError("[G10] [PLY] Detected non triangulated faces in file \"%s\"\n", path);
            return 0;
    }
}
