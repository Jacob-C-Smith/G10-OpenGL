#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <G10/GXLinear.h>
#include <G10/GXTransform.h>
#include <G10/GXJSON.h>

#define PRINT_INDENT 3

struct GXBone_s
{
	char               *name;
	vec3               *head;
	vec3               *tail;
	mat4               *transformation;
	bool                connected;
	struct GXBone_s    *children;
	struct GXBone_s    *next;
};

struct GXRig_s
{
	char     *name;
	GXBone_t *bones;
};

GXRig_t  *createRig          ( void );                                                 // ✅ Creates an empty rig
GXBone_t *createBone         ( void );                                                 // ✅ Creates an empty bone

GXRig_t  *loadRig            ( const char *path );                                     // ✅ Loads a rig from a file
GXRig_t  *loadRigAsJSON      ( char       *token );                                    // ✅ Creates a rig from text

GXBone_t *loadArmiture       ( const char *path );                                     // ✅ Loads a bone from a file
GXBone_t *loadArmitureAsJSON ( char       *token );                                    // ✅ Creates a bone from text

GXBone_t *searchBone         ( GXBone_t   *bone, char     *name, size_t searchDepth ); // ❌ Searches for a bone in a rig

int       printRig           ( GXBone_t   *rig );                                      // ✅ Prints a rig

GXBone_t *removeBone         ( GXRig_t    *rig,  GXBone_t *bone );

int       destroyRig         ( GXRig_t    *rig );
int       destroyBone        ( GXBone_t   *bone );

