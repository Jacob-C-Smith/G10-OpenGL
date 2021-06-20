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
    char*     name;
    IPaddress IP;
    TCPsocket socket;
    int       result;
};
typedef struct GXServer_s GXServer_t;

GXServer_t* createServer     ( );
GXServer_t* loadServer       ( const char path[] );
GXServer_t* loadServerAsJSON ( char* token );

int         connect          ( GXServer_t* server, char* host, u16 port );
int         sendCommand      ( GXServer_t* server, char* command );

int         destroyServer    ( GXServer_t* server );