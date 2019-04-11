#pragma once

#define PROC_DEFAULT 1024

struct WIN_PROCESS
{
	DWORD pid;
	char pname[STR_SIZE];
};

void get_processes(struct WIN_PROCESS *procarr);
HANDLE get_process_handle_by_name(BYTE *processname);