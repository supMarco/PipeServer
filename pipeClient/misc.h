#pragma once
#include "includes.h"

BOOL stopscan;

HANDLE get_process_handle_by_name(BYTE *);
BOOL load_file(BYTE *, BYTE **);
#ifdef BUILD64
void dword_to_aob_64(DWORD64, BYTE *);
#endif
void dword_to_aob_32(DWORD, BYTE *);