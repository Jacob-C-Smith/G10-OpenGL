#ifndef debugmode

#define GXDEBUGSCENE      0x0000000000000001 // Debugger logging for scenes
#define GXDEBUGCAMERA     0x0000000000000002 // Debugger logging for cameras
#define GXDEBUGENTITY     0x0000000000000004 // Debugger logging for entities
#define GXDEBUGTEXTURE    0x0000000000000008 // Debugger logging for textures
#define GXDEBUGMESH       0x0000000000000010 // Debugger logging for meshes
#define GXDEBUGSHADER     0x0000000000000020 // Debugger logging for shaders
#define GXDEBUGMATERIAL   0x0000000000000040 // Debugger logging for materials
#define GXDEBUGTRANSFORM  0x0000000000000080 // Debugger logging for transforms
#define GXDEBUGCOMMENTS   0x0000000000000100 // Debugger logging for comments in files
#define GXDEBUGVERBOSE    0x0000000000000200 // Prints a LOT of info
#define GXDEBUGALL        0xFFFFFFFFFFFFFFFF // Debug everything

#ifdef _DEBUG
#define GXDEBUGMODE GXDEBUGALL
#else
#define GXDEBUGMODE 0x0
#endif

#endif
