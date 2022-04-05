#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <G10/GXtypedef.h>
#include <G10/GXLinear.h>
#include <G10/GXEntity.h>

struct GXCollision_s
{
	GXEntity_t           *a,
		                 *b;
	size_t                begin_tick,
		                  end_tick;
	vec3                  a_in_b,
		                  b_in_a,
		                  normal;
	float                 depth;
	bool                  has_collision,
		                  has_aabb_collision;
	struct GXCollision_s *next;
};

// Allocators
GXCollision_t *create_collision               ( );

// Constructors
GXCollision_t *create_collision_from_entities ( GXEntity_t *a, GXEntity_t *b );

int            update_collision               ( GXCollision_t *collision );

int            resolve_collision              ( GXCollision_t *collision );

// Destructors
int            destroy_collision              ( GXCollision_t *collision );