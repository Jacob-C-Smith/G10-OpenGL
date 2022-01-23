#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <G10/GXtypedef.h>
#include <G10/GXTexture.h>

typedef u8 RGBE[4];

GXTexture_t *load_hdr ( const char path[] );