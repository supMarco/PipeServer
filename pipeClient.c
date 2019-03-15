#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <tlhelp32.h>

#define STRSIZE 256
#define ALLOCATIONSIZE 4096
#define EXENAME "DKII.EXE"
#define DLLNAME "Dll1.dll"
#define PIPENAME "\\\\.\\pipe\\Test"

BOOL init_ofna(OPENFILENAMEA *, BYTE *);
BOOL inject_and_start_server(BYTE *, BYTE *);
HANDLE get_process_handle_by_name(BYTE *);
void dword_to_aob(DWORD, BYTE *);
void show_main_menu();
void show_menu_sig();

int main(void)
{
	OPENFILENAMEA OFNA = { 0 };
	//IMAGE_DOS_HEADER test;
	HANDLE hPipe = NULL;
	DWORD bytesWritten = NULL, bytesRead = NULL, dwVal = NULL;
	BYTE dllPath[STRSIZE] = { 0 }, buffer[STRSIZE], yn = 'N';
	BOOL initSuccess = FALSE, toggle = FALSE;

	if (init_ofna(&OFNA, dllPath, STRSIZE))
	{
		fprintf(stdout, "~>i got to inject and start the pipe server, do you want me to continue?(Y/N)\n~>");
		fscanf(stdin, "%c", &yn);
		if (yn == 'Y') fprintf(stdout, "~>looking for " DLLNAME "'s path...");
		if (yn == 'Y' && GetOpenFileNameA(&OFNA))
		{
			if (inject_and_start_server(dllPath, "?pipe_server_start@@YAXXZ"))
			{
				hPipe = CreateFile(TEXT(PIPENAME), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL); //Connects to the pipe server
				if (hPipe != INVALID_HANDLE_VALUE)
				{
					while (TRUE)
					{
						show_main_menu();

						fscanf(stdin, "%s", buffer); //Gets a "command"

						if (!strcmp(buffer, "exit"))
							break;

						WriteFile(hPipe, buffer, STRSIZE - 1, &bytesWritten, NULL); //Sends your "command" to the server
						if (!initSuccess)
						{
							if (!strcmp(buffer, "init"))
							{
								fprintf(stdout, "\n\n~>ready to start!\n\n\n");
								initSuccess = TRUE;
							}
						}
						else
						{
							if (!strcmp(buffer, "toggle_freeze_money"))
							{
								ReadFile(hPipe, &toggle, sizeof(BOOL), &bytesRead, NULL); //Fetches the current "toggle_freeze_money" state from the server
								toggle ? fprintf(stdout, "\n\n~>freeze_money ON\n\n\n") : fprintf(stdout, "\n\n~>freeze_money OFF\n\n\n");
							}
							else if (!strcmp(buffer, "add_money"))
							{
								fprintf(stdout, "~>how many? >");
								fscanf(stdin, "%d", &dwVal);
								WriteFile(hPipe, &dwVal, sizeof(DWORD), &bytesWritten, NULL); //Sends the amount to the server
								fprintf(stdout, "\n\n~>+%d money!\n\n\n", dwVal);
							}
							else if (!strcmp(buffer, "spawn_in_game_cheat"))
							{
								show_menu_sig();
								fscanf(stdin, "%d", &dwVal);
								WriteFile(hPipe, &dwVal, sizeof(DWORD), &bytesWritten, NULL); //Sends the id to the server
								fprintf(stdout, "\n\n~>cheat %d enabled!\n\n\n", dwVal);
							}
						}
					}
					CloseHandle(hPipe);
				}
			}
		}
	}
	return 0;
}

BOOL inject_and_start_server(BYTE * dllname, BYTE * functiontocall)
{
	BYTE injectBuffer[ALLOCATIONSIZE] = { 0 };
	BOOL outcome = FALSE;
	HANDLE hProcess = get_process_handle_by_name(EXENAME), hThread = NULL;
	DWORD functionLocation = NULL, injectionLocation = NULL, bytesRead = NULL, bytesWritten = NULL, tId = NULL;
	DWORD pLoadLibrary = NULL, pGetProcAddress = NULL, startaddress = NULL, position = NULL, position2 = NULL, threadExitCode = NULL;

	if (hProcess)
	{
		if (injectionLocation = VirtualAllocEx(hProcess, NULL, ALLOCATIONSIZE, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE))
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
							fprintf(stdout, "\n\n~>failed injecting %s.\n\n", dllname);
							break;
						case 3:
							fprintf(stdout, "\n\n~>failed executing the function of %s.\n\n", dllname);
							break;
						default:
								fprintf(stdout, "\n\n~>%s injected.\n\n", dllname);
								outcome = TRUE;
							break;
						}
					}
				}
			}
		}
		CloseHandle(hProcess);
	}	

	if (!outcome) fprintf(stdout, "~>unknown error during injection.\n\n");

	return outcome;
}

HANDLE get_process_handle_by_name(BYTE * processname)
{
	HANDLE hProcess = NULL;
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (hSnapshot != INVALID_HANDLE_VALUE && Process32First(hSnapshot, &processEntry))
	{
		do
		{
			if (!strcmp(processEntry.szExeFile, processname))
			{
				hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID);
				break;
			}
		} while (Process32Next(hSnapshot, &processEntry));
	}
	CloseHandle(hSnapshot);
	return hProcess;
}

void dword_to_aob(DWORD dword, BYTE * bytes)
{
	*(DWORD *)bytes = dword;
}

BOOL init_ofna(OPENFILENAMEA * pofna, BYTE * path, DWORD pathsize)
{
	if (pofna)
	{
		pofna->lStructSize = sizeof(OPENFILENAMEA);
		pofna->lpstrFile = path;
		pofna->nMaxFile = pathsize;
		pofna->lpstrFilter = DLLNAME "\0" DLLNAME "\0\0";
		pofna->nFilterIndex = 1;
		pofna->Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	}
	return pofna;
}

void show_main_menu()
{
	fprintf(stdout, "~>current functions\n{\n"
		"~>0: init\n"
		"~>1: toggle_freeze_money\n"
		"~>2: add_money\n"
		"~>3: spawn_in_game_cheat\n"
        "}\nselect a function {by name eg. init} ~>");
}

void show_menu_sig()
{
	fprintf(stdout, "~>game cheats\n{\n"
		"~>0: show me the money (Get Money)\n"
		"~>1: now the rain has gone (Show the Map)\n"
		"~>2: feel the power (All Units Level 10)\n"
		"~>3: this is my church (Enable All Rooms)\n"
		"~>4: fit the best (Enable All Traps and Rooms)\n"
		"~>5: i believe its magic (Get All Spells)\n"
		"~>6: do not fear the reaper (Win Level)\n"
		"~>7: ha ha thisaway ha ha thataway (100K Mana)\n"
		"}\nselect a cheat {by id eg. 0 ~>");
}