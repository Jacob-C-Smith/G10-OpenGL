#include <G10/GXNetworking.h>

GXServer_t *createServer     ( )
{
	// Initialized data
	GXServer_t* ret = calloc(1,sizeof(GXServer_t));
	
    // Check the memory
    #ifndef NDEBUG
        if (ret == 0)
            goto noMem;
    #endif

    return ret;

    // Error handling
    {
        noMem:
        #ifndef NDEBUG
            gPrintError("[G10] [Server] Out of memory.\n");
        #endif
        return 0;
    }
}

GXServer_t *loadServer       ( const char  path[] )
{
	// Initialized data
    GXServer_t *ret      = 0;
    FILE       *f        = fopen(path, "rb");
    size_t      i        = 0;
    char       *data     = 0;

    // Load up the file
    i    = gLoadFile(path, 0, false);
    data = calloc(i, sizeof(u8));
    gLoadFile(path, data, false);

    // Load the server from data
    ret = loadServerAsJSON(data);

    // Free the camera
    free(data);

    return ret;   
}

GXServer_t *loadServerAsJSON ( char       *token )
{
	 // Initialized data
	GXServer_t  *ret        = createServer();
	size_t       len        = strlen(token);
    size_t       tokenCount = GXParseJSON(token, len, 0, (void*)0);
    JSONValue_t *tokens     = calloc(tokenCount, sizeof(JSONValue_t));

    // Parse the camera object
    GXParseJSON(token, len, tokenCount, tokens);

    // Copy relevent data for a server object
	for ( size_t l = 0; l < tokenCount; l++ )
	{

		// Parse out the server name
		if ( strcmp("name", tokens[l].name) == 0 )
		{

		}

		// Parse out the host
		if ( strcmp("host", tokens[l].name) == 0 )
		{
		
		}

		// Parse out the port
		if ( strcmp("port", tokens[l].name) == 0 )
		{

		}

		if ( strcmp("player name", tokens[l].name) == 0 )
		{
			
		}
	}
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
	if (SDLNet_ResolveHost(&server->IP, host, port))
	{
		if (++retries == MAX_RETRIES+1)
			goto maxRetries;
		gPrintError("[G10] [Networking] Failed to resolve \"%s\":%hu. Attempting reconnect %d/%d\r",host,port, (int)retries,MAX_RETRIES);
		SDL_Delay(100);
		goto retryConnection;
	}

	a = (server->IP.host & 0x000000FF),
	b = (server->IP.host & 0x0000FF00) >> 8,
	c = (server->IP.host & 0x00FF0000) >> 16,
	d = (server->IP.host & 0xFF000000) >> 24;
	p = (server->IP.port >> 8) | (server->IP.port << 8);

	retries = 0;
	
	retrySocket:
	server->socket = SDLNet_TCP_Open(&server->IP);
	if (!server->socket)
	{
		if (++retries == MAX_RETRIES + 1)
			goto maxRetries;
		gPrintError("[G10] [Networking] Failed to open a socket. Attempting reconnect %d/%d\r", (int)retries, MAX_RETRIES);
		SDL_Delay(100);
		goto retrySocket;
	}

	#ifndef NDEBUG
		gPrintLog("[G10] [Networking] Connected to \"%s\" aka %u.%u.%u.%u:%u\n", host, a, b, c, d, p);
	#endif
	
	return 0;
	maxRetries:
	#ifndef NDEBUG
		gPrintLog("\n[G10] [Networking] Maximum retries reached\n");
	#endif
	return 0;
}

int         sendCommand      ( GXServer_t *server, char *command, size_t len )
{
	// Initialized data
	size_t commandLen = 0;

	if (len == 0)
		commandLen = strlen(command);
	else
		commandLen = len;
	
	 

	// Send the command via TCP, then compare the sent bytes to the length of the command. If everything 
	// went according to plan, commandLen = sent bytes. If so, server result = 1. Else, result = 0.
	server->result = (commandLen == (SDLNet_TCP_Send(server->socket, command, commandLen))) ? 1 : 0;

	// And just return the result
	return server->result;
}

int sendConnectCommand(GXServer_t* server, char* name, char** party)
{
	size_t len = 1+strlen(name);

	char* buffer = calloc(2 + len, sizeof(u8));
	*(u16*)buffer = (u16)COMMAND_CONNECT;
	strncpy(&buffer[2], name, len);

	sendCommand(server, buffer, len+2);

	free(buffer);
	
	return 0;
}

int         sendTextChat     ( GXServer_t* server, char* message, GXCommands_t channel )
{
	size_t  len    = strlen(message);
	char   *buffer = calloc(len + 5, sizeof(u8));

	*(u16*)buffer  = (u16)COMMAND_CHAT;
	buffer[2]      = channel;

	strncpy(&buffer[3], message, len);
	sendCommand(server, buffer, 0);
	free(buffer);

	return 0;
}

int sendDisconnectCommand ( GXServer_t *server ) 
{
	size_t  len    = 4;
	char   *buffer = calloc(len, sizeof(u8));

	*(u16*)buffer = (u16)COMMAND_DISCONNECT;
	
	sendCommand(server, buffer, len);
	free(buffer);

	return 0;
}

int sendDisplaceOrientCommand(GXServer_t* server, GXEntity_t* entity)
{
	size_t  len = 2 + sizeof(float) * 3;
	char   *buffer = calloc(len, sizeof(u8));

	*(u16*)buffer = (u16)COMMAND_DISPLACE_ROTATE;

	*(float*)&buffer[2]  = entity->transform->location.x,
	*(float*)&buffer[6]  = entity->transform->location.y,
	*(float*)&buffer[10] = entity->transform->location.z;

	sendCommand(server, buffer, len);

	free(buffer);
	 
	return 0;
}

int         destroyServer    ( GXServer_t *server )
{
	// Zero set everything
	server->name    = (void*)0;
	server->IP.host = 0;
	server->IP.port = 0;
	server->result  = 0;

	// Close the socket
	SDLNet_TCP_Close(server->socket);
	server->socket = 0;

	// Free the server
	free(server);

	return 0;
}
