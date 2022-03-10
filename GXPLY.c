#include <G10/GXPLY.h>

GXPart_t *load_ply ( const char path[], GXPart_t *part )
{

    // Commentary
    {
        /* 
         * The PLY loaders job is  to turn a PLY file header into a 64 bit metadata value.
         * the function is written to recognize vertex properties and parse them into common vertex
         * groups. Each vertex property in a vertex group should appear concurrently. Each 
         * group has an associated byte code. The vertex groups metadata value is used to create a 
         * vertex array object, vertex buffer object, and element buffer object.  
         * 
         * ┌─────────────────────┬──────────────────────────────────────────────────┬──────┐
         * │ Vertex group name   │ Vertex properties as they appear in the PLY file │ Code │                                                   
         * ├─────────────────────┼──────────────────────────────────────────────────┼──────┤
         * │ Geometric vertices  │ ( x, y, z )                                      │ 0x01 │
         * │ Texture coordinates │ ( u, v ) or ( s, t )                             │ 0x02 │
         * │ Vertex normals      │ ( nx, ny, nz )                                   │ 0x04 │
         * │ Vertex bitangents   │ ( bx, by, bz )                                   │ 0x08 │
         * │ Vertex tangents     │ ( tx, ty, tz )                                   │ 0x10 │
         * │ Vertex colors       │ ( red, green, blue, alpha ) or ( r, g, b, a )    │ 0x20 │
         * │ Bone groups         │ ( b0, b1, b2, b3 )                               │ 0x40 │
         * │ Bone weights        │ ( w0, w1, w2, w3 )                               │ 0x80 │
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
         *     any comments it n_counters. After this pass, the elements are allocated for
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
         *     the vertexGroups quad word. This 64 bit number is an especially compact way to 
         *     represent both the type and position of each vertex group in the file. Each 
         *     code is packed into the 64 bit number, such that the last vertex group occupies 
         *     the eight least significant bits. For comprehension and completeness, here are a 
         *     few examples and how they should be interpreted.
         * 
         *        63......................................0
         *     A. | 00 | 00 | 00 | 00 | 00 | 01 | 04 | 20 |
         *     B. | 00 | 00 | 00 | 00 | 00 | 04 | 02 | 01 |
         *     C. | 01 | 02 | 04 | 08 | 10 | 20 | 40 | 80 |
         *       
         *     A. First is geometric, second is normals, third is colors 
         *     B. First is normals, second is texture coordinates, third is geometric
         *     C. First is geometric, second is texture coordinates, third is vertex normals, fourth is 
         *        vertex bitangents, fifth is vertex tangents, sixth is vertex bitangents, seventh is 
         *        bone groups,and eighth is bone weights.
         * 
         * After the third pass, all the metadata to describe the file data has been extracted, and 
         * the function can start making the VAO, and VBO. The last significant byte
         * of the vertex groups variable is masked off, and switch()ed against the vertex group codes. 
         * Vertex attribute pointers are created for the specific offset and stride of the vertex
         * group. The first vertex group will be at layout position 0, the second at 1, the third at 2, and soforth. 
         * 
         * 
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
    GXPLYfile_t   *plyFile            = calloc(1, sizeof(GXPLYfile_t));

    // Load the file
    i = g_load_file(path, 0, true);
    data = calloc(i, sizeof(u8));
    g_load_file(path, data, true);

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
                plyFile->n_elements++;

            // Here is as good a place as any to look for comments
            #ifndef NDEBUG
                if(*(u32*)cData == GXPLY_HComment)
                {
                    i = 0;
                    while (cData[++i] != '\n');
                    g_print_log("[G10] [PLY] Comment in file \"%s\" : %.*s\n", path, i-8, &cData[8]);
                }
            #endif

            // Zero set the index
            i = 0;
            while (cData[++i] != '\n'); // Skip to the end of the line
            cData = &cData[i + 1];      // Set the pointer
        }

        // Allocate for PLY file elements
        plyFile->elements = calloc(plyFile->n_elements+1, sizeof(GXPLYelement_t));

        // Copy the data pointer again
        cData = data;

        // Pass 2
        {
            while (*(u32*)cData != GXPLY_HEnd)
            {

                // Check if iterator is on an element
                if (*(u32*)cData == GXPLY_HElement)
                {
                    // TODO: Dynamically determine size. 
                    char *l = strchr(cData+8, ' ');
                    size_t n_len = l-(cData+8);
                    plyFile->elements[j].name = calloc(n_len+1, sizeof(u8));
                    strncpy(plyFile->elements[j].name, cData+8, n_len);

                    plyFile->elements[j].n_count = atoi(cData + 8 + n_len+1);

                    i = 0;
                    while (cData[++i] != '\n'); // Skip to the end of the line
                    cData = &cData[i + 1];      // Set the pointer

                    // Check if iterator is on a property
                    while (*(u32*)cData == GXPLY_HProperty)
                    {
                        // Increment properties
                        plyFile->elements[j].n_properties += 1;

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

                // Allocate space for the properties
            p2propertyExit:
                plyFile->elements[j].properties = calloc(plyFile->elements[j].n_properties, sizeof(GXPLYproperty_t));
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

                // Check if iterator is on an element
                if (*(u32*)cData == GXPLY_HElement)
                {
                    // TODO: Dynamically determine size. 

                    i = 0;
                    while (cData[++i] != '\n'); // Skip to the end of the line
                    cData = &cData[i + 1];      // Set the pointer

                    // Check if iterator is on a property
                    while (*(u32*)cData == GXPLY_HProperty)
                    {
                        // Increment properties
                        cData += 9;

                        // Compute stride and type size for each element and property respectively
                        switch (*(u32*)cData)
                        {
                        case GXPLY_char:
                        case GXPLY_uchar:
                            plyFile->elements[j].s_stride                += 1;
                            plyFile->elements[j].properties[k].type_size =  1;
                            break;
                        case GXPLY_short:
                        case GXPLY_ushort:
                            plyFile->elements[j].s_stride                += 2;
                            plyFile->elements[j].properties[k].type_size =  2;
                            break;
                        case GXPLY_int:
                        case GXPLY_uint:
                        case GXPLY_float:
                            plyFile->elements[j].s_stride                += 4;
                            plyFile->elements[j].properties[k].type_size =  4;
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
                for (int b = 0; b < plyFile->elements[a].n_properties; b++)
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
                    else if (strncmp(plyFile->elements[a].properties[b].name, "tx", 2) == 0)
                    {
                        plyFile->flags <<= 8;
                        plyFile->flags |= (GXPLY_Tangent);
                        tflags |= GXPLY_TX;
                        vertexGroupCount++;
                    }
                    else if (strncmp(plyFile->elements[a].properties[b].name, "ty", 2) == 0)
                        tflags |= GXPLY_TY;
                    else if (strncmp(plyFile->elements[a].properties[b].name, "tz", 2) == 0)
                        tflags |= GXPLY_TZ;
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
                        plyFile->flags |= (GXPLY_Bitangent);
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

            if (plyFile->flags & GXPLY_Tangent &&
                !(tflags & GXPLY_TX &&
                  tflags & GXPLY_TY &&
                  tflags & GXPLY_TZ))
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
            g_print_log("[G10] [PLY] Missing vertex attributes detected in \"%s\"\n",path);
            goto processVAO;
        irregularVertices:
            g_print_log("[G10] [PLY] Detected irregular vertex attribute grouping in \"%s\"\n", path);
            goto processVAO;
    }
    processVAO:
    cData = cData+11;

    // Set a few variables to construct the VAO, VBO, and EBO
    vertexArray            = (float*)cData;
    verticesInBuffer       = plyFile->elements[0].n_count * plyFile->elements[0].s_stride;
    indices                = (void*)&cData[verticesInBuffer];
    part->elements_in_buffer = ((GLuint)plyFile->elements[1].n_count * (GLuint)3);

    correctedIndicies      = calloc((size_t)part->elements_in_buffer, sizeof(u32));

    if (correctedIndicies == (void*)0)
        return (void*)0;
    
    size_t indcnt = 0;
        
    // Fixes the indices
    for (i = 0; i < plyFile->elements[1].n_count; i++)
    {
        correctedIndicies[(i * 3) + 0] = indices[i].a;
        correctedIndicies[(i * 3) + 1] = indices[i].b;
        correctedIndicies[(i * 3) + 2] = indices[i].c;
        #ifndef NDEBUG
            indcnt += indices[i].count;
            if (!(indices[i].a < indices[i].b < indices[i].c))
                g_print_warning("[G10] [PLY] Irregular indices detected in face < %d %d %d >\n", indices[i].a, indices[i].b, indices[i].c);
        #endif
    }

    #ifndef NDEBUG
        if (indcnt % 3 != 0)
            goto nonTriangulated;
    #endif

    // Create buffers
    {
        // Generate the vertex array and all of its contents, as well as the element buffer
        glGenVertexArrays(1, &part->vertex_array);
        glGenBuffers(1, &part->vertex_buffer);
        glGenBuffers(1, &part->element_buffer);

        glBindVertexArray(part->vertex_array);

        // Populate and enable the vertex buffer, element buffer, and UV coordinates
        glBindBuffer(GL_ARRAY_BUFFER, part->vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, verticesInBuffer, vertexArray, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, part->element_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, part->elements_in_buffer * sizeof(u32), correctedIndicies, GL_STATIC_DRAW);
    }
    
    for (int i = 0; vertexGroupCount > i; i++)
    {
        size_t g = (0xFF << (8*(vertexGroupCount - 1)));
        char   h = (plyFile->flags & g) >> (8 * (vertexGroupCount - 1));
        plyFile->flags <<= 8;
        switch (h)
        {
        case GXPLY_Geometric:
        case GXPLY_Normal:
        case GXPLY_Tangent:
        case GXPLY_Bitangent:
        case GXPLY_Color:
            glVertexAttribPointer(i, 3, GL_FLOAT, GL_FALSE, plyFile->elements[0].s_stride, vertexAttribOffset * sizeof(float));
            vertexAttribOffset += 3;
            glEnableVertexAttribArray(i);
            break;

        case GXPLY_Texture:
            glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, plyFile->elements[0].s_stride, vertexAttribOffset * sizeof(float));
            vertexAttribOffset += 2;
            glEnableVertexAttribArray(i);
            break;

        case GXPLY_Bones:
            glVertexAttribIPointer(i, 4, GL_INT, plyFile->elements[0].s_stride, vertexAttribOffset * sizeof(float));
            vertexAttribOffset += 4;
            glEnableVertexAttribArray(i);
            break;
        case GXPLY_Weights:
            glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, plyFile->elements[0].s_stride, vertexAttribOffset * sizeof(float));
            vertexAttribOffset += 4;
            glEnableVertexAttribArray(i);
            break;
        }
    }
    i ^= i;

    // Destroy the PLY File
    {
        // Depopulate all elements
        for (size_t i = 0; i < plyFile->n_elements; i++)
        {
            // Depopulate all properties of an element
            for (size_t j = 0; j < plyFile->elements[i].n_properties; j++)
            {
               // Free the name of the property
               free(plyFile->elements[i].properties[j].name);

               // Zero set the type_size
               plyFile->elements[i].properties[j].type_size = 0;
            }

            // Free the properties
            free(plyFile->elements[i].properties);
            
            // Free the name of the element
            free(plyFile->elements[i].name);

            // Zero set all the primatives
            plyFile->elements[i].n_count = 0;
            plyFile->elements[i].n_properties = 0;
            plyFile->elements[i].s_stride = 0;
        }

        // Free the elements
        free(plyFile->elements);

        // Zero set all the primatives
        plyFile->flags = 0;
        plyFile->format = 0;
        plyFile->n_elements = 0;

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
            g_print_error("[G10] [PLY] Failed to load file %s\n", path);
            return 0;
        noPart:
            g_print_error("[G10] [PLY] Null pointer provided for parameter \"part\" in call to %s\n", __FUNCSIG__);
            return 0;
        noPath:
            g_print_error("[G10] [PLY] Null pointer provided for parameter \"path\" in call to %s\n", __FUNCSIG__);
            return 0;
        invalidHeader:
            g_print_error("[G10] [PLY] Invalid header detected in file \"%s\"\n",path);
            return 0;
        noDoubleSupport:
            g_print_error("[G10] [PLY] Double detected in element \"%s\" in file \"%s\"\n", plyFile->elements[j].name, path);
            return 0;
        unrecognizedPropertyType:
            g_print_error("[G10] [PLY] Unrecognized property type detected in file \"%s\"\n", path);
            return 0;
        nonTriangulated:
            g_print_error("[G10] [PLY] Detected non triangulated faces in file \"%s\"\n", path);
            return 0;
    }
}

vec3* load_ply_geometric_points(const char path[], size_t* count)
{
    
    // Argument checking
    {
        #ifndef NDEBUG
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
    GXPLYfile_t   *plyFile            = calloc(1, sizeof(GXPLYfile_t));
    vec3          *ret = 0;

    // Load the file
    i = g_load_file(path, 0, true);
    data = calloc(i, sizeof(u8));
    g_load_file(path, data, true);

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
                plyFile->n_elements++;

            // Here is as good a place as any to look for comments
            #ifndef NDEBUG
                if(*(u32*)cData == GXPLY_HComment)
                {
                    i = 0;
                    while (cData[++i] != '\n');
                    g_print_log("[G10] [PLY] Comment in file \"%s\" : %.*s\n", path, i-8, &cData[8]);
                }
            #endif

            // Zero set the index
            i = 0;
            while (cData[++i] != '\n'); // Skip to the end of the line
            cData = &cData[i + 1];      // Set the pointer
        }

        // Allocate for PLY file elements
        plyFile->elements = calloc(plyFile->n_elements+1, sizeof(GXPLYelement_t));

        // Copy the data pointer again
        cData = data;

        // Pass 2
        {
            while (*(u32*)cData != GXPLY_HEnd)
            {

                // Check if iterator is on an element
                if (*(u32*)cData == GXPLY_HElement)
                {
                    // TODO: Dynamically determine size. 

                    plyFile->elements[j].name = calloc(65, sizeof(u8));
                    sscanf(cData, "element %s %d\n", plyFile->elements[j].name, &plyFile->elements[j].n_count);


                    i = 0;
                    while (cData[++i] != '\n'); // Skip to the end of the line
                    cData = &cData[i + 1];      // Set the pointer

                    // Check if iterator is on a property
                    while (*(u32*)cData == GXPLY_HProperty)
                    {
                        // Increment properties
                        plyFile->elements[j].n_properties += 1;

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

                // Allocate space for the properties
            p2propertyExit:
                plyFile->elements[j].properties = calloc(plyFile->elements[j].n_properties, sizeof(GXPLYproperty_t));
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

                // Check if iterator is on an element
                if (*(u32*)cData == GXPLY_HElement)
                {
                    // TODO: Dynamically determine size. 

                    i = 0;
                    while (cData[++i] != '\n'); // Skip to the end of the line
                    cData = &cData[i + 1];      // Set the pointer

                    // Check if iterator is on a property
                    while (*(u32*)cData == GXPLY_HProperty)
                    {
                        // Increment properties
                        cData += 9;

                        // Compute stride and type size for each element and property respectively
                        switch (*(u32*)cData)
                        {
                        case GXPLY_char:
                        case GXPLY_uchar:
                            plyFile->elements[j].s_stride                += 1;
                            plyFile->elements[j].properties[k].type_size =  1;
                            break;
                        case GXPLY_short:
                        case GXPLY_ushort:
                            plyFile->elements[j].s_stride                += 2;
                            plyFile->elements[j].properties[k].type_size =  2;
                            break;
                        case GXPLY_int:
                        case GXPLY_uint:
                        case GXPLY_float:
                            plyFile->elements[j].s_stride                += 4;
                            plyFile->elements[j].properties[k].type_size =  4;
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
                for (int b = 0; b < plyFile->elements[a].n_properties; b++)
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
            g_print_log("[G10] [PLY] Missing vertex attributes detected in \"%s\"\n",path);
            goto processVAO;
        irregularVertices:
            g_print_log("[G10] [PLY] Detected irregular vertex attribute grouping in \"%s\"\n", path);
            goto processVAO;
    }
    processVAO:
    cData = cData+11;

    vertexArray            = (float*)cData;
    verticesInBuffer       = plyFile->elements[0].n_count * plyFile->elements[0].s_stride, sizeof(char);

    i ^= i;

    ret = calloc(plyFile->elements[0].n_count, sizeof(vec3));

    for (size_t n = 0; n < plyFile->elements[0].n_count; n++)
    {

        // TODO: Vectorize
        ret[n].x = vertexArray[n * 3 + 0],
        ret[n].y = vertexArray[n * 3 + 1],
        ret[n].z = vertexArray[n * 3 + 2];
    }
    
    if (count)
        count[0] = plyFile->elements[0].n_count;

    // Destroy the PLY File
    {
        // Depopulate all elements
        for (size_t i = 0; i < plyFile->n_elements; i++)
        {
            // Depopulate all properties of an element
            for (size_t j = 0; j < plyFile->elements[i].n_properties; j++)
            {
               // Free the name of the property
               free(plyFile->elements[i].properties[j].name);

               // Zero set the type_size
               plyFile->elements[i].properties[j].type_size = 0;
            }

            // Free the properties
            free(plyFile->elements[i].properties);
            
            // Free the name of the element
            free(plyFile->elements[i].name);

            // Zero set all the primatives
            plyFile->elements[i].n_count = 0;
            plyFile->elements[i].n_properties = 0;
            plyFile->elements[i].s_stride = 0;
        }

        // Free the elements
        free(plyFile->elements);

        // Zero set all the primatives
        plyFile->flags = 0;
        plyFile->format = 0;
        plyFile->n_elements = 0;

        // Free the plyFile
        free(plyFile);
    }

    free(data);

    // Count up properties
    return ret;

    // Error handling
    {
        noFile:
            g_print_error("[G10] [PLY] Failed to load file %s\n", path);
            return 0;
        noPart:
            g_print_error("[G10] [PLY] Null pointer provided for parameter part in call to %s\n", __FUNCSIG__);
            return 0;
        noPath:
            g_print_error("[G10] [PLY] Null pointer provided for parameter path in call to %s\n", __FUNCSIG__);
            return 0;
        invalidHeader:
            g_print_error("[G10] [PLY] Invalid header detected in file \"%s\"\n",path);
            return 0;
        noDoubleSupport:
            g_print_error("[G10] [PLY] Double detected in element \"%s\" in file \"%s\"\n", plyFile->elements[j].name, path);
            return 0;
        unrecognizedPropertyType:
            g_print_error("[G10] [PLY] Unrecognized property type detected in file \"%s\"\n", path);
            return 0;
        nonTriangulated:
            g_print_error("[G10] [PLY] Detected non triangulated faces in file \"%s\"\n", path);
            return 0;
    }
}
