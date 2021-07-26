#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL_net.h>
#include <G10/GXtypedef.h>

#define MAX_RETRIES 32

struct GXServer_s
{
    // Uninitialized data
    char     *name;
    IPaddress IP;
    TCPsocket socket;
    int       result;
};
typedef struct GXServer_s GXServer_t;

GXServer_t *createServer     ( );                                             // ✅ Creates an empty server connection
GXServer_t *loadServer       ( const char  path[] );                          // ✅ Loads a server from a file
GXServer_t *loadServerAsJSON ( char       *token );                           // ✅ Creates a server from text

int         connect          ( GXServer_t *server, char *host,   u16 port );  // ✅ Connects to an IP/Port
int         sendCommand      ( GXServer_t *server, char *command );           // ✅ Send some data down the line

int         destroyServer    ( GXServer_t *server );                          // ✅ Destroys a server