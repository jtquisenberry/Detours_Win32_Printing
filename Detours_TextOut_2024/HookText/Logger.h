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
        
        // Initialization
        hFile = 0x0;
        m_fileLogFile = nullptr;

        
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
        std::string s(base_path);
        s += "\\" + strFilename;
        std::wstring ws(s.begin(), s.end());
        m_strFQLogFilename = ws;
    }

    void SetPathUserProfile(std::wstring strFilename=L"")
    {
        wchar_t* base_path;
        size_t len;
        errno_t err = _wdupenv_s(&base_path, &len, L"USERPROFILE");
        std::wstring ws(base_path);
        ws += L"\\" + strFilename;
        m_strFQLogFilename = ws;
    }

    void Open()
    {
        m_fileLogFile = _wfopen(m_strFQLogFilename.c_str(), L"a+");
    }

    void Flush()
    {
        fflush(m_fileLogFile);
    }

    void Close()
    {
        fclose(m_fileLogFile);
    }
};


