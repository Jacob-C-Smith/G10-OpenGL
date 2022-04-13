#pragma once

#include <G10/GXtypedef.h>
#include <G10/GXNetworking.h>

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

// Command generation
GXCommand_t* connect_command(char* name);
GXCommand_t* no_op_command(void);                                                   // X Generates a noop command
GXCommand_t* chat_command(enum GXChatChannels_e channel, const char* chat);        // X Generates a chat command
GXCommand_t* displace_orient_command(GXCameraController_t* camera_controller);
