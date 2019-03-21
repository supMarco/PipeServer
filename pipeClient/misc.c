#pragma once
#include "includes.h"

BOOL stopscan = FALSE;

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

void code_cave_scanner(struct _codecavescanner *param)
{
	MEMORY_BASIC_INFORMATION memory_basic_information = { NULL };
	DWORD64 current = NULL;
	DWORD64 bytecounter = NULL;
	DWORD64 bytesread = NULL;
	BYTE currentbyte = NULL;
	BYTE *buffer = NULL;
	BYTE found[8*2+1] = { 0 };
	BOOL validregion = FALSE;

	SendMessage(param->listbox, LB_RESETCONTENT, NULL, NULL);

	update_progress(param->percentage, 0);
	current = param->startaddress;
	
	while ((current < param->stopaddress) && !stopscan)
	{
		update_progress(param->percentage, current);
		VirtualQueryEx(param->hProcess, current, &memory_basic_information, sizeof(memory_basic_information));

		switch (param->includeReadOnlyNonExecutable)
		{
		case TRUE:
			validregion = ((memory_basic_information.AllocationProtect && (PAGE_EXECUTE || PAGE_EXECUTE_READ || PAGE_EXECUTE_READWRITE || PAGE_READONLY || PAGE_EXECUTE_WRITECOPY)));
			break;

		case FALSE:
			validregion = ((memory_basic_information.AllocationProtect && (PAGE_EXECUTE || PAGE_EXECUTE_READ || PAGE_EXECUTE_READWRITE || PAGE_EXECUTE_WRITECOPY)));
			break;
		}

		if (!validregion)
		{
			current += (DWORD64)memory_basic_information.RegionSize;
			continue;
		}

		buffer = malloc(memory_basic_information.RegionSize);

		bytecounter = currentbyte = 0;

		if (ReadProcessMemory(param->hProcess, memory_basic_information.BaseAddress, buffer, memory_basic_information.RegionSize, bytesread))
		{
			for (int i = 0; i < memory_basic_information.RegionSize; i++)
			{
				if (buffer[i] == currentbyte) bytecounter++;
				else
				{
					currentbyte = buffer[i];
					bytecounter = 1;
				}
				if (bytecounter == param->size)
				{
					_i64toa(((DWORD64)(memory_basic_information.BaseAddress) + i - bytecounter + 1), found, 16);
					_strupr(found);
					SendMessage(param->listbox, LB_ADDSTRING, NULL, found);
				}
			}
		}
		current += (DWORD64)memory_basic_information.RegionSize;
		free(buffer);
	}
	if (stopscan)
	{
		EnableWindow(param->stopbutton, TRUE);
		stopscan = FALSE;
	}
	free(param);
}

void update_progress(HWND label, DWORD64 curraddr)
{
	BYTE strval[8*2+13] = "scanning... ";
	_i64toa(curraddr, strval+12, 16);
	SetWindowText(label, strval);
}