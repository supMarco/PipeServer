#include "includes.h"

#define STR_SIZE 256
#define DEFAULT_ALLOCATION_SIZE 4096
#define DLL_NAME "pipeServer.dll"
#define PIPE_NAME "\\\\.\\pipe\\pipeServer"

BOOL inject_and_start_server(BYTE *, BYTE *, BYTE *);

#ifdef BUILD64
void dword_to_aob_64(DWORD64, BYTE *);
#endif
void dword_to_aob_32(DWORD, BYTE *);

void show_main_menu();

BOOL initSuccess = FALSE;
#ifdef BUILD64
DWORD64 serverLocation = FALSE;
DWORD64 bytesWritten = NULL;
DWORD64 bytesRead = NULL;
#else
DWORD serverLocation = FALSE;
DWORD bytesWritten = NULL;
DWORD bytesRead = NULL;
#endif

WORD exeMachineType = NULL;
WORD dllMachineType = NULL;
HANDLE hProcess = NULL;

int main(void)
{
	OPENFILENAMEA OFNA = { NULL };
	HANDLE hPipe = NULL;
	HANDLE hDll = NULL;
	HANDLE hExe = NULL;
#ifdef BUILD64
	DWORD64 dwVal = NULL;
	DWORD64 numberOfExportedFunctions = NULL;
#else
	DWORD dwVal = NULL;
	DWORD numberOfExportedFunctions = NULL;
#endif
	BYTE dllPath[STR_SIZE] = { 0 };
	BYTE buffer[STR_SIZE];
	BYTE yn = 'N';
	BYTE *dllBuffer = NULL;
	BYTE *exportedFunctions[STR_SIZE] = { NULL };
	BYTE exeName[STR_SIZE] = { 0 };
	BYTE exePath[STR_SIZE] = { 0 };
	BYTE * exeBuffer = NULL;


	if (init_ofna_dll(&OFNA, dllPath, sizeof(dllPath)))
	{
		fprintf(stdout, "injecting and starting the pipe server, do you want me to continue?(Y/N)\n~>");
		fscanf(stdin, "%c", &yn);
		if (yn == 'Y' || yn == 'y') fprintf(stdout, "\nlooking for " DLL_NAME "'s path...");
		if ((yn == 'Y' || yn == 'y') && GetOpenFileNameA(&OFNA))
		{
			hDll = CreateFile(dllPath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
			if (hDll != INVALID_HANDLE_VALUE)
			{
				dllBuffer = calloc(1, GetFileSize(hDll, NULL) + 1);
				ReadFile(hDll, dllBuffer, GetFileSize(hDll, NULL), &bytesRead, NULL);
				dllMachineType = get_machine_type(dllBuffer);
				CloseHandle(hDll);

				fprintf(stdout, "\nexe name? ~>");
				fscanf(stdin, "%s", exeName);

				if (hProcess = get_process_handle_by_name(exeName))
				{ 
					if (GetModuleFileNameEx(hProcess, NULL, exePath, sizeof(exePath) - 1))
					{
						hExe = CreateFile(exePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
						if (hExe != INVALID_HANDLE_VALUE)
						{
							exeBuffer = calloc(1, GetFileSize(hExe, NULL) + 1);
							ReadFile(hExe, exeBuffer, GetFileSize(hExe, NULL), &bytesRead, NULL);
							exeMachineType = get_machine_type(exeBuffer);
							CloseHandle(hExe);

							if (exeMachineType == dllMachineType)
							{
								numberOfExportedFunctions = get_exported_functions_x64_x86(dllBuffer, exportedFunctions, dllMachineType);
								fprintf(stdout, "\n[`Exported DLL Functions List`]");
								for (int i = 0; i < numberOfExportedFunctions; i++)
								{
									fprintf(stdout, "\n```%d: %s```\n", i, exportedFunctions[i]);
								}
								fprintf(stdout, "\n\nselect the server starter {by id eg. 0} ~>");
								fscanf(stdin, "%d", &numberOfExportedFunctions);
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
													fprintf(stdout, "\nready to start!");
#ifdef BUILD64
													ReadFile(hPipe, &serverLocation, sizeof(DWORD64), &bytesRead, NULL); //Fetch info from the server (server location in the target)
#else
													ReadFile(hPipe, &serverLocation, sizeof(DWORD), &bytesRead, NULL); //Fetch info from the server (server location in the target)
#endif
													fprintf(stdout, "\nserver address: %p", serverLocation);
													initSuccess = TRUE;
												}
											}
											else
											{
												if (!strcmp(buffer, "pipe_message_box"))
												{
													fprintf(stdout, "\nmessage to spawn ~>");
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
					CloseHandle(hProcess);
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
	HANDLE hThread = NULL;
#ifdef BUILD64
	DWORD64 functionLocation = NULL;
	DWORD64 injectionLocation = NULL;
	DWORD64 tId = NULL;
	DWORD64 pLoadLibrary = NULL;
	DWORD64 pGetProcAddress = NULL;
	DWORD64 startaddress = NULL;
	DWORD64 position = NULL;
	DWORD64 position2 = NULL;
	DWORD64 threadExitCode = NULL;
#else
	DWORD functionLocation = NULL;
	DWORD injectionLocation = NULL;
	DWORD tId = NULL;
	DWORD pLoadLibrary = NULL;
	DWORD pGetProcAddress = NULL;
	DWORD startaddress = NULL;
	DWORD position = NULL;
	DWORD position2 = NULL;
	DWORD threadExitCode = NULL;
#endif

	if (injectionLocation = VirtualAllocEx(hProcess, NULL, DEFAULT_ALLOCATION_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE))
	{
		position = injectionLocation;

		memcpy(injectBuffer, dllname, strlen(dllname) + 1);
		position += strlen(dllname) + 1;
		position2 += strlen(dllname) + 1;

		functionLocation = position;

		memcpy(injectBuffer + position2, functiontocall, strlen(functiontocall) + 1);
		position += strlen(functiontocall) + 1;
		position2 += strlen(functiontocall) + 1;

		startaddress = position;

		if (exeMachineType == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		{
#ifdef BUILD64
			dword_to_aob_64(0x28EC8348, injectBuffer + position2); //SUB RSP, 0x28
			position += 4;
			position2 += 4;

			dword_to_aob_64(0xB948, injectBuffer + position2); //MOV RCX
			position += 2;
			position2 += 2;

			dword_to_aob_64(injectionLocation, injectBuffer + position2); //IMM64
			position += 8;
			position2 += 8;
#endif
		}
		else
		{
			injectBuffer[position2] = 0x68; //PUSH
			position++;
			position2++;

			dword_to_aob_32(injectionLocation, injectBuffer + position2); //IMM32
			position += 4;
			position2 += 4;
		}

		if (exeMachineType == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		{
#ifdef BUILD64
			dword_to_aob_64(0x08EB0000000215FF, injectBuffer + position2); //CALL ABSOLUTE
			position += 8;
			position2 += 8;
#endif
		}
		else
		{
			injectBuffer[position2] = 0xE8; //CALL RELATIVE
			position++;
			position2++;
		}

		if (exeMachineType == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		{
#ifdef BUILD64
			pLoadLibrary = GetProcAddress(GetModuleHandle("kernel32"), "LoadLibraryA");
			dword_to_aob_64(pLoadLibrary, injectBuffer + position2); //ABS64
			position += 8;
			position2 += 8;

			dword_to_aob_64(0x28C48348, injectBuffer + position2); //ADD RSP, 0x28
			position += 4;
			position2 += 4;
#endif
		}
		else
		{
			pLoadLibrary = GetProcAddress(GetModuleHandle("kernel32"), "LoadLibraryA");
			dword_to_aob_32(pLoadLibrary - (position - 1) - 5, injectBuffer + position2); //REL32
			position += 4;
			position2 += 4;
		}

		if (exeMachineType == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		{
#ifdef BUILD64
			dword_to_aob_64(0xC08548, injectBuffer + position2); //TEST RAX,RAX
			position += 3;
			position2 += 3;
#endif
		}
		else
		{
			dword_to_aob_32(0xC085, injectBuffer + position2); //TEST EAX,EAX
			position += 2;
			position2 += 2;
		}
		injectBuffer[position2] = 0x75; //JNZ 
		position++;
		position2++;

		dword_to_aob_32(((position - 1) + 3 + 5) - (position - 1) - 2, injectBuffer + position2); //REL8
		position++;
		position2++;


		injectBuffer[position2] = 0xB8; //MOV R32
		position++;
		position2++;

		injectBuffer[position2] = 0x02; //IMM32
		position += 4;
		position2 += 4;

		injectBuffer[position2] = 0xC3; //RET
		position++;
		position2++;

		if (exeMachineType == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		{
#ifdef BUILD64
			dword_to_aob_64(0x28EC8348, injectBuffer + position2); //SUB RSP, 0x28
			position += 4;
			position2 += 4;

			dword_to_aob_64(0xBA48, injectBuffer + position2); //MOV RDX
			position += 2;
			position2 += 2;

			dword_to_aob_64(functionLocation, injectBuffer + position2); //IMM64
			position += 8;
			position2 += 8;

			dword_to_aob_64(0xC88B48, injectBuffer + position2); //MOV RCX,RAX
			position += 3;
			position2 += 3;
#endif
		}
		else
		{
			injectBuffer[position2] = 0x68; //PUSH
			position++;
			position2++;

			dword_to_aob_32(functionLocation, injectBuffer + position2); //IMM32
			position += 4;
			position2 += 4;

			injectBuffer[position2] = 0x50; //PUSH EAX
			position++;
			position2++;
		}

		if (exeMachineType == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		{
#ifdef BUILD64
			dword_to_aob_64(0x08EB0000000215FF, injectBuffer + position2); //CALL ABSOLUTE
			position += 8;
			position2 += 8;
#endif
		}
		else
		{
			injectBuffer[position2] = 0xE8; //CALL
			position++;
			position2++;
		}

		if (exeMachineType == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		{
#ifdef BUILD64
			pGetProcAddress = GetProcAddress(GetModuleHandle("kernel32"), "GetProcAddress");
			dword_to_aob_64(pGetProcAddress, injectBuffer + position2); //ABS64
			position += 8;
			position2 += 8;

			dword_to_aob_64(0x28C48348, injectBuffer + position2); //ADD RSP, 0x28
			position += 4;
			position2 += 4;
#endif
		}
		else
		{
			pGetProcAddress = GetProcAddress(GetModuleHandle("kernel32"), "GetProcAddress");
			dword_to_aob_32(pGetProcAddress - (position - 1) - 5, injectBuffer + position2); //REL32
			position += 4;
			position2 += 4;
		}

		if (exeMachineType == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		{
#ifdef BUILD64
			dword_to_aob_64(0xC08548, injectBuffer + position2); //TEST RAX,RAX
			position += 3;
			position2 += 3;
#endif
		}
		else
		{
			dword_to_aob_32(0xC085, injectBuffer + position2); //TEST EAX,EAX
			position += 2;
			position2 += 2;
		}

		injectBuffer[position2] = 0x75; //JNZ 
		position++;
		position2++;

		dword_to_aob_32(((position - 1) + 3 + 5) - (position - 1) - 2, injectBuffer + position2); //REL8
		position++;
		position2++;


		injectBuffer[position2] = 0xB8; //MOV R32
		position++;
		position2++;

		injectBuffer[position2] = 0x03; //IMM32
		position += 4;
		position2 += 4;

		injectBuffer[position2] = 0xC3; //RET
		position++;
		position2++;
#ifdef BUILD64
		if (exeMachineType == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		{
			dword_to_aob_64(0x28EC8348, injectBuffer + position2); //SUB RSP, 0x28
			position += 4;
			position2 += 4;
		}
#endif
		dword_to_aob_32(0xD0FF, injectBuffer + position2); //CALL EAX
		position += 2;
		position2 += 2;
#ifdef BUILD64
		if (exeMachineType == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		{
			dword_to_aob_64(0x28C48348, injectBuffer + position2); //ADD RSP, 0x28
			position += 4;
			position2 += 4;
		}
#endif

		injectBuffer[position2] = 0xB8; //MOV R32
		position++;
		position2++;

		injectBuffer[position2] = 0x01; //IMM32
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
						fprintf(stdout, "\nfailed injecting %s.", dllname);
						break;
					case 3:
						fprintf(stdout, "\nfailed executing the function of %s.", dllname);
						break;
					default:
						fprintf(stdout, "\n%s injected.", dllname);
						outcome = TRUE;
						break;
					}
				}
			}
		}
	}

	if (!outcome) fprintf(stdout, "\nunknown error during injection.");
	return outcome;
}


#ifdef BUILD64
void dword_to_aob_64(DWORD64 dword, BYTE * bytes)
{
	*(DWORD64 *)bytes = dword;
}
#endif
void dword_to_aob_32(DWORD dword, BYTE * bytes)
{
	*(DWORD *)bytes = dword;
}



void show_main_menu()
{
	fprintf(stdout, "\n[`dll functions`]\n{\n"
		"~>```0: init```\n"
		"~>```1: pipe_message_box```\n"
		"}\nselect a function {by name eg. init} ~>");
}