#include <G10/GXCommand.h>

GXCommand_t* create_command(void)
{
	// Initialized data
	GXCommand_t* ret = calloc(1, sizeof(GXCommand_t));

	// TODO: Check memory
	return ret;
}

GXCommand_t* connect_command(char* name)
{
	GXCommand_t* ret = create_command();
	size_t       len = strlen(name);

	ret->type = COMMAND_CONNECT;
	ret->command.connect = calloc(1, sizeof(struct connect_s));
	ret->command.connect->name = calloc(len + 1, sizeof(char));

	strncpy(ret->command.connect->name, name, len);

	return ret;
}

GXCommand_t* no_op_command(void)
{
	GXCommand_t* ret = create_command();

	ret->type = COMMAND_NOOP;
	ret->len = 2;

	return ret;
}

GXCommand_t* chat_command(enum GXChatChannels_e channel, const char* chat)
{
	GXCommand_t* ret = create_command();

	size_t chat_len = strlen(chat);

	ret->type = COMMAND_CHAT;
	ret->len = 2 + chat_len + 1;

	ret->command.chat = calloc(1, sizeof(struct connect_s));
	ret->command.chat->chat_text = calloc(chat_len + 1, sizeof(char));

	strncpy(ret->command.chat->chat_text, chat, chat_len);


	return ret;
}

GXCommand_t* displace_orient_command(GXCameraController_t* camera_controller)
{
	GXCommand_t* ret = create_command();

	ret->type = COMMAND_DISPLACE_ROTATE;
	ret->len = 2 + sizeof(struct displace_rotate_s);

	// Return the command
	return ret;
}


int flush_commands ( GXServer_t* server )
{
	// Argument check
	{
		if (server == (void*)0)
			return 1; // TODO
		if (server->output == (void*)0)
			return 2; // TODO
		if (server->output[0] == (void*)0)
			return 3; // TODO 
	}

	// Initialized data
	u8          *packet   = calloc(MAX_DATA_PER_PACKET, sizeof(u8));
	size_t       packet_i = 0;
	GXCommand_t *i        = server->output[0];

	// Iterate over packets
	for ( size_t j = 0 ; server->output[j]; j++)
	{
		// If there is an underflow, the packet has been assembled
		if (server->output_front == -1 || server->output_front > server->output_rear)
			goto packet_assembled;

		// Set the command pointer
		i = server->output[j];

		// If this command is too long to fit in the packet, continue flushing the packet
		if (packet_i + i->len > MAX_DATA_PER_PACKET)
			goto packet_assembled;

		server->output_data[++packet_i] = i->type & 0xff;
		server->output_data[++packet_i] = (i->type & 0xFF00) >> 8;

		switch (i->type)
		{
			case COMMAND_NOOP:
				{

				}
			    break;
			case COMMAND_CHAT:
				{
					// Write the chat channel
					server->output_data[++packet_i] = (i->command.chat->channel & 0xFF00);
					server->output_data[++packet_i] = ( i->command.chat->channel & 0x00FF );
					char *chat = i->command.chat->chat_text;
					strncpy(&server->output_data[++packet_i], chat, strlen(chat));
					packet_i += strlen(chat);
				}
				break;
			case COMMAND_DISPLACE_ROTATE:
				{

				}
				break;
			case COMMAND_CONNECT:
				{
					char *name = i->command.connect->name;
					strncpy(&server->output_data[++packet_i], name, strlen(name));
					packet_i += strlen(name);

				}
				break;
			case COMMAND_DISCONNECT:
				{

				}
				break;
			case COMMAND_A:
			case COMMAND_B:
			case COMMAND_C:
			case COMMAND_D:
			case COMMAND_X:
			case COMMAND_W:
			case COMMAND_Y:
			case COMMAND_Z:
				break;
			default:
				break;
		}


		packet_i += i->len;
		

		server->output_front++;
	}
	packet_assembled:

	// Send the packet through the socket
	SDLNet_TCP_Send(server->socket, server->output_data+1, packet_i);
	memset(server->output, 0, sizeof(void*) * MAX_COMMANDS);
	server->output_rear = 0;
	server->output_front = 0;

	return 0;

	// Error handling
	{
		// Argument errors
		{
			noServer:
				#ifndef NDEBUG
					g_print_error("[G10] [Network] Null pointer provided for \"server\" in call to function \"%s\"\n", __FUNCSIG__ );
				#endif
				return 0;

			noCommand:
				#ifndef NDEBUG
					g_print_error("[G10] [Network] Null pointer provided for \"command\" in call to function \"%s\"\n", __FUNCSIG__ );
				#endif
				return 0;
		}

		// Queue errors
		{
			command_underflow:
				#ifndef NDEBUG
					g_print_warning("[G10] [Network] Command queue underflow, consider increasing bandwidth\n");
				#endif
				return 0;
		}
	}
}
