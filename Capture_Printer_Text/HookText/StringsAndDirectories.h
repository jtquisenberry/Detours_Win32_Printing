#pragma once


//#include <cstdio>
#include <stdio.h>
#include <windows.h>
#include <string>
#include <iostream>
#include <locale>
#include <codecvt>


void GetPathAndFilenameW(wchar_t* FQFilename, std::wstring* strPath, std::wstring* strFilename, std::wstring* strFilenameNoExtension, std::wstring* strExtension)
{
    std::wstring strFQFilename(FQFilename);

    int last_backslash_position = (int)strFQFilename.find_last_of(L"\\/");
    int last_dot_position = (int)strFQFilename.find_last_of(L".");
    *strPath = strFQFilename.substr(0, last_backslash_position);
    *strFilename = strFQFilename.substr(last_backslash_position + 1, (strFQFilename.length() - last_backslash_position - 1));

    if (last_dot_position > -1)
    {
        *strFilenameNoExtension = strFQFilename.substr(last_backslash_position + 1, (last_dot_position + 1) - (last_backslash_position + 1) - 1);
        *strExtension = strFQFilename.substr((last_dot_position + 1), (strFQFilename.length() - last_dot_position - 1));
    }
    else
    {
        *strFilenameNoExtension = strFQFilename.substr(last_backslash_position + 1, (strFQFilename.length() - last_backslash_position - 1));
        *strExtension = L"";
    }

    return;
}


std::string WideToAnsiString(std::wstring ws)
{
    if (ws.empty())
    {
        return std::string();
    }

    int size_needed = WideCharToMultiByte(CP_ACP, 0, &ws[0], (int)ws.size(), NULL, 0, NULL, NULL);
    std::string strAnsi(size_needed, 0);
    WideCharToMultiByte(CP_ACP, 0, &ws[0], (int)ws.size(), &strAnsi[0], size_needed, NULL, NULL);
    return strAnsi;
}