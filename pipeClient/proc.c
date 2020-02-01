#pragma once

#include "includes.h"

void get_processes(struct WIN_PROCESS *processes)
{
	HANDLE hProcess = NULL;
	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (hSnapshot != INVALID_HANDLE_VALUE && Process32First(hSnapshot, &processEntry))
	{
		int i = 0;
		do
		{
			strcpy(processes[i].pname, processEntry.szExeFile);
			processes[i].pid = processEntry.th32ProcessID;
			i++;
		} while (Process32Next(hSnapshot, &processEntry));
	}
	CloseHandle(hSnapshot);
}

HANDLE get_process_handle_by_name(BYTE *processname)
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