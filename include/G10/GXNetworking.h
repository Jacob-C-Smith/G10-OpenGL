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
    COMMAND_NOOP            = 0x0000, // 0000 0000 0000 0000
    COMMAND_A               = 0x0033, // 0000 0000 0011 0011 
    COMMAND_B               = 0x0055, // 0000 0000 0101 0101
    COMMAND_CONNECT         = 0x0077, // 0000 0000 0111 0111
    COMMAND_C               = 0x00AA, // 0000 0000 1010 1010
    COMMAND_D               = 0x00CC, // 0000 0000 1100 1100
    COMMAND_CHAT            = 0x00FF, // 0000 0000 1111 1111
    COMMAND_X               = 0x3300, // 0011 0011 0000 0000
    COMMAND_W               = 0x5500, // 0101 0101 0000 0000
    COMMAND_DISPLACE_ROTATE = 0x7700, // 0111 0111 0000 0000
    COMMAND_Y               = 0xAA00, // 1010 1010 0000 0000
    COMMAND_Z               = 0xCC00, // 1100 1100 0000 0000
    COMMAND_DISCONNECT      = 0xFF00  // 1111 1111 0000 0000
};
typedef enum GXCommands_e GXCommands_t;

enum GXChatChannels_e {
    CHAT_PARTY   = 0xFF,
    CHAT_TEAM    = 0xF0,
    CHAT_ALL     = 0x0F,
    CHAT_COMMAND = 0x00
};
typedef enum GXChatChannels_e GXChatChannels_t;

GXServer_t *createServer              ( void );                                                         // ✅ Creates an empty server connection

GXServer_t *loadServer                ( const char  path[] );                                           // ✅ Loads a server from a file
GXServer_t *loadServerAsJSON          ( char       *token );                                            // ✅ Creates a server from text

int         connect                   ( GXServer_t *server  , char       *host   , u16    port );             // ✅ Connects to an IP/Port
int         sendCommand               ( GXServer_t *server  , char       *command, size_t len );              // ✅ Send some data down the line

int         sendConnectCommand        ( GXServer_t *server  , char       *name   , char          **party );   // ✅ TODO
int         sendTextChat              ( GXServer_t *server  , char       *message, GXCommands_t    channel ); // ✅ TODO
int         sendDisconnectCommand     ( GXServer_t *server );                                           // ✅ TODO

int         sendDisplaceOrientCommand ( GXServer_t *server  , GXEntity_t *entity );

int         recvCommand               ( GXServer_t *server  , char       *buffer , size_t len );

int         parseCommand              ( char       *commands, size_t      len );


int         destroyServer             ( GXServer_t *server );                                           // ✅ Destroys a server