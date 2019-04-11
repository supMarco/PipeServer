#pragma once
#include "includes.h"

BOOL load_file(BYTE *filepath, BYTE **buffer);
#ifdef BUILD64
void dword_to_aob_64(DWORD64 dword, BYTE *bytes);
#endif
void dword_to_aob_32(DWORD dword, BYTE *bytes);