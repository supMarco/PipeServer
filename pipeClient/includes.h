#pragma once

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef UNICODE
#undef UNICODE
#endif

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
#define PIPE_SERVER_STARTER "pipeServerRun"
#define PIPE_SERVER_INIT "init"
#define PIPE_SERVER_MB "pipeMessageBox"

#define ID_FUNCTIONS_STATIC_MAIN 1001
#define ID_PROCESSES_STATIC_MAIN 1002
#define ID_PATH_STATIC_MAIN 1003

#define ID_INJECT_BUTTON_MAIN 2001
#define ID_CALL_BUTTON_MAIN 2002
#define ID_OPEN_BUTTON_MAIN 2003
#define ID_ATTACH_BUTTON_MAIN 2004

#define ID_PATH_EDIT_MAIN 3001
#define ID_ARGUMENT_EDIT_MAIN 3002

#define ID_FUNCTIONS_LIST_MAIN 4001
#define ID_PROCESSES_LIST_MAIN 4002

#define ID_MENU_LIST02_REFRESH 5001
