#include "stdafx.h"
#include <stdlib.h>

#pragma region defines
#define STRSIZE 256
#define EXENAME "DKII.EXE"
#define PIPENAME "\\\\.\\pipe\\Test"
#define MONEYOFFSET 0x7E
#pragma endregion

#pragma region prototypes
_declspec(dllexport) void PipeServerStart(); //This is the only function I need to export
BOOL init();
void toggle_freeze_money();
void add_money(DWORD);
void spawn_in_game_cheat(DWORD);
bool Compare(const BYTE*, const BYTE*, const char*);
DWORD Pattern(DWORD, DWORD, BYTE *, const char *);
#pragma endregion

#pragma region locals
BOOL initSuccess = FALSE;
BOOL moneyFlag = FALSE;
DWORD moneyHook = NULL;
DWORD moneyVal = NULL;
DWORD playerHook = NULL;
DWORD playerBase = NULL;
DWORD cheatSelector = NULL;
#pragma endregion



_declspec(dllexport) void PipeServerStart()
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
	DWORD vpTemp = NULL;
	DWORD GameModule = (DWORD)GetModuleHandleA(EXENAME);

	if (GameModule)
	{
		moneyHook = Pattern(GameModule, 0x7fffffffffff, (BYTE *)"\x74\x03\x89\x6F\x7E", "xxxxx"); //Fetches the Hook location in the target
		playerHook = Pattern(GameModule, 0x7fffffffffff, (BYTE *)"\x8B\x85\x82\x00\x00\x00", "xxxxxx"); //Fetches the Hook location in the target
		if (moneyHook && playerHook)
		{
			if (VirtualProtect((LPVOID)moneyHook, 0x64, PAGE_EXECUTE_READWRITE, &vpTemp)) //Makes the page that contains the code I want to modify writeable
			{
				if (VirtualProtect((LPVOID)playerHook, 0x64, PAGE_EXECUTE_READWRITE, &vpTemp)) //As above
				{
					//BASIC CODE INJECTION EXAMPLE//

					if (FALSE) //Creating my own cave lol
					{
						//[cave start]
						__asm {
						    getPlayer:
							mov [playerBase],ebp
							mov eax, dword ptr [ebp+0x82]
							ret
						}
						//[cave end]
					}

					//[setup jumps start]
					__asm {
						mov eax, dword ptr [playerHook]
						mov byte ptr [eax],0xE8 //0xE8 = call
						mov ebx, getPlayer //getPlayer = caveAddr
						sub ebx, dword ptr[playerHook]
						sub ebx, 5
						mov[eax+1], ebx //call caveAddr - hookLocation - 5
						mov byte ptr [eax+1+4], 0x90 //nop
					}
					//[setup jumps end]
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

void toggle_freeze_money()
{
	__asm {
	  cmp byte ptr [moneyFlag],0
	  jne l_disable
	  mov eax, [moneyHook]
	  test eax,eax
	  je l_exitFM
      mov byte ptr [eax], 0xEB //Changes "je" (0x74) to "jmp" (0xEB)
	  mov byte ptr [moneyFlag], 1
	  jmp l_exitFM
	  l_disable:
	  mov eax, [moneyHook]
	  test eax,eax
	  je l_exitFM
	  mov byte ptr [eax], 0x74 //Changes "jmp" (0xEB) back to "je" (0x74)
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
	DWORD gamemodule = (DWORD)GetModuleHandleA(EXENAME);
	__asm {
		mov eax, dword ptr [gamemodule]
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

//AOB Scanning Functions

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
