#include "includes.h"

#define STR_SIZE 256
#define DEFAULT_ALLOCATION_SIZE 4096
#define DLL_NAME "pipeServer.dll"
#define PIPE_NAME "\\\\.\\pipe\\pipeServer"

BOOL inject_and_start_server(BYTE *, BYTE *, BYTE *);
void dword_to_aob(DWORD, BYTE *);
void show_main_menu();

BOOL initSuccess = FALSE;
DWORD serverLocation = FALSE;

int main(void)
{
	OPENFILENAMEA OFNA = { NULL };
	HANDLE hPipe = NULL, hDll = NULL;
	DWORD bytesWritten = NULL, bytesRead = NULL, dwVal = NULL, dllSize = NULL, numberOfExportedFunctions = NULL;
	BYTE dllPath[STR_SIZE] = { 0 }, buffer[STR_SIZE], yn = 'N', *dllBuffer = NULL, *exportedFunctions[STR_SIZE] = { NULL }, exeName[STR_SIZE] = { 0 };


	if (init_ofna_dll(&OFNA, dllPath, STR_SIZE))
	{
		fprintf(stdout, "~>`injecting and starting the pipe server, do you want me to continue`?(Y/N)\n~>");
		fscanf(stdin, "%c", &yn);
		if (yn == 'Y' || yn == 'y') fprintf(stdout, "\n\n~>`looking for " DLL_NAME "'s path`...");
		if ((yn == 'Y' || yn == 'y') && GetOpenFileNameA(&OFNA))
		{
			hDll = CreateFile(dllPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
			if (hDll != INVALID_HANDLE_VALUE)
			{
				dllSize = GetFileSize(hDll, NULL);
				dllBuffer = calloc(1, dllSize + 1);
				ReadFile(hDll, dllBuffer, dllSize, &bytesRead, NULL);
				numberOfExportedFunctions = get_exported_functions(dllBuffer, exportedFunctions);
				CloseHandle(hDll);
				fprintf(stdout, "\n\n[`Exported Functions List`]");
				for (int i = 0; i < numberOfExportedFunctions; i++)
				{
					fprintf(stdout, "\n***%d: %s***\n", i, exportedFunctions[i]);
				}
				fprintf(stdout, "\n\n~>`select the server starter` {by id eg. 0} ~>");
				fscanf(stdin, "%d", &numberOfExportedFunctions);
				fprintf(stdout, "\n\n~>`exe name`?~>");
				fscanf(stdin, "%s", exeName);
				if (inject_and_start_server(exeName, dllPath, exportedFunctions[numberOfExportedFunctions]))
				{
					hPipe = CreateFile(TEXT(PIPE_NAME), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, NULL, NULL); //Connects to the pipe server
					if (hPipe != INVALID_HANDLE_VALUE)
					{
						while (TRUE)
						{
							show_main_menu();

							fscanf(stdin, "%s", buffer); //Gets a "command"

							if (!strcmp(buffer, "exit"))
								break;

							WriteFile(hPipe, buffer, STR_SIZE - 1, &bytesWritten, NULL); //Sends your "command" to the server

							if (!initSuccess)
							{
								if (!strcmp(buffer, "init"))
								{
									fprintf(stdout, "\n\n~>`ready to start`!\n\n\n");
									ReadFile(hPipe, &serverLocation, sizeof(DWORD), &bytesRead, NULL); //Fetch info from the server (server location in the target)
									fprintf(stdout, "\n\n~>`server address`: %p!\n\n\n", serverLocation);
									initSuccess = TRUE;
								}
							}
							else
							{
								if (!strcmp(buffer, "pipe_message_box"))
								{
									fprintf(stdout, "\n\n~>`message to spawn`~>");
									fscanf(stdin, "%s", buffer);
									WriteFile(hPipe, buffer, STR_SIZE - 1, &bytesWritten, NULL); //Sends the message to spawn to the server
								}
							}
						}
						CloseHandle(hPipe);
					}
				}
			}
		}
	}
	return 0;
}

BOOL inject_and_start_server(BYTE * exename, BYTE * dllname, BYTE * functiontocall)
{
	BYTE injectBuffer[DEFAULT_ALLOCATION_SIZE] = { 0 };
	BOOL outcome = FALSE;
	HANDLE hProcess = get_process_handle_by_name(exename), hThread = NULL;
	DWORD functionLocation = NULL, injectionLocation = NULL, bytesRead = NULL, bytesWritten = NULL, tId = NULL;
	DWORD pLoadLibrary = NULL, pGetProcAddress = NULL, startaddress = NULL, position = NULL, position2 = NULL, threadExitCode = NULL;

	if (hProcess)
	{
		if (injectionLocation = VirtualAllocEx(hProcess, NULL, DEFAULT_ALLOCATION_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE))
		{
			position = injectionLocation;

			memcpy(injectBuffer, dllname, strlen(dllname) + 1);
			position += strlen(dllname) + 1;
			position2 += strlen(dllname) + 1;

			functionLocation = position;

			memcpy(injectBuffer+position2, functiontocall, strlen(functiontocall) + 1);
			position += strlen(functiontocall) + 1;
			position2 += strlen(functiontocall) + 1;

			startaddress = position;
			
			injectBuffer[position2] = 0x68; //PUSH
			position++;
			position2++;

			dword_to_aob(injectionLocation, injectBuffer + position2); //imm32
			position += 4;
			position2 += 4;

			injectBuffer[position2] = 0xE8; //CALL
			position++;
			position2++;

			pLoadLibrary = GetProcAddress(GetModuleHandle("kernel32"), "LoadLibraryA");
			dword_to_aob(pLoadLibrary - (position - 1) - 5, injectBuffer + position2); //rel32
			position += 4;
			position2 += 4;

			dword_to_aob(0xC085, injectBuffer + position2); //TEST EAX,EAX
			position += 2;
			position2 += 2;

			injectBuffer[position2] = 0x75; //JNZ 
			position++;
			position2++;

			dword_to_aob(((position - 1) + 3 + 5) - (position - 1) - 2, injectBuffer + position2); //rel8
			position++;
			position2++;

			injectBuffer[position2] = 0xB8; //MOV r32
			position++;
			position2++;

			injectBuffer[position2] = 0x02; //imm32
			position += 4;
			position2 += 4;

			injectBuffer[position2] = 0xC3; //RET
			position++;
			position2++;

			injectBuffer[position2] = 0x68; //PUSH
			position++;
			position2++;

			dword_to_aob(functionLocation, injectBuffer + position2); //imm32
			position += 4;
			position2 += 4;

			injectBuffer[position2] = 0x50; //PUSH EAX
			position++;
			position2++;

			injectBuffer[position2] = 0xE8; //CALL
			position++;
			position2++;

			pGetProcAddress = GetProcAddress(GetModuleHandle("kernel32"), "GetProcAddress");
			dword_to_aob(pGetProcAddress - (position - 1) - 5, injectBuffer + position2); //rel32
			position += 4;
			position2 += 4;

			dword_to_aob(0xC085, injectBuffer + position2); //TEST EAX,EAX
			position += 2;
			position2 += 2;

			injectBuffer[position2] = 0x75; //JNZ 
			position++;
			position2++;

			dword_to_aob(((position - 1) + 3 + 5) - (position - 1) - 2, injectBuffer + position2); //rel8
			position++;
			position2++;


			injectBuffer[position2] = 0xB8; //MOV r32
			position++;
			position2++;

			injectBuffer[position2] = 0x03; //imm32
			position += 4;
			position2 += 4;

			injectBuffer[position2] = 0xC3; //RET
			position++;
			position2++;

			dword_to_aob(0xD0FF, injectBuffer + position2); //CALL EAX
			position += 2;
			position2 += 2;

			injectBuffer[position2] = 0xB8; //MOV r32
			position++;
			position2++;

			injectBuffer[position2] = 0x01; //imm32
			position += 4;
			position2 += 4;

			injectBuffer[position2] = 0xC3; //RET
			position++;
			position2++;

			if (WriteProcessMemory(hProcess, injectionLocation, injectBuffer, position2, &bytesWritten))
			{
				if (hThread = CreateRemoteThread(hProcess, NULL, NULL, startaddress, NULL, NULL, tId))
				{
					Sleep(100);
					if (GetExitCodeThread(hThread, &threadExitCode))
					{
						switch (threadExitCode)
						{
						case 2:
							fprintf(stdout, "\n\n~>`failed injecting %s`.\n\n", dllname);
							break;
						case 3:
							fprintf(stdout, "\n\n~>`failed executing the function of %s`.\n\n", dllname);
							break;
						default:
								fprintf(stdout, "\n\n~>`%s injected`.\n\n", dllname);
								outcome = TRUE;
							break;
						}
					}
				}
			}
		}
		CloseHandle(hProcess);
	}	

	if (!outcome) fprintf(stdout, "~>`unknown error during injection`.\n\n");

	return outcome;
}

void dword_to_aob(DWORD dword, BYTE * bytes)
{
	*(DWORD *)bytes = dword;
}

void show_main_menu()
{
	fprintf(stdout, "[`dll functions`]\n{\n"
		"~>0: `init`\n"
		"~>1: `pipe_message_box`\n"
		"}\n`select a function` {by name eg. init} ~>");
}