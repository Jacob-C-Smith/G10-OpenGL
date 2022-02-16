#include <G10/GXNetworking.h>

GXServer_t *create_server     ( )
{
	// Initialized data
	GXServer_t* ret = calloc(1,sizeof(GXServer_t));
	
    // Check the memory
	{
		#ifndef NDEBUG
			if (ret == 0)
				goto noMem;
		#endif
	}
    return ret;

    // Error handling
    {
        noMem:
        #ifndef NDEBUG
            g_print_error("[G10] [Server] Out of memory.\n");
        #endif
        return 0;
    }
}

GXServer_t *load_server       ( const char  path[] )
{
	// Initialized data
    GXServer_t *ret      = 0;
    FILE       *f        = fopen(path, "rb");
    size_t      i        = 0;
    char       *data     = 0;

    // Load up the file
    i    = g_load_file(path, 0, false);
    data = calloc(i, sizeof(u8));
    g_load_file(path, data, false);

    // Load the server from data
    ret = load_server_as_json(data);

    // Free the camera
    free(data);

    return ret;   
}

GXServer_t *load_server_as_json ( char       *token )
{
	 // Initialized data
	GXServer_t  *ret        = create_server();
	size_t       len        = strlen(token);
    size_t       tokenCount = parse_json(token, len, 0, (void*)0);
    JSONToken_t *tokens     = calloc(tokenCount, sizeof(JSONToken_t));

    // Parse the camera object
    parse_json(token, len, tokenCount, tokens);

    // Copy relevent data for a server object
	for ( size_t l = 0; l < tokenCount; l++ )
	{

		// Parse out the server name
		if ( strcmp("name", tokens[l].key) == 0 )
		{

		}

		// Parse out the host
		if ( strcmp("host", tokens[l].key) == 0 )
		{
		
		}

		// Parse out the port
		if ( strcmp("port", tokens[l].key) == 0 )
		{

		}
		
		if ( strcmp("player name", tokens[l].key) == 0 )
		{
			
		}
	}

	// Allocate for queues and buffers
	ret->output      = calloc(MAX_COMMANDS, sizeof(GXCommand_t*));
	ret->input       = calloc(MAX_COMMANDS, sizeof(GXCommand_t*));

	ret->output_data = calloc(MAX_DATA_PER_PACKET, sizeof(char));
	ret->input_data  = calloc(MAX_DATA_PER_PACKET, sizeof(char));
	
	return ret;
}

int         connect          ( GXServer_t *server, char *host,    u16 port )
{
	// Initialized data
	size_t retries = 0;
	u8     a,           // bits 24-31 of the IP
	       b,           // bits 16-23 of the IP
	       c,           // bits 8-15  of the IP
	       d;           // bits 0-7   of the IP
	u16    p;           // Port number

	retryConnection:
	if (SDLNet_ResolveHost(&server->ip, host, port))
	{
		if (++retries == MAX_RETRIES+1)
			goto maxRetries;
		g_print_error("[G10] [Networking] Failed to resolve \"%s\":%hu. Attempting reconnect %d/%d\r",host,port, (int)retries,MAX_RETRIES);
		SDL_Delay(100);
		goto retryConnection;
	}

	a = (server->ip.host & 0x000000FF),
	b = (server->ip.host & 0x0000FF00) >> 8,
	c = (server->ip.host & 0x00FF0000) >> 16,
	d = (server->ip.host & 0xFF000000) >> 24;
	p = (server->ip.port >> 8) | (server->ip.port << 8);

	retries = 0;
	
	retrySocket:
	server->socket = SDLNet_TCP_Open(&server->ip);
	if (!server->socket)
	{
		if (++retries == MAX_RETRIES + 1)
			goto maxRetries;
		g_print_error("[G10] [Networking] Failed to open a socket. Attempting reconnect %d/%d\r", (int)retries, MAX_RETRIES);
		SDL_Delay(100);
		goto retrySocket;
	}

	#ifndef NDEBUG
		g_print_log("[G10] [Networking] Connected to \"%s\" aka %u.%u.%u.%u:%u\n", host, a, b, c, d, p);
	#endif
	
	return 0;
	maxRetries:
	#ifndef NDEBUG
		g_print_log("\n[G10] [Networking] Maximum retries reached\n");
	#endif
	return 0;
}

GXCommand_t* create_command(void)
{
	// Initialized data
	GXCommand_t *ret = calloc(1, sizeof(GXCommand_t));

	// TODO: Check memory
	return ret;
}

GXCommand_t* connect_command(char* name)
{
	GXCommand_t* ret = create_command();
	size_t       len = strlen(name);

	ret->type                  = COMMAND_CONNECT;
	ret->command.connect       = calloc(1, sizeof(struct connect_s));
	ret->command.connect->name = calloc(len+1, sizeof(char));

	strncpy(ret->command.connect->name, name, len);

	return ret;
}

GXCommand_t* no_op_command(void)
{
	GXCommand_t *ret = create_command();

	ret->type = COMMAND_NOOP;
	ret->len  = 2;

	return ret;
}

GXCommand_t* chat_command(enum GXChatChannels_e channel, const char* chat)
{
	GXCommand_t *ret = create_command();

	size_t chat_len  = strlen(chat);

	ret->type = COMMAND_CHAT;
	ret->len  = 2 + chat_len + 1;

	ret->command.chat = calloc(1, sizeof(struct connect_s));
	ret->command.chat->chat_text = calloc(chat_len + 1, sizeof(char));

	strncpy(ret->command.chat->chat_text, chat, chat_len);


	return ret;
}

GXCommand_t* displace_orient_command(GXCameraController_t* camera_controller)
{
	GXCommand_t *ret = create_command();

	ret->type = COMMAND_DISPLACE_ROTATE;
	ret->len  = 2+sizeof(struct displace_rotate_s);

	// Allocate for a displace rotate struct
	struct displace_rotate_s *l = calloc(1, sizeof(struct displace_rotate_s));

	// Set the position vector of the camera
	l->x     = camera_controller->camera->where.x;
	l->y     = camera_controller->camera->where.y;
	l->z     = camera_controller->camera->where.z;

	// Set the vertical and horizontal angle of the camera
	l->v_ang = camera_controller->v_ang;
	l->h_ang = camera_controller->h_ang;

	// Set the command pointer to the right place
	ret->command.displace_rotate = l;

	// Return the command
	return ret;
}

char *process_chat_command ( char *command )
{
	u16   len  = *(u16*)&command[2];
	char *chat = calloc(len+1, sizeof(char));

	strncpy(chat, &command[3], len);

	g_print_log("[G10] [Network] Chat: %s\n", chat);

	return 0;
}

int enqueue_command(GXServer_t* server, GXCommand_t* command)
{
	// Argument check
	{
		#ifndef NDEBUG
			if(server == (void*)0)
				goto noServer;
			if (command == (void*)0)
				goto noCommand;
		#endif
	}

	// Error checking
	{

		// Check for overflows
		if (server->output_rear == MAX_COMMANDS - 1)
			goto command_overflow;

	}

	// Insert the command to the queue
	server->output[server->output_rear++] = command;
	
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
			command_overflow:
				#ifndef NDEBUG
					g_print_warning("[G10] [Network] Command queue overflow, consider increasing bandwidth\n");
				#endif
				return 0;
		}
	}
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

int recv_commands(GXServer_t* server)
{
	SDLNet_TCP_Recv(server->socket, server->input_data, MAX_DATA_PER_PACKET);
	return 0;
}

int destroy_server    ( GXServer_t *server )
{
	// Close the socket
	SDLNet_TCP_Close(server->socket);

	// Free the server
	free(server);

	return 0;
}
