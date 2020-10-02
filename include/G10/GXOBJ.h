#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <G10/GXDebug.h>

#include <G10/GXtypedef.h>
#include <G10/GXMesh.h>

GXmesh_t* loadOBJMesh (const char path[]); // ✅ Loads an OBJ file from path and returns a pointer to GXmesh_t
