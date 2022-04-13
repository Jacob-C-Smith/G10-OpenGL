#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL_net.h>

#include <G10/GXtypedef.h>
#include <G10/GXCameraController.h>
#include <G10/GXEntity.h>
#include <G10/GXTransform.h>

#define MAX_RETRIES         32    // How many retries should be attempted before disconnecting
#define MAX_COMMANDS        64    // How many commands can be stored in a queue at one time
#define MAX_DATA_PER_PACKET 4096  // 4096 bytes maximum
#define PACKET_INTERVAL     10    //
#define MAX_BANDWIDTH       65536 // 64 KB/s

struct GXServer_s
{
    // General data
    char     *name;
    IPaddress ip;
    TCPsocket socket;
    
    // Command queues
    GXCommand_t **input,
                **output;
    char         *input_data,
                 *output_data;
    size_t        input_front, 
                  input_rear,
                  output_front,
                  output_rear;
};


enum GXChatChannels_e {
    CHAT_PARTY   = 0xFF,
    CHAT_TEAM    = 0xF0,
    CHAT_ALL     = 0x0F,
    CHAT_COMMAND = 0x00
};

// Allocators
GXServer_t  *create_server             ( void );                                                   // ✅ Creates an empty server connection
GXCommand_t *create_command            ( void );

// Constructors
GXServer_t  *load_server               ( const char  path[] );                                     // ✅ Loads a server from a file
GXServer_t  *load_server_as_json       ( char       *token );                                      // ✅ Creates a server from text


int          connect                   ( GXServer_t *server  , char       *host   , u16    port ); // ✅ Connects to an IP/Port

// Command processing
char*        process_chat_command      ( char *command );

// Command queue
int          enqueue_command           ( GXServer_t *server, GXCommand_t *command );               // X Places command in the outbound queue
GXCommand_t* dequeue_command           ( GXServer_t *server );                                     // X Removes a command from the in queue

// Command queue processing
int          flush_commands            ( GXServer_t *server );                                     // X Send all commands
int          recv_commands             ( GXServer_t *server );                                     // X Get commands

// Command processing
int          parse_command            ( GXCommand_t *command );                                    // X Parse command
int          parse_chat               ( struct chat_s chat );                                      // X Parse a chat command

// Deallocators
int          destroy_server           ( GXServer_t *server );                                     // ✅ Destroys a server