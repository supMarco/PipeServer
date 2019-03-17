#pragma once
#include "includes.h"

WORD get_machine_type(BYTE * exe);
DWORD get_exported_functions(BYTE *, BYTE **);
DWORD rva_to_file_offset(BYTE *, DWORD);