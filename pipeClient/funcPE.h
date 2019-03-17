#pragma once
#include "includes.h"

WORD get_machine_type(BYTE *);
DWORD get_exported_functions_x64_x86(BYTE *, BYTE **, WORD);
DWORD rva_to_file_offset_x64_x86(BYTE *, DWORD, WORD);