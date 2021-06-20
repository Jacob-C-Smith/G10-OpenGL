#include <G10/GXNetworking.h>

GXServer_t* createServer()
{
	// Initialized data
	GXServer_t* ret = malloc(sizeof(GXServer_t));
	
	// Zero set the struct
	ret->name    = (void*)0;
	ret->socket  = 0;
	ret->IP.host = 0;
	ret->IP.port = 0;
	ret->result  = 0;

	return ret;
}

GXServer_t* loadServer(const char path[])
{
	GXServer_t* ret = createServer();
	return ret;
}

GXServer_t* loadServerAsJSON(char* token)
{
	GXServer_t* ret = createServer();
	return ret;
}

int connect(GXServer_t* server, char* host, u16 port)
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
		printf("[G10] [Networking] Failed to resolve \"%s\":%hu. Attempting reconnect %d/%d\r",host,port, (int)retries,MAX_RETRIES);
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
		printf("[G10] [Networking] Failed to open a socket. Attempting reconnect %d/%d\r", (int)retries, MAX_RETRIES);
		SDL_Delay(100);
		goto retrySocket;
	}

	#ifndef NDEBUG
		printf("[G10] [Networking] Connected to \"%s\" aka %u.%u.%u.%u:%u\n", host, a, b, c, d, p);
	#endif
	
	return 0;
	maxRetries:
	#ifndef NDEBUG
		printf("\n[G10] [Networking] Maximum retries reached\n");
	#endif
	return 0;
}

int sendCommand(GXServer_t* server, char* command)
{
	// Initialized data
	size_t commandLen = strlen(command);

	// Send the command via TCP, then compare the sent bytes to the length of the command. If everything 
	// went according to plan, commandLen = sent bytes. If so, server result = 1. Else, result = 0.
	server->result = (commandLen == (SDLNet_TCP_Send(server->socket, command, commandLen))) ? 1 : 0;

	// And just return the result
	return server->result;
}

int destroyServer(GXServer_t* server)
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
