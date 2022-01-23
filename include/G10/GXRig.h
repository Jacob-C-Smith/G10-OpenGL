#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <G10/GXtypedef.h>
#include <G10/GXLinear.h>
#include <G10/GXTransform.h>
#include <JSON/JSON.h>

#define PRINT_INDENT 3

struct GXBone_s
{
	char               *name;
	vec3               *head;
	vec3               *tail;
	int                 index;
	mat4               *transformation;
	bool                connected;
	struct GXBone_s    *children;
	struct GXBone_s    *next;
};

struct GXPose_s
{
	struct GXPose_s *next;
};

struct GXRig_s
{
	char     *name;
	GXBone_t *bones;
};

GXRig_t  *create_rig        ( void );                                                 // ✅ Creates an empty rig
GXPose_t *create_pose       ( void );                                                 // ✅ Creates an empty pose
GXBone_t *create_bone       ( void );                                                 // ✅ Creates an empty bone

GXRig_t  *load_rig          ( const char *path );                                     // ✅ Loads a rig from a file
GXRig_t  *load_rig_as_json  ( char       *token );                                    // ✅ Creates a rig from a JSON token

GXPose_t *load_pose         ( const char *poses );                                    // ❌ Loads a pose from a file
GXPose_t *load_pose_as_json ( const char *poses );                                    // ❌ Creates a pose from a JSON token
 
GXBone_t *load_bone         ( const char *path );                                     // ✅ Loads a bone from a file
GXBone_t *load_bone_as_json ( char       *token );                                    // ✅ Creates a bone from a JSON token

GXBone_t *find_bone         ( GXBone_t   *bone, char     *name, size_t searchDepth ); // ❌ Searches for a bone in a rig

int       print_rig         ( GXBone_t   *rig );                                      // ✅ Prints a rig

GXBone_t *remove_bone       ( GXRig_t    *rig,  GXBone_t *bone );

int       destroy_rig       ( GXRig_t    *rig );
int       destroy_bone      ( GXBone_t   *bone );

