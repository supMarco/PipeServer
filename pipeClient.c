#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS

#define STRSIZE 256
#define PIPENAME "\\\\.\\pipe\\Test"

#include <Windows.h>
#include <stdio.h>

void showMainMenu();
void showMenuSIG();

int main(void)
{
	HANDLE hPipe = NULL;
	DWORD bytesWritten = NULL;
	DWORD bytesRead = NULL;
	BOOL initSuccess = FALSE;
	BOOL toggle = FALSE;
	DWORD dwVal = NULL;
	char buffer[STRSIZE];


	hPipe = CreateFile(TEXT(PIPENAME), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL); //Connects to the pipe server
	if (hPipe != INVALID_HANDLE_VALUE)
	{
		while (TRUE)
		{
			showMainMenu();

			fscanf(stdin, "%s", buffer); //Gets a "command"

			if (!strcmp(buffer, "exit"))
				break;

			WriteFile(hPipe, buffer, STRSIZE - 1, &bytesWritten, NULL); //Sends your "command" to the server
			if (!initSuccess)
			{
				if (!strcmp(buffer, "init"))
				{
					fprintf(stdout, "\n\nready to start!\n\n\n");
					initSuccess = TRUE;
				}
			}
			else
			{
				if (!strcmp(buffer, "toggle_freeze_money"))
				{
					ReadFile(hPipe, &toggle, sizeof(BOOL), &bytesRead, NULL); //Fetches the current "toggle_freeze_money" state from the server
					toggle ? fprintf(stdout, "\n\nfreeze_money ON\n\n\n") : fprintf(stdout, "\n\nfreeze_money OFF\n\n\n");
				}
				else if (!strcmp(buffer, "add_money"))
				{
					fprintf(stdout, "how many? >");
					fscanf(stdin, "%d", &dwVal);
					WriteFile(hPipe, &dwVal, sizeof(DWORD), &bytesWritten, NULL); //Sends the amount to the server
					fprintf(stdout, "\n\n+%d money!\n\n\n", dwVal);
				}
				else if (!strcmp(buffer, "spawn_in_game_cheat"))
				{
					showMenuSIG();
					fscanf(stdin, "%d", &dwVal);
					WriteFile(hPipe, &dwVal, sizeof(DWORD), &bytesWritten, NULL); //Sends the id to the server
					fprintf(stdout, "\n\ncheat %d enabled!\n\n\n", dwVal);
				}
			}		
		}
		CloseHandle(hPipe);
	}

	return (0);
}

void showMainMenu()
{
	fprintf(stdout, "current functions\n{\n"
		"0: init\n"
		"1: toggle_freeze_money\n"
		"2: add_money\n"
		"3: spawn_in_game_cheat\n"
        "}\nselect a function {by name eg. init} >");
}

void showMenuSIG()
{
	fprintf(stdout, "game cheats\n{\n"
		"0: show me the money (Get Money)\n"
		"1: now the rain has gone (Show the Map)\n"
		"2: feel the power (All Units Level 10)\n"
		"3: this is my church (Enable All Rooms)\n"
		"4: fit the best (Enable All Traps and Rooms)\n"
		"5: i believe its magic (Get All Spells)\n"
		"6: do not fear the reaper (Win Level)\n"
		"7: ha ha thisaway ha ha thataway (100K Mana)\n"
		"}\nselect a cheat {by id eg. 0 >");
}