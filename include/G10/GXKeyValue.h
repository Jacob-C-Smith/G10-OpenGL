#pragma once

#include <string.h>

#include <G10/GXtypedef.h>
#include <G10/G10.h>

// Contains information about what information should be passed to a shader
struct GXKeyValue_s
{
    char                *key,
                        *value;
    struct GXKeyValue_s *next;
};

GXKeyValue_t *createKeyValue ( const char   *key,  const char *value );
const char   *findValue      ( GXKeyValue_t *list, size_t      count, char *key ); // ✅ Walks through an array until a matching key / value pair is found
