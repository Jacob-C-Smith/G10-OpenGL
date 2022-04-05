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


// TODO: Refactor commands to use the same type punning system as G UI.
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

struct connect_s {
    char *name;
};

struct chat_s {
    enum GXChatChannels_e  channel;
    char                  *chat_text;
};

struct displace_rotate_s
{
    float x,
          y,
          z;
    u16   h_ang,
          v_ang,
          inputs_1,
          inputs_2;
};

struct disconnect_s
{
    i32 i;
};

union GXCommands_u
{
    struct connect_s         *connect;
    struct chat_s            *chat;
    struct displace_rotate_s *displace_rotate;
    struct disconnect_s      *disconnect;
};

struct GXCommand_s
{
    size_t             len;
    enum GXCommands_e  type;
    union GXCommands_u command;
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

// Command generation
GXCommand_t *connect_command           ( char                 *name );
GXCommand_t *no_op_command             ( void );                                                   // X Generates a noop command
GXCommand_t *chat_command              ( enum GXChatChannels_e channel, const char *chat );        // X Generates a chat command
GXCommand_t *displace_orient_command   ( GXCameraController_t *camera_controller );

// Command processing
char*          process_chat_command      ( char *command );

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