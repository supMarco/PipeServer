#pragma once
#include "includes.h"


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

BOOL load_file(BYTE * filepath, BYTE ** buffer)
{
	DWORD bytesRead = NULL;
	HANDLE hFile = CreateFile(filepath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		*buffer = (BYTE *)calloc(1, GetFileSize(hFile, NULL) + 1);
		ReadFile(hFile, *buffer, GetFileSize(hFile, NULL), &bytesRead, NULL);
		CloseHandle(hFile);
	}
	return bytesRead;

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

void code_cave_scanner(HANDLE hProcess, DWORD64 startaddress, DWORD64 stopaddress, DWORD64 size, BOOL includeReadOnlyNonExecutable, HWND listbox)
{
	MEMORY_BASIC_INFORMATION memory_basic_information = { NULL };
	DWORD64 current = NULL;
	DWORD64 bytecounter = NULL;
	DWORD64 bytesread = NULL;
	BYTE currentbyte = NULL;
	BYTE *buffer = NULL;
	BYTE found[8*2+1] = { 0 };
	BOOL validRegion = FALSE;

	SendMessage(listbox, LB_RESETCONTENT, NULL, NULL);

	current = startaddress;
	
	while (current < stopaddress)
	{
		VirtualQueryEx(hProcess, current, &memory_basic_information, sizeof(memory_basic_information));

		switch (includeReadOnlyNonExecutable)
		{
		case TRUE:
			validRegion = ((memory_basic_information.AllocationProtect && (PAGE_EXECUTE || PAGE_EXECUTE_READ || PAGE_EXECUTE_READWRITE || PAGE_READONLY || PAGE_EXECUTE_WRITECOPY)));
			break;

		case FALSE:
			validRegion = ((memory_basic_information.AllocationProtect && (PAGE_EXECUTE || PAGE_EXECUTE_READ || PAGE_EXECUTE_READWRITE || PAGE_EXECUTE_WRITECOPY)));
			break;
		}

		if (!validRegion)
		{
			current += (DWORD64)memory_basic_information.RegionSize;
			continue;
		}

		buffer = malloc(memory_basic_information.RegionSize);

		bytecounter = currentbyte = 0;

		if (ReadProcessMemory(hProcess, memory_basic_information.BaseAddress, buffer, memory_basic_information.RegionSize, bytesread))
		{
			for (int i = 0; i < memory_basic_information.RegionSize; i++)
			{
				if (buffer[i] == currentbyte) bytecounter++;
				else
				{
					currentbyte = buffer[i];
					bytecounter = 1;
				}
				if (bytecounter == size)
				{
					_i64toa(((DWORD64)(memory_basic_information.BaseAddress) + i - bytecounter + 1), found, 16);
					_strupr(found);
					SendMessage(listbox, LB_ADDSTRING, NULL, found);
				}
			}
		}

		current += (DWORD64)memory_basic_information.RegionSize;
		free(buffer);
	}
}