#include <iostream>
#include <windows.h>
#include <sstream>
#include <fstream>
#include <string>

#pragma once


// Logger: handles receiving and then writing logs to a file
//===========================================================

class StreamLogger
{
private:
    std::wstringstream stream;
    HANDLE file;

public:
    typedef std::ostream& (*ManipFn)(std::ostream&);

    std::wstring m_strLogFile;

    // initialize the log file as "<executable name>_log.txt"
    StreamLogger(std::wstring strLogFile=L"")
    {
        if (strLogFile != L"")
        {
            m_strLogFile = strLogFile;
        }
        else
        {
            wchar_t exeName[MAX_PATH] = { 0 };
            // Retrieves the fully qualified path for the file that contains the specified 
            // module.The module must have been loaded by the current process.
            GetModuleFileName(NULL, exeName, MAX_PATH);
            std::wstring strExeName = std::wstring(exeName);
            m_strLogFile = strExeName + L"_log.txt";
        }

        std::wifstream fs_Log(m_strLogFile);
        if (!fs_Log.is_open())
        {
            return;
        }
    }

    // override the << operator to redirect values to our internal stream
    template <typename T>
    StreamLogger& operator <<(T const& value)
    {
        stream << value;
        return *this;
    }

    // override the << operator to detect the std::endl stream manipulation
    StreamLogger& operator<<(ManipFn manip)
    {
        if (manip == static_cast<ManipFn>(std::endl))
            this->write_log();

        return *this;
    }

    void write_log()
    {
        std::wstring log;
        DWORD logLength;

        stream << L"\n";
        log = stream.str();
        logLength = wcslen(log.c_str()) * sizeof(wchar_t);

        WriteFile(this->file, log.c_str(), logLength, NULL, NULL);

        stream.str(std::wstring()); // clear the stream
        stream.clear(); // clear errors
    }

    void close()
    {
        CloseHandle(this->file);
    }
};


