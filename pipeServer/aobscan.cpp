#include "stdafx.h"

BOOL Compare(const BYTE* pData, const BYTE* bMask, const char* szMask)
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