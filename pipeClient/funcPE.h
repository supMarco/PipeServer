#pragma once
#include "includes.h"

WORD get_machine_type(BYTE *pe);
DWORD get_time_date_stamp(BYTE *pe);
DWORD get_exported_functions(BYTE *dll, BYTE **names, WORD machine);
DWORD rva_to_file_offset(BYTE *dll, DWORD rva, WORD machine);