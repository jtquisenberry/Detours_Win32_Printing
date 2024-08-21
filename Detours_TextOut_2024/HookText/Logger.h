#include "StringsAndDirectories.h"
#include <iostream>
#include <windows.h>
#include <sstream>
#include <fstream>
#include <string>

#pragma once


// Logger: handles receiving and then writing logs to a file
//===========================================================

class Logger
{
private:
    HANDLE hFile;

public:
    typedef std::ostream& (*ManipFn)(std::ostream&);
    std::wstring m_strFQLogFilename;
    FILE* m_fileLogFile;

    Logger(std::wstring strLogFile=L"")
    {
        m_strFQLogFilename = strLogFile;
    }

    /**
     * Retrieves the fully qualified path for the file that contains the current 
     * module.The module must have been loaded by the current process.
     * 
     * @param null.
     * @return void.
     */
    void SetPathExePath()
    {
        wchar_t exeName[MAX_PATH] = { 0 };
        GetModuleFileName(NULL, exeName, MAX_PATH);

        std::wstring strPath;
        std::wstring strFilename;
        std::wstring strFilenameNoExtension;
        std::wstring strExtension;
        int bytes = GetModuleFileName(NULL, exeName, MAX_PATH);
        GetPathAndFilenameW(exeName, &strPath, &strFilename, &strFilenameNoExtension, &strExtension);

        m_strFQLogFilename = strPath;
    }

    /**
     * Retrieves the fully qualified path for the file that contains the current
     * module.The module must have been loaded by the current process.
     *
     * @param null.
     * @return void.
     */
    void SetPathUserProfileA(std::string strFilename="")
    {
        char* base_path;
        size_t len;
        errno_t err = _dupenv_s(&base_path, &len, "USERPROFILE");
        char str_ObjectsPath[260];
        memset(&str_ObjectsPath[0], 0, sizeof(str_ObjectsPath));
        strcat_s(str_ObjectsPath, 260, base_path);
        strcat_s(str_ObjectsPath, 260, strFilename.c_str());        
    }

    void SetPathUserProfile(std::wstring strFilename=L"")
    {
        wchar_t* base_path;
        size_t len;
        errno_t err = _wdupenv_s(&base_path, &len, L"USERPROFILE");
        std::wstring ws(base_path);
        // MessageBox(NULL, L"Title", ws.c_str(), MB_OK);

        //wchar_t str_ObjectsPath[260];
        //memset(&str_ObjectsPath[0], 0, sizeof(str_ObjectsPath));
        //wcscat_s(str_ObjectsPath, 260, base_path);
        //wcscat_s(str_ObjectsPath, 260, strFilename.c_str());

        ws += L"\\" + strFilename;

        MessageBox(NULL, L"Title", ws.c_str(), MB_OK);
        m_strFQLogFilename = ws;
        MessageBox(NULL, L"Title", m_strFQLogFilename.c_str(), MB_OK);

        //std::wstring m_strFQLogFilename(str_ObjectsPath);
    }

    void Open()
    {
        m_fileLogFile = _wfopen(m_strFQLogFilename.c_str(), L"a+");
    }

    void Close()
    {
        fclose(m_fileLogFile);
    }
};


