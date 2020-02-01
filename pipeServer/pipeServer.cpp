#include "stdafx.h"

#define BUILD64
#include "aobscan.h"

#define STR_SIZE 256
#define PIPE_NAME "\\\\.\\pipe\\pipeServer"

_declspec(dllexport) void pipeServerRun();
_declspec(dllexport) BOOL init();
_declspec(dllexport) void pipeMessageBox(BYTE*);

const char* captionMBA = "pipeMessageBox Example";
BOOL initSuccess = FALSE;
HANDLE hPipe = NULL;
BYTE * inputBuffer;
DWORD bytesWritten = 0;
DWORD bytesRead = 0;
#ifdef BUILD64
DWORD64 serverLocation = 0;
#else
DWORD serverLocation = NULL;
#endif

_declspec(dllexport) void pipeServerRun()
{
	if ((inputBuffer = (BYTE*)calloc(1, STR_SIZE)))
	{
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

						if (strstr((const char*)inputBuffer, "init"))
						{
							initSuccess = init();
#ifdef BUILD64
							serverLocation = (DWORD64)(&pipeServerRun);
							WriteFile(hPipe, &serverLocation, sizeof(DWORD64), &bytesWritten, NULL); //Sends info to the server (server location in the target)
#else
							serverLocation = (DWORD)(&pipeServerRun);
							WriteFile(hPipe, &serverLocation, sizeof(DWORD), &bytesWritten, NULL); //Sends info to the server (server location in the target)
#endif

						}
					}
					else
					{
						if (strstr((const char*)inputBuffer, "pipeMessageBox")) //Fetch the message to spawn from the client
						{
							if (ReadFile(hPipe, inputBuffer, STR_SIZE - 1, &bytesRead, NULL))
							{
								pipeMessageBox(inputBuffer);
							}
						}
					}
				}
			}
			DisconnectNamedPipe(hPipe);
		}
	}
}


BOOL init()
{
	return TRUE;
}

void pipeMessageBox(BYTE * message)
{
	MessageBoxA(NULL, (LPCSTR)message, "PipeServer", NULL);
}

BOOL injectDll(BYTE * dllpath)
{
	return (BOOL)LoadLibraryA((LPCSTR)dllpath);
}