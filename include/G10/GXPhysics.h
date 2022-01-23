#pragma once

#include <stdio.h>
#include <string.h>
#include <stdio.h>

#include <G10/GXtypedef.h>
#include <G10/GXLinear.h>

#include <G10/GXEntity.h>

vec3 *summate_forces         ( vec3       *forces, size_t      forceCount); // 
int   integrate_displacement ( GXEntity_t *entity, float       deltaTime);  // 
