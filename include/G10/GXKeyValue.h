#pragma once

#include <G10/GXtypedef.h>
#include <string.h>

// Contains information about what information should be passed to a shader
struct GXKeyValue_s
{
	size_t key;
	char*  value;
};
typedef struct GXKeyValue_s GXKeyValue_t;

char* findValue ( GXKeyValue_t* list, size_t count, size_t key );