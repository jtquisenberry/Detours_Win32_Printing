// Detours_TextOut_2024.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#pragma comment(lib, "detoured.lib")
#pragma comment(lib, "detours.lib")

//#include <cstdio>
#include <stdio.h>
#include <windows.h>
#include "detours.h"

int main()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    si.cb = sizeof(STARTUPINFO);
    wchar_t DirPath[MAX_PATH] = { 0 };
    char DirPathA[MAX_PATH] = { 0 };
    char DLLPath[MAX_PATH] = { 0 };
    wchar_t DetourPath[MAX_PATH] = { 0 };
    wchar_t AppPath[MAX_PATH] = { 0 };
    GetCurrentDirectory(MAX_PATH, DirPath);
    GetCurrentDirectoryA(MAX_PATH, DirPathA);
    sprintf_s(DLLPath, MAX_PATH, "%s\\HookTextOut.dll", DirPathA);
    //swprintf_s(DetourPath, MAX_PATH, L"%s\\detoured.dll", DirPath);
    //sprintf_s(AppPath, MAX_PATH, L"%s\\Example.exe", DirPath);
    swprintf_s(AppPath, MAX_PATH, L"%s\\WordPad.exe", DirPath);

    printf("DLLPath %s\n", DLLPath);
    wprintf((LPWSTR)L"DirPath: %s\n", DirPath);
    wprintf((LPWSTR)L"AppPath: %s\n", AppPath);
    
    
    DetourCreateProcessWithDll(L"WordPad.exe", AppPath, NULL, NULL, true, CREATE_DEFAULT_ERROR_MODE,
        NULL, NULL, &si, &pi, DLLPath, NULL);



    return 0;
}
