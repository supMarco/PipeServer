#include "includes.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

void on_window_create(HWND);
void on_button01_click();
void on_button02_click();
void on_button03_click();
void on_button04_click();
void on_button05_click();


HWND hwndMain = NULL;
HWND hwndStaticMain01 = NULL;
HWND hwndStaticMain02 = NULL;
HWND hwndStaticMain03 = NULL;
HWND hwndStaticMain04 = NULL;
HWND hwndStaticMain05 = NULL;
HWND hwndStaticMain06 = NULL;
HWND hwndButtonMain01 = NULL;
HWND hwndButtonMain02 = NULL;
HWND hwndButtonMain03 = NULL;
HWND hwndButtonMain04 = NULL;
HWND hwndButtonMain05 = NULL;
HWND hwndEditMain01 = NULL;
HWND hwndEditMain02 = NULL;
HWND hwndEditMain03 = NULL;
HWND hwndEditMain04 = NULL;
HWND hwndEditMain05 = NULL;
HWND hwndEditMain06 = NULL;
HWND hwndListMain01 = NULL;
HWND hwndListMain02 = NULL;

BYTE dllPath[STR_SIZE] = { 0 };
BYTE exePath[STR_SIZE] = { 0 };
BYTE exeName[STR_SIZE] = { 0 };
BYTE serverLocationStr[STR_SIZE] = { 0 };

BOOL initSuccess = FALSE;
#ifdef BUILD64
DWORD64 serverLocation = FALSE;
DWORD64 bytesWritten = NULL;
DWORD64 bytesRead = NULL;
#else
DWORD serverLocation = FALSE;
DWORD bytesWritten = NULL;
DWORD bytesRead = NULL;
#endif
WORD exeMachine = NULL;
WORD dllMachine = NULL;

HANDLE hProcess = NULL;
HANDLE hPipe = NULL;

BYTE errorMessage01[] = "failed injecting the DLL";
BYTE errorMessage02[] = "failed executing the function";
BYTE errorMessage03[] = "unknown error during injection";
BYTE errorMessage04[] = "failed starting the server";
BYTE successMessage01[] = "DLL injected";

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	BYTE className[] = "mainWindowClass";
	MSG msg;

	WNDCLASS wndClass = { 0 };
	wndClass.hInstance = hInst;
	wndClass.lpszClassName = className;
	wndClass.lpfnWndProc = WindowProc;
	wndClass.hbrBackground = (HBRUSH)GetSysColorBrush(COLOR_3DFACE);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&wndClass);

	hwndMain = CreateWindow(className, "Pipe Client - Disconnected from server", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 700, 380, NULL, NULL, hInst, NULL);

	if (hwndMain)
	{
		ShowWindow(hwndMain, nCmdShow);
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		on_window_create(hwnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_BUTTON01_MAIN:
			on_button01_click();
			return 0;
		case ID_BUTTON02_MAIN:
			on_button02_click();
			return 0;
		case ID_BUTTON03_MAIN:
			on_button03_click();
			return 0;
		case ID_BUTTON04_MAIN:
			on_button04_click();
			return 0;
		case ID_BUTTON05_MAIN:
			on_button05_click();
			return 0;
		}
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}



void on_window_create(HWND hwnd)
{
	hwndButtonMain01 = CreateWindow("Button", "Inject and connect", WS_CHILD | WS_VISIBLE, 10, 10, 150, 60, hwnd, (HMENU)ID_BUTTON01_MAIN, NULL, NULL);
	hwndButtonMain02 = CreateWindow("Button", "Call function", WS_CHILD | WS_VISIBLE, 10, 300, 280, 25, hwnd, (HMENU)ID_BUTTON02_MAIN, NULL, NULL);
	hwndButtonMain03 = CreateWindow("Button", "...", WS_CHILD | WS_VISIBLE, 645, 10, 30, 25, hwnd, (HMENU)ID_BUTTON03_MAIN, NULL, NULL);
	hwndButtonMain04 = CreateWindow("Button", "Scan for codecaves", WS_CHILD | WS_VISIBLE, 485, 300, 190, 25, hwnd, (HMENU)ID_BUTTON04_MAIN, NULL, NULL);
	hwndButtonMain05 = CreateWindow("Button", "Stop", WS_CHILD | WS_VISIBLE, 395, 300, 80, 25, hwnd, (HMENU)ID_BUTTON05_MAIN, NULL, NULL);
	hwndStaticMain01 = CreateWindow("Static", "Functions exported by pipeServer.dll", WS_CHILD | WS_VISIBLE | SS_LEFT, 15, 90, 300, 20, hwnd, (HMENU)ID_LABEL01_MAIN, NULL, NULL);
	hwndStaticMain02 = CreateWindow("Static", "Machine:", WS_CHILD | WS_VISIBLE | SS_LEFT, 320, 50, 300, 20, hwnd, (HMENU)ID_LABEL02_MAIN, NULL, NULL);
	hwndStaticMain04 = CreateWindow("Static", "?", WS_CHILD | WS_VISIBLE | SS_LEFT, 390, 50, 35, 20, hwnd, (HMENU)ID_LABEL04_MAIN, NULL, NULL);
	hwndStaticMain03 = CreateWindow("Static", "Server address:", WS_CHILD | WS_VISIBLE | SS_LEFT, 455, 50, 300, 20, hwnd, (HMENU)ID_LABEL03_MAIN, NULL, NULL);
	hwndStaticMain05 = CreateWindow("Static", "?", WS_CHILD | WS_VISIBLE | SS_LEFT, 570, 50, 100, 20, hwnd, (HMENU)ID_LABEL05_MAIN, NULL, NULL);
	hwndStaticMain06 = CreateWindow("Static", "", WS_CHILD | WS_VISIBLE | SS_LEFT, 395, 90, 300, 20, hwnd, (HMENU)ID_LABEL06_MAIN, NULL, NULL);
	hwndEditMain01 = CreateWindow("Edit", "DLL path", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_AUTOHSCROLL, 170, 10, 470, 25, hwnd, (HMENU)ID_EDIT01_MAIN, NULL, NULL);
	hwndEditMain02 = CreateWindow("Edit", "EXE target", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 170, 45, 130, 25, hwnd, (HMENU)ID_EDIT02_MAIN, NULL, NULL);
	hwndEditMain03 = CreateWindow("Edit", "Argument #1", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 10, 270, 280, 25, hwnd, (HMENU)ID_EDIT03_MAIN, NULL, NULL);
	hwndEditMain04 = CreateWindow("Edit", "Start address", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 395, 270, 110, 25, hwnd, (HMENU)ID_EDIT04_MAIN, NULL, NULL);
	hwndEditMain05 = CreateWindow("Edit", "Stop address", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 510, 270, 110, 25, hwnd, (HMENU)ID_EDIT05_MAIN, NULL, NULL);
	hwndEditMain06 = CreateWindow("Edit", "Size", WS_CHILD | WS_VISIBLE | WS_BORDER, 625, 270, 50, 25, hwnd, (HMENU)ID_EDIT06_MAIN, NULL, NULL);
	hwndListMain01 = CreateWindow("ListBox", NULL, WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_DISABLENOSCROLL | LBS_STANDARD, 10, 115, 280, 155, hwnd, (HMENU)ID_LIST01_MAIN, NULL, NULL);
	hwndListMain02 = CreateWindow("ListBox", NULL, WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_DISABLENOSCROLL | LBS_STANDARD, 395, 115, 280, 155, hwnd, (HMENU)ID_LIST02_MAIN, NULL, NULL);
	SendMessage(hwndEditMain04, EM_SETLIMITTEXT, 8 * 2, NULL);
	SendMessage(hwndEditMain05, EM_SETLIMITTEXT, 8 * 2, NULL);
	SendMessage(hwndEditMain06, EM_SETLIMITTEXT, 8 * 2, NULL);
}

void on_button01_click()
{

#ifdef BUILD64
	DWORD64 numberOfExportedFunctions = NULL;
#else
	DWORD numberOfExportedFunctions = NULL;
#endif
	DWORD starterIndex = NULL;
	BYTE * serverExportedFunctions[STR_SIZE] = { NULL };
	BYTE * dllBuffer = NULL;
	BYTE * exeBuffer = NULL;

	if (!load_file(dllPath, &dllBuffer))
		return;

	dllMachine = get_machine_type(dllBuffer);

	GetWindowText(hwndEditMain02, (LPSTR)exeName, sizeof(exeName) - 1);

	if (!(hProcess = get_process_handle_by_name(exeName)))
	{
		free(dllBuffer);
		return;
	}

	if (!GetModuleFileNameEx(hProcess, NULL, exePath, sizeof(exePath) - 1))
	{
		free(dllBuffer);
		return;
	}

	if (!load_file(exePath, &exeBuffer))
		return;

	exeMachine = get_machine_type(exeBuffer);

	if (exeMachine != dllMachine)
	{
		free(exeBuffer);
		free(dllBuffer);
		return;
	}
	numberOfExportedFunctions = get_exported_functions_x64_x86(dllBuffer, serverExportedFunctions, dllMachine);

	for (unsigned int i = 0; i < numberOfExportedFunctions && serverExportedFunctions[i]; i++)
	{
		if (strstr((char const *)serverExportedFunctions[i], (const char *)PIPE_SERVER_STARTER)) starterIndex = i;
		SendMessage(hwndListMain01, LB_ADDSTRING, NULL, (LPARAM)serverExportedFunctions[i]);
	}
	free(exeBuffer);
	free(dllBuffer);

	if (!inject_and_start_server(serverExportedFunctions[starterIndex]))
	{
		MessageBox(hwndMain, errorMessage03, "Error", MB_ICONERROR | MB_OK);
		return;
	}

	if ((hPipe = CreateFile(TEXT(PIPE_NAME), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE)
	{
		MessageBox(hwndMain, errorMessage04, "Error", MB_ICONERROR | MB_OK);
		SendMessage(hwndListMain01, LB_RESETCONTENT, NULL, NULL);
		return;
	}

	SetWindowText(hwndMain, "Pipe Client - Connected to server");
	EnableWindow(hwndButtonMain01, FALSE);
}

void on_button02_click()
{
	DWORD selFunctionIndex = NULL;
	DWORD selFunctionLen = NULL;
	BYTE selFunction[STR_SIZE] = { 0 };
	BYTE message[STR_SIZE] = { 0 };

	if ((selFunctionIndex = SendMessage(hwndListMain01, LB_GETCURSEL, NULL, NULL)) == LB_ERR) return;
	selFunctionLen = SendMessage(hwndListMain01, LB_GETTEXTLEN, (WPARAM)selFunctionIndex, NULL);
	SendMessage(hwndListMain01, LB_GETTEXT, selFunctionIndex, (LPARAM)selFunction);
	if (!initSuccess)
	{
		if (strstr((char * const)selFunction, (const char *)"init"))
		{
			WriteFile(hPipe, selFunction, STR_SIZE - 1, (LPDWORD)&bytesWritten, NULL);
			ReadFile(hPipe, &serverLocation, sizeof(DWORD64), (LPDWORD)&bytesRead, NULL); //Fetch info from the server (server location in the target)
			//Grabbing exe info from the server
			_i64toa(serverLocation, serverLocationStr, 16);
			_strupr(serverLocationStr);
			if (exeMachine == IMAGE_NT_OPTIONAL_HDR64_MAGIC) SetWindowText(hwndStaticMain04, "PE64");
			else SetWindowText(hwndStaticMain04, "PE32");
			SetWindowText(hwndStaticMain05, serverLocationStr);
			initSuccess = TRUE;
			MessageBox(hwndMain, "Ready to start", "Pipe Client", MB_OK | MB_ICONINFORMATION);
			return;
		}
	}
	else
	{
		if (strstr((char * const)selFunction, (const char *)"pipe_message_box"))
		{
			GetWindowText(hwndEditMain03, message, STR_SIZE - 1);
			WriteFile(hPipe, selFunction, STR_SIZE - 1, (LPDWORD)&bytesWritten, NULL);
			WriteFile(hPipe, message, STR_SIZE - 1, (LPDWORD)&bytesWritten, NULL); //Sends the message to spawn to the server
			return;
		}
	}
}

void on_button03_click()
{
	OPENFILENAMEA ofna = { 0 };
	ofna.lStructSize = sizeof(OPENFILENAMEA);
	ofna.lpstrFile = (LPSTR)dllPath;
	ofna.nMaxFile = sizeof(dllPath);
	ofna.lpstrFilter = "DLL Files\0*.DLL\0\0";
	ofna.nFilterIndex = 1;
	ofna.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (!GetOpenFileNameA(&ofna))
		return;
	SetWindowText(hwndEditMain01, dllPath);
}

void on_button04_click()
{
	BYTE startaddressstr[STR_SIZE] = { NULL };
	BYTE stopaddressstr[STR_SIZE] = { NULL };
	BYTE sizestr[STR_SIZE] = { NULL };

	DWORD64 startaddress = NULL;
	DWORD64 stopaddress = NULL;
	DWORD64 size = NULL;
	BOOL includeReadOnlyNonExecutable = FALSE;

	GetWindowText(hwndEditMain04, startaddressstr, STR_SIZE - 1);
	GetWindowText(hwndEditMain05, stopaddressstr, STR_SIZE - 1);
	GetWindowText(hwndEditMain06, sizestr, STR_SIZE - 1);
	sscanf((LPCTSTR)startaddressstr, "%I64X", &startaddress);
	sscanf((LPCTSTR)stopaddressstr, "%I64X", &stopaddress);
	sscanf((LPCTSTR)sizestr, "%I64X", &size);

	struct _codecavescanner *param = malloc(sizeof(struct _codecavescanner));
	param->hProcess = hProcess;
	param->startaddress = startaddress;
	param->stopaddress = stopaddress;
	param->size = size;
	param->includeReadOnlyNonExecutable = includeReadOnlyNonExecutable;
	param->listbox = hwndListMain02;
	param->stopbutton = hwndButtonMain05;
	param->percentage = hwndStaticMain06;

	if (size) CreateThread(NULL, NULL, &code_cave_scanner, (LPVOID)param, NULL, NULL);
	else MessageBox(hwndMain, "Insert a proper size", "Error", MB_ICONERROR | MB_OK);
}

void on_button05_click()
{
	if (!stopscan)
	{
		stopscan = TRUE;
		EnableWindow(hwndButtonMain05, FALSE);
	}
}

BOOL inject_and_start_server(BYTE * functiontocall)
{

	BYTE injectBuffer[DEFAULT_ALLOCATION_SIZE] = { 0 };
	BOOL outcome = FALSE;
	HANDLE hThread = NULL;
#ifdef BUILD64
	DWORD64 functionLocation = NULL;
	DWORD64 injectionLocation = NULL;
	DWORD64 tId = NULL;
	DWORD64 pLoadLibrary = NULL;
	DWORD64 pGetProcAddress = NULL;
	DWORD64 startaddress = NULL;
	DWORD64 position = NULL;
	DWORD64 position2 = NULL;
	DWORD64 threadExitCode = NULL;
#else
	DWORD functionLocation = NULL;
	DWORD injectionLocation = NULL;
	DWORD tId = NULL;
	DWORD pLoadLibrary = NULL;
	DWORD pGetProcAddress = NULL;
	DWORD startaddress = NULL;
	DWORD position = NULL;
	DWORD position2 = NULL;
	DWORD threadExitCode = NULL;
#endif

#ifdef BUILD64
	if (!(injectionLocation = (DWORD64)VirtualAllocEx(hProcess, NULL, DEFAULT_ALLOCATION_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)))
		return outcome;
#else
	if (!(injectionLocation = (DWORD)VirtualAllocEx(hProcess, NULL, DEFAULT_ALLOCATION_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE)))
		return outcome;
#endif

	position = injectionLocation;

	memcpy(injectBuffer, dllPath, strlen((const char*)dllPath) + 1);
	position += strlen((const char*)dllPath) + 1;
	position2 += strlen((const char*)dllPath) + 1;

	functionLocation = position;

	memcpy(injectBuffer + position2, functiontocall, strlen((const char *)functiontocall) + 1);
	position += strlen((const char *)functiontocall) + 1;
	position2 += strlen((const char *)functiontocall) + 1;

	startaddress = position;

	if (exeMachine == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
#ifdef BUILD64
		dword_to_aob_64(0x28EC8348, injectBuffer + position2); //SUB RSP, 0x28
		position += 4;
		position2 += 4;

		dword_to_aob_64(0xB948, injectBuffer + position2); //MOV RCX
		position += 2;
		position2 += 2;

		dword_to_aob_64(injectionLocation, injectBuffer + position2); //IMM64
		position += 8;
		position2 += 8;
#endif
	}
	else
	{
		injectBuffer[position2] = 0x68; //PUSH
		position++;
		position2++;

		dword_to_aob_32(injectionLocation, injectBuffer + position2); //IMM32
		position += 4;
		position2 += 4;
	}

	if (exeMachine == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
#ifdef BUILD64
		dword_to_aob_64(0x08EB0000000215FF, injectBuffer + position2); //CALL ABSOLUTE
		position += 8;
		position2 += 8;
#endif
	}
	else
	{
		injectBuffer[position2] = 0xE8; //CALL RELATIVE
		position++;
		position2++;
	}

	if (exeMachine == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
#ifdef BUILD64
		pLoadLibrary = (DWORD64)GetProcAddress(GetModuleHandle("kernel32"), "LoadLibraryA");
		dword_to_aob_64(pLoadLibrary, injectBuffer + position2); //ABS64
		position += 8;
		position2 += 8;

		dword_to_aob_64(0x28C48348, injectBuffer + position2); //ADD RSP, 0x28
		position += 4;
		position2 += 4;
#endif
	}
	else
	{
		pLoadLibrary = (DWORD)GetProcAddress(GetModuleHandle("kernel32"), "LoadLibraryA");
		dword_to_aob_32(pLoadLibrary - (position - 1) - 5, injectBuffer + position2); //REL32
		position += 4;
		position2 += 4;
	}

	if (exeMachine == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
#ifdef BUILD64
		dword_to_aob_64(0xC08548, injectBuffer + position2); //TEST RAX,RAX
		position += 3;
		position2 += 3;
#endif
	}
	else
	{
		dword_to_aob_32(0xC085, injectBuffer + position2); //TEST EAX,EAX
		position += 2;
		position2 += 2;
	}
	injectBuffer[position2] = 0x75; //JNZ 
	position++;
	position2++;

	dword_to_aob_32(((position - 1) + 3 + 5) - (position - 1) - 2, injectBuffer + position2); //REL8
	position++;
	position2++;


	injectBuffer[position2] = 0xB8; //MOV R32
	position++;
	position2++;

	injectBuffer[position2] = 0x02; //IMM32
	position += 4;
	position2 += 4;

	injectBuffer[position2] = 0xC3; //RET
	position++;
	position2++;

	if (exeMachine == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
#ifdef BUILD64
		dword_to_aob_64(0x28EC8348, injectBuffer + position2); //SUB RSP, 0x28
		position += 4;
		position2 += 4;

		dword_to_aob_64(0xBA48, injectBuffer + position2); //MOV RDX
		position += 2;
		position2 += 2;

		dword_to_aob_64(functionLocation, injectBuffer + position2); //IMM64
		position += 8;
		position2 += 8;

		dword_to_aob_64(0xC88B48, injectBuffer + position2); //MOV RCX,RAX
		position += 3;
		position2 += 3;
#endif
	}
	else
	{
		injectBuffer[position2] = 0x68; //PUSH
		position++;
		position2++;

		dword_to_aob_32(functionLocation, injectBuffer + position2); //IMM32
		position += 4;
		position2 += 4;

		injectBuffer[position2] = 0x50; //PUSH EAX
		position++;
		position2++;
	}

	if (exeMachine == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
#ifdef BUILD64
		dword_to_aob_64(0x08EB0000000215FF, injectBuffer + position2); //CALL ABSOLUTE
		position += 8;
		position2 += 8;
#endif
	}
	else
	{
		injectBuffer[position2] = 0xE8; //CALL
		position++;
		position2++;
	}

	if (exeMachine == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
#ifdef BUILD64
		pGetProcAddress = (DWORD64)GetProcAddress(GetModuleHandle("kernel32"), "GetProcAddress");
		dword_to_aob_64(pGetProcAddress, injectBuffer + position2); //ABS64
		position += 8;
		position2 += 8;

		dword_to_aob_64(0x28C48348, injectBuffer + position2); //ADD RSP, 0x28
		position += 4;
		position2 += 4;
#endif
	}
	else
	{
		pGetProcAddress = (DWORD)GetProcAddress(GetModuleHandle("kernel32"), "GetProcAddress");
		dword_to_aob_32(pGetProcAddress - (position - 1) - 5, injectBuffer + position2); //REL32
		position += 4;
		position2 += 4;
	}

	if (exeMachine == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
#ifdef BUILD64
		dword_to_aob_64(0xC08548, injectBuffer + position2); //TEST RAX,RAX
		position += 3;
		position2 += 3;
#endif
	}
	else
	{
		dword_to_aob_32(0xC085, injectBuffer + position2); //TEST EAX,EAX
		position += 2;
		position2 += 2;
	}

	injectBuffer[position2] = 0x75; //JNZ 
	position++;
	position2++;

	dword_to_aob_32(((position - 1) + 3 + 5) - (position - 1) - 2, injectBuffer + position2); //REL8
	position++;
	position2++;


	injectBuffer[position2] = 0xB8; //MOV R32
	position++;
	position2++;

	injectBuffer[position2] = 0x03; //IMM32
	position += 4;
	position2 += 4;

	injectBuffer[position2] = 0xC3; //RET
	position++;
	position2++;
#ifdef BUILD64
	if (exeMachine == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		dword_to_aob_64(0x28EC8348, injectBuffer + position2); //SUB RSP, 0x28
		position += 4;
		position2 += 4;
	}
#endif
	dword_to_aob_32(0xD0FF, injectBuffer + position2); //CALL EAX
	position += 2;
	position2 += 2;
#ifdef BUILD64
	if (exeMachine == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
	{
		dword_to_aob_64(0x28C48348, injectBuffer + position2); //ADD RSP, 0x28
		position += 4;
		position2 += 4;
	}
#endif

	injectBuffer[position2] = 0xB8; //MOV R32
	position++;
	position2++;

	injectBuffer[position2] = 0x01; //IMM32
	position += 4;
	position2 += 4;

	injectBuffer[position2] = 0xC3; //RET
	position++;
	position2++;

	if (!WriteProcessMemory(hProcess, (LPVOID)injectionLocation, injectBuffer, position2, &bytesWritten))
		return outcome;

	if (!(hThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)startaddress, NULL, NULL, (LPDWORD)tId)))
		return outcome;

	Sleep(100);
	if (GetExitCodeThread(hThread, (LPDWORD)&threadExitCode))
	{
		switch (threadExitCode)
		{
		case 2:
			MessageBox(hwndMain, errorMessage01, dllPath, MB_ICONERROR | MB_OK);
			break;
		case 3:
			MessageBox(hwndMain, errorMessage02, dllPath, MB_ICONERROR | MB_OK);
			break;
		default:
			MessageBox(hwndMain, successMessage01, dllPath, MB_ICONINFORMATION | MB_OK);
			outcome = TRUE;
			break;
		}
	}

	return outcome;
}