#include "stdafx.h"

#define STR_SIZE 256
#define PIPE_NAME "\\\\.\\pipe\\pipeServer"

_declspec(dllexport) void pipe_server_start(); //This is the only function I need to export
BOOL init();
void pipe_message_box(BYTE *);

BOOL initSuccess = FALSE;
HANDLE hPipe = NULL;
BYTE * inputBuffer;
DWORD bytesWritten = NULL;
DWORD bytesRead = NULL;
#ifdef BUILD64
DWORD64 serverLocation = NULL;
#else
DWORD serverLocation = NULL;
#endif
const char * captionMBA = "pipe_message_box Example";

_declspec(dllexport) void pipe_server_start()
{
	inputBuffer = (BYTE *)calloc(1, STR_SIZE);

	hPipe = CreateNamedPipe(TEXT(PIPE_NAME), PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 1024 * 16, 1024 * 16, INFINITE, NULL);

	if (hPipe != INVALID_HANDLE_VALUE)
	{
		if (ConnectNamedPipe(hPipe, NULL)) //Waits for a client to connect
		{
			while (ReadFile(hPipe, inputBuffer, STR_SIZE - 1, &bytesRead, NULL)) //This will constantly read the client's "command" I will send
			{
				inputBuffer[bytesRead] = '\0'; //We will call a certain DLL function accortding to the "command" 

				if (!initSuccess) //init() has to be called first, and just once!
				{

					if (!strcmp((const char *)inputBuffer, "init"))
					{
						initSuccess = init();
#ifdef BUILD64
						serverLocation = (DWORD64)(&pipe_server_start);
						WriteFile(hPipe, &serverLocation, sizeof(DWORD64), &bytesWritten, NULL); //Sends info to the server (server location in the target)
#else
						serverLocation = (DWORD)(&pipe_server_start);
						WriteFile(hPipe, &serverLocation, sizeof(DWORD), &bytesWritten, NULL); //Sends info to the server (server location in the target)
#endif
						
					}
				}
				else
				{
					if (!strcmp((const char *)inputBuffer, "pipe_message_box")) //Fetch the message to spawn from the client
					{
						ReadFile(hPipe, inputBuffer, STR_SIZE - 1, &bytesRead, NULL);
						pipe_message_box(inputBuffer);
					}
				}
			}
		}
		DisconnectNamedPipe(hPipe);
	}
}


BOOL init()
{
	return TRUE;
}

void pipe_message_box(BYTE * message)
{
	MessageBoxA(NULL, (LPCSTR)message, "test", NULL);
}