#pragma once
#include "includes.h"

BOOL init_ofna_dll(OPENFILENAMEA * pofna, BYTE * path, DWORD pathsize)
{
	if (pofna)
	{
		pofna->lStructSize = sizeof(OPENFILENAMEA);
		pofna->lpstrFile = path;
		pofna->nMaxFile = pathsize;
		pofna->lpstrFilter = "DLL Files\0*.DLL\0\0";
		pofna->nFilterIndex = 1;
		pofna->Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	}
	return pofna;
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