#pragma comment(lib, "detoured.lib")
#pragma comment(lib, "detours.lib")

#undef UNICODE
#include <cstdio>
#include <windows.h>
#include <detours.h>

int main()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    si.cb = sizeof(STARTUPINFO);
	char DirPath[MAX_PATH] = {0};
    char DLLPath[MAX_PATH] = {0};
    char DetourPath[MAX_PATH] = {0};
	char AppPath[MAX_PATH] = {0};
	GetCurrentDirectory(MAX_PATH, DirPath);
    sprintf_s(DLLPath, MAX_PATH, "%s\\ShellHookDLL.dll", DirPath);
    sprintf_s(DetourPath, MAX_PATH, "%s\\detoured.dll", DirPath);
    //sprintf_s(AppPath, MAX_PATH, "%s\\Example.exe", DirPath);
	sprintf_s(AppPath, MAX_PATH, "%s\\WordPad.exe", DirPath);
	DetourCreateProcessWithDll(NULL, AppPath, NULL,
        NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
        &si, &pi, DetourPath, DLLPath, NULL);
    return 0;
}
