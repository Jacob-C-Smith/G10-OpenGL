#pragma once	

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <G10/G10.h>
#include <G10/GXtypedef.h>

// Enumeration for the types of JSON objects.
enum GXJSONValueType
{
    GXJSONobject    = 0, // Anything within a "{ ... }".
    GXJSONarray     = 1, // Any array.
    GXJSONstring    = 2, // Any string.
    GXJSONprimative = 3  // Any other value.
};
typedef enum GXJSONValueType_e GXJSONValueType_t;

// This union is used to differentiate pointers form double pointers, 
// or more simply, arrays from non arrays.
union GXJSONContent_u {
    void  *nWhere;       // Pointer to object.
    void **aWhere;       // Double pointer array of objects.
};
typedef union GXJSONContent_u GXJSONContent_t;

// Container for the 
struct JSONValue_s
{
    char              *name;    // The key.
    GXJSONValueType_t  type;    // The type.
    GXJSONContent_t    content; // Where the value appears, if type is array, then where is double pointer to text in array.
};
typedef struct JSONValue_s JSONValue_t;

int GXParseJSON ( char *text, size_t len, size_t count, JSONValue_t *where ); // ✅ Parses a JSON file and places tokens at an array of JSONValue_t* tokens. 
                                                                              //    If the function is called when the where parameter is nullptr, the
                                                                              //    function will return the number of entities that need to be allocated for.