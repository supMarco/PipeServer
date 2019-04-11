#pragma once
#include "includes.h"

BOOL load_file(BYTE *filepath, BYTE **buffer)
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
void dword_to_aob_64(DWORD64 dword, BYTE *bytes)
{
	*(DWORD64 *)bytes = dword;
}
#endif

void dword_to_aob_32(DWORD dword, BYTE *bytes)
{
	*(DWORD *)bytes = dword;
}