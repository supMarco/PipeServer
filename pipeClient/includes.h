#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <psapi.h>
#include "funcPE.h"
#include "misc.h"

#pragma comment(lib,"user32.lib")
#pragma comment(lib, "Comdlg32.lib")

#define BUILD64 //comment this out for x86

#define STR_SIZE 256
#define DEFAULT_ALLOCATION_SIZE 4096
#define DLL_NAME "pipeServer.dll"
#define PIPE_NAME "\\\\.\\pipe\\pipeServer"
#define PIPE_SERVER_STARTER "pipe_server_start"

#define ID_LABEL01_MAIN 1001
#define ID_LABEL02_MAIN 1002
#define ID_LABEL03_MAIN 1003
#define ID_LABEL04_MAIN 1004
#define ID_LABEL05_MAIN 1005

#define ID_BUTTON01_MAIN 2001
#define ID_BUTTON02_MAIN 2002
#define ID_BUTTON03_MAIN 2003

#define ID_EDIT01_MAIN 3001
#define ID_EDIT02_MAIN 3002
#define ID_EDIT03_MAIN 3003

#define ID_LIST01_MAIN 4001