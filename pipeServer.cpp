#include "stdafx.h"
#include <stdlib.h>

#pragma region defines
#define STRSIZE 256
#define EXENAME "DKII.EXE"
#define PIPENAME "\\\\.\\pipe\\Test"
#define MONEYOFFSET 0x7E
#pragma endregion

#pragma region prototypes
_declspec(dllexport) void pipe_server_start(); //This is the only function I need to export
BOOL init();
void toggle_freeze_money(); //Patch example
void add_money(DWORD);
void spawn_in_game_cheat(DWORD);
void get_player(); //Injection example
void setup_call(DWORD, DWORD, DWORD);
bool Compare(const BYTE*, const BYTE*, const char*);
DWORD Pattern(DWORD, DWORD, BYTE *, const char *);
#pragma endregion

#pragma region locals
//########[Misc]#########
DWORD vpTemp = NULL;
DWORD gameModule = NULL;
//########[Hooks]########
DWORD moneyHook = NULL;
DWORD playerHook = NULL;
//######[Variables]######
DWORD moneyVal = NULL;
DWORD playerBase = NULL;
DWORD cheatSelector = NULL;
//########[Flags]########
BOOL initSuccess = FALSE;
BOOL moneyFlag = FALSE;
#pragma endregion



_declspec(dllexport) void pipe_server_start()
{
	HANDLE hPipe = NULL;
	char cheatName[STRSIZE] = { 0 };
	DWORD bytesWritten = NULL;
	DWORD bytesRead = NULL;

	hPipe = CreateNamedPipe(TEXT(PIPENAME), PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 1024 * 16, 1024 * 16, INFINITE, NULL);

	if (hPipe != INVALID_HANDLE_VALUE)
	{
		if (ConnectNamedPipe(hPipe, NULL)) //Waits for a client to connect
		{
			while (ReadFile(hPipe, cheatName, STRSIZE - 1, &bytesRead, NULL)) //This will constantly read the client's "command" I will send
			{
				cheatName[bytesRead] = '\0'; //We will call a certain DLL function accortding to the "command" 

				if (!initSuccess) //init() has to be called first, and just once!
				{

					if (!strcmp(cheatName, "init"))
					{
						initSuccess = init();
					}
				}
				else
				{
					if (!strcmp(cheatName, "toggle_freeze_money"))
					{
						toggle_freeze_money();
						WriteFile(hPipe, &moneyFlag, sizeof(BOOL), &bytesWritten, NULL); //Sends the current "toggle_freeze_money" state to the client
					}
					else if (!strcmp(cheatName, "add_money"))
					{
						ReadFile(hPipe, &moneyVal, sizeof(DWORD), &bytesRead, NULL); //Fetches the amount from the client
						add_money(moneyVal);
					}
					else if (!strcmp(cheatName, "spawn_in_game_cheat"))
					{
						ReadFile(hPipe, &cheatSelector, sizeof(DWORD), &bytesRead, NULL); //Fetches the id from the client
						spawn_in_game_cheat(cheatSelector);
					}
				}
			}
		}
		DisconnectNamedPipe(hPipe);
	}
}

BOOL init()
{
	gameModule = (DWORD)GetModuleHandleA(EXENAME);

	if (gameModule)
	{
		moneyHook = Pattern(gameModule, 0x7fffffffffff, (BYTE *)"\x74\x03\x89\x6F\x7E", "xxxxx"); //Fetches the Hook location in the target
		playerHook = Pattern(gameModule, 0x7fffffffffff, (BYTE *)"\x8B\x85\x82\x00\x00\x00", "xxxxxx"); //Fetches the Hook location in the target
		if (moneyHook && playerHook)
		{
			if (VirtualProtect((LPVOID)moneyHook, 0x64, PAGE_EXECUTE_READWRITE, &vpTemp)) //Makes the page that contains the code I want to modify writeable
			{
				if (VirtualProtect((LPVOID)playerHook, 0x64, PAGE_EXECUTE_READWRITE, &vpTemp)) //As above
				{
					setup_call((DWORD)(&get_player), playerHook, 1); //jumping from playerHook to my code in "get_player()"
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

void toggle_freeze_money() //Patch example
{
	__asm {
	  cmp byte ptr [moneyFlag],0
	  jne l_disable
	  mov eax, [moneyHook]
	  test eax,eax
	  je l_exitFM
      mov byte ptr [eax], 0xEB
	  mov byte ptr [moneyFlag], 1
	  jmp l_exitFM
	  l_disable:
	  mov eax, [moneyHook]
	  test eax,eax
	  je l_exitFM
	  mov byte ptr [eax], 0x74
	  mov byte ptr [moneyFlag], 0
	  l_exitFM:
	}
	return;
}

void add_money(DWORD val)
{
	__asm {
		mov eax, dword ptr [playerBase]
		test eax,eax
		je l_exitAM
		mov ebx, dword ptr [moneyVal]
		add dword ptr [eax+MONEYOFFSET],ebx
	l_exitAM:
	}
	return;
}

void spawn_in_game_cheat(DWORD cheat)
{
	__asm {
		mov eax, dword ptr [gameModule]
		test eax,eax
		je l_exitSIGC
		push dword ptr [cheat]
		lea ecx,[eax+0x2CEA10]
		lea eax,[eax+0xA760]
		call eax
		l_exitSIGC:
	}
	return;
}

void get_player() //Injection example
{
	__asm {
	    mov [playerBase], ebp
	    mov eax, dword ptr [ebp+0x82]
	    ret
	}
}

void setup_call(DWORD to, DWORD from, DWORD nops)
{
	__asm {
		mov eax, dword ptr [from]
		mov byte ptr [eax], 0xE8
		mov ebx, [to]
		sub ebx, dword ptr [from]
		sub ebx, 5
		mov [eax+1], ebx
		add eax, 5
		xor ecx,ecx
		l_loopSC:
		cmp ecx, dword ptr [nops]
		jae l_exitSC
		mov byte ptr [eax+ecx], 0x90
		add ecx, 1
		jmp l_loopSC
		l_exitSC:
	}
}

#pragma region Aobscan
bool Compare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
		if (*szMask == 'x' && *pData != *bMask)   return 0;
	return (*szMask) == NULL;
}

DWORD Pattern(DWORD dwAddress, DWORD dwLen, BYTE *bMask, const char * szMask)
{
	for (DWORD i = 0; i < dwLen; i++)
		if (Compare((BYTE*)(dwAddress + i), bMask, szMask))  return (DWORD)(dwAddress + i);
	return 0;
}
#pragma endregion