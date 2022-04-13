#include <G10/GXNetworking.h>

GXServer_t *create_server     ( )
{
	// Initialized data
	GXServer_t* ret = calloc(1,sizeof(GXServer_t));
	
    // Check the memory
	{
		#ifndef NDEBUG
			if (ret == 0)
				goto no_mem;
		#endif
	}

    return ret;

    // Error handling
    {

		// Standard library errors
		{
	        no_mem:
		    #ifndef NDEBUG
			    g_print_error("[Standard library] Failed to allocate memory in call to funciton \"%s\".\n", __FUNCSIG__);
			#endif
			return 0;
		}
    }
}

GXServer_t *load_server       ( const char  path[] )
{
	// TODO: Argument check

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

	// TODO: Error handling
}

GXServer_t *load_server_as_json ( char       *token )
{
	// TODO: Argument check

	// Initialized data
	GXServer_t  *ret        = create_server();
	size_t       len        = strlen(token);
    size_t       token_count = parse_json(token, len, 0, (void*)0);
    JSONToken_t *tokens     = calloc(token_count, sizeof(JSONToken_t));

    // Parse the camera object
    parse_json(token, len, token_count, tokens);

    // Copy relevent data for a server object
	for ( size_t l = 0; l < token_count; l++ )
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
