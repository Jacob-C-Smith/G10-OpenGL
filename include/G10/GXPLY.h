#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <G10/GXtypedef.h>
#include <G10/GXMesh.h>

// PLY Debugging
#define GXPLY_CheckTriangulation  false

// PLY header strings
#define GXPLY_HSignature          0x0A796C70 // "ply\n"
#define GXPLY_HFormat             0x6D726F66 // "form"
#define GXPLY_HComment            0x6D6D6F63 // "comm"
#define GXPLY_HElement            0x6D656C65 // "elem"
#define GXPLY_HProperty           0x706F7270 // "prop"
#define GXPLY_HEnd                0x5F646E65 // "end_"

// PLY types
#define GXPLY_char                0x72616863 // "char"
#define GXPLY_uchar               0x61686375 // "ucha"
#define GXPLY_short               0x726F6873 // "shor"
#define GXPLY_ushort              0x6F687375 // "usho"
#define GXPLY_int                 0x20746E69 // "int "
#define GXPLY_uint                0x746E6975 // "uint"
#define GXPLY_float               0x616F6C66 // "floa"
#define GXPLY_double              0x62756F64 // "doub"
#define GXPLY_list                0x7473696C // "list"

// Vertex groups that we are likely to encounter
#define GXPLY_Geometric           0x00001
#define GXPLY_Texture             0x00002
#define GXPLY_Normal              0x00004
#define GXPLY_Bitangent           0x00008
#define GXPLY_Color               0x00010
#define GXPLY_Bones               0x00020
#define GXPLY_Weights             0x00040

#define GXPLY_MAXVERTEXGROUPS     8

// Geometric
#define GXPLY_X                   0x000001
#define GXPLY_Y                   0x000002
#define GXPLY_Z                   0x000004

// Texture
#define GXPLY_S                   0x000008
#define GXPLY_T                   0x000010

// Normal
#define GXPLY_NX                  0x000020
#define GXPLY_NY                  0x000040
#define GXPLY_NZ                  0x000080

// Bitangent
#define GXPLY_BX                  0x000100
#define GXPLY_BY                  0x000200
#define GXPLY_BZ                  0x000400

// Color
#define GXPLY_R                   0x000800
#define GXPLY_G                   0x001000
#define GXPLY_B                   0x002000
#define GXPLY_A                   0x004000

// Bones
#define GXPLY_B0                  0x008000
#define GXPLY_B1                  0x010000
#define GXPLY_B2                  0x020000
#define GXPLY_B3                  0x040000

// Bone weights
#define GXPLY_BW0                 0x080000
#define GXPLY_BW1                 0x100000
#define GXPLY_BW2                 0x200000
#define GXPLY_BW3                 0x400000

enum GXPLYFileType_e {
    none   = 0,
    ASCII  = 1,
    little = 2,
    big    = 3
};
typedef enum GXPLYFileType_e GXPLYFileType_t;

struct GXPLYproperty_s
{
    size_t typeSize;
    char  *name;
};
typedef struct GXPLYproperty_s GXPLYproperty_t;

struct GXPLYelement_s
{
    size_t           nCount;
    size_t           nProperties;
    size_t           sStride;
    GXPLYproperty_t *properties;
    char            *name;
};
typedef struct GXPLYelement_s GXPLYelement_t;

struct GXPLYfile_s {
    size_t          nElements,
                    flags;
    GXPLYFileType_t format;
    GXPLYelement_t *elements;
};
typedef struct GXPLYfile_s GXPLYfile_t;

#pragma pack (push)
#pragma pack (1) // Sorry RISC platforms :(
struct GXPLYindex_s {
    u8  count;
    u32 a;
    u32 b;
    u32 c;
};
#pragma pack (pop)
typedef struct GXPLYindex_s GXPLYindex_t;

GXPart_t *loadPLY ( const char path[], GXPart_t *part ); // ✅ Loads a PLY file from path and returns a pointer to GXMesh_t
