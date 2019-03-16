#pragma once
#include "includes.h"

BOOL init_ofna_dll(OPENFILENAMEA *, BYTE *);
HANDLE get_process_handle_by_name(BYTE *);