#pragma once

#define PROC_DEFAULT 1024

struct WIN_PROCESS
{
	DWORD pid;
	char pname[STR_SIZE];
};

void getProcesses(struct WIN_PROCESS *);
HANDLE get_process_handle_by_name(BYTE *);