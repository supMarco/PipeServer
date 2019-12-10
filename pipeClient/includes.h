#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <commctrl.h>

#define STR_SIZE 256

#include "funcPE.h"
#include "misc.h"
#include "proc.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' " "version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define BUILD64 //comment this out for x86

#define DEFAULT_ALLOCATION_SIZE 4096
#define DLL_NAME "pipeServer.dll"
#define PIPE_NAME "\\\\.\\pipe\\pipeServer"
#define PIPE_SERVER_STARTER "pipe_server_start"

#define ID_LABEL01_MAIN 1001
#define ID_LABEL02_MAIN 1002
#define ID_LABEL03_MAIN 1003
#define ID_LABEL04_MAIN 1004

#define ID_BUTTON01_MAIN 2001
#define ID_BUTTON02_MAIN 2002
#define ID_BUTTON03_MAIN 2003
#define ID_BUTTON04_MAIN 2004

#define ID_EDIT01_MAIN 3001
#define ID_EDIT02_MAIN 3002

#define ID_LIST01_MAIN 4001
#define ID_LIST02_MAIN 4002

#define ID_MENU_LIST02_REFRESH 5001
