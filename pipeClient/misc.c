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