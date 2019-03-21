#pragma once
#include "includes.h"


HANDLE get_process_handle_by_name(BYTE *);
BOOL load_file(BYTE *, BYTE **);
#ifdef BUILD64
void dword_to_aob_64(DWORD64, BYTE *);
#endif
void dword_to_aob_32(DWORD, BYTE *);
void code_cave_scanner(HANDLE hProcess, DWORD64 startaddress, DWORD64 stopaddress, DWORD64 size, BOOL includeReadOnlyNonExecutable, HWND listbox, HWND percentage);
void update_progress(HWND, DWORD64);