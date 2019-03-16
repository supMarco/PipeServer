#pragma once
#include "includes.h"

DWORD get_exported_functions(BYTE *, BYTE **);
DWORD rva_to_file_offset(BYTE *, DWORD);