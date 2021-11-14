#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL_net.h>

#include <G10/GXtypedef.h>
#include <G10/GXEntity.h>
#include <G10/GXTransform.h>

#define MAX_RETRIES 32

struct GXServer_s
{
    // Uninitialized data
    char     *name;
    IPaddress IP;
    TCPsocket socket;
    int       result;
};

enum GXCommands_e {
    COMMAND_NOOP = 0x0000,
    COMMAND_CONNECT = 0x0077,
    COMMAND_DISPLACE_ROTATE = 0x7700,
    COMMAND_CHAT = 0x00FF,
    COMMAND_DISCONNECT = 0xFF00
};

typedef enum GXCommands_e GXCommands_t;

GXServer_t *createServer          ( void );                                                       // ✅ Creates an empty server connection

GXServer_t *loadServer            ( const char  path[] );                                         // ✅ Loads a server from a file
GXServer_t *loadServerAsJSON      ( char       *token );                                          // ✅ Creates a server from text

int         connect               ( GXServer_t *server, char *host   , u16    port );             // ✅ Connects to an IP/Port
int         sendCommand           ( GXServer_t *server, char *command, size_t len );              // ✅ Send some data down the line

int         sendConnectCommand    ( GXServer_t *server, char *name   , char          **party );   // ✅ TODO
int         sendTextChat          ( GXServer_t *server, char *message, GXCommands_t    channel ); // ✅ TODO
int         sendDisconnectCommand ( GXServer_t *server );                                         // ✅ TODO

int         sendDisplaceOrientCommand     ( GXServer_t *server, GXEntity_t *entity );

int         destroyServer         ( GXServer_t *server );                                         // ✅ Destroys a server