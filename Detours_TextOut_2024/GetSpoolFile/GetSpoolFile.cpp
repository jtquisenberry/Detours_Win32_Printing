// GetSpoolFile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "ConfigReader.h"
#include <iostream>
//#include <Windows.h>
#include <io.h>

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <psapi.h>
#include <strsafe.h>
#include <string.h>

//#include <unistd.h>
#include <fcntl.h>
#include <sstream>


#define BUFSIZE 512

DWORD g_BytesTransferred = 0;
std::wstring printer = L"";
std::wstring spool_file_source_path = L"";
std::wstring spool_file_destination_path = L"";
std::wstring log_file_location = L"";
int job_id = 0;

VOID CALLBACK FileIOCompletionRoutine(
    __in  DWORD dwErrorCode,
    __in  DWORD dwNumberOfBytesTransfered,
    __in  LPOVERLAPPED lpOverlapped
);

VOID CALLBACK FileIOCompletionRoutine(
    __in  DWORD dwErrorCode,
    __in  DWORD dwNumberOfBytesTransfered,
    __in  LPOVERLAPPED lpOverlapped)
{
    _tprintf(TEXT("Error code:        %d\n"), dwErrorCode);
    _tprintf(TEXT("Number of bytes:   %d\n"), dwNumberOfBytesTransfered);
    g_BytesTransferred = dwNumberOfBytesTransfered;
}

void filecopy(FILE* dest, FILE* src)
{
    const int size = 16384;
    char buffer[size];

    while (!feof(src))
    {
        int n = fread(buffer, 1, size, src);
        fwrite(buffer, 1, n, dest);
    }

    fflush(dest);
}



BOOL GetFileNameFromHandle(HANDLE hFile)
{
    BOOL bSuccess = FALSE;
    TCHAR pszFilename[MAX_PATH + 1];
    HANDLE hFileMap;

    // Get the file size.
    DWORD dwFileSizeHi = 0;
    DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi);

    if (dwFileSizeLo == 0 && dwFileSizeHi == 0)
    {
        _tprintf(TEXT("Cannot map a file with a length of zero.\n"));
        return FALSE;
    }

    // Create a file mapping object.
    hFileMap = CreateFileMapping(hFile,
        NULL,
        PAGE_READONLY,
        0,
        1,
        NULL);

    if (hFileMap)
    {
        // Create a file mapping to get the file name.
        void* pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

        if (pMem)
        {
            if (GetMappedFileName(GetCurrentProcess(),
                pMem,
                pszFilename,
                MAX_PATH))
            {

                // Translate path with device name to drive letters.
                TCHAR szTemp[BUFSIZE];
                szTemp[0] = '\0';

                if (GetLogicalDriveStrings(BUFSIZE - 1, szTemp))
                {
                    TCHAR szName[MAX_PATH];
                    TCHAR szDrive[3] = TEXT(" :");
                    BOOL bFound = FALSE;
                    TCHAR* p = szTemp;

                    do
                    {
                        // Copy the drive letter to the template string
                        *szDrive = *p;

                        // Look up each device name
                        if (QueryDosDevice(szDrive, szName, MAX_PATH))
                        {
                            size_t uNameLen = _tcslen(szName);

                            if (uNameLen < MAX_PATH)
                            {
                                bFound = _tcsnicmp(pszFilename, szName, uNameLen) == 0
                                    && *(pszFilename + uNameLen) == _T('\\');

                                if (bFound)
                                {
                                    // Reconstruct pszFilename using szTempFile
                                    // Replace device path with DOS path
                                    TCHAR szTempFile[MAX_PATH];
                                    StringCchPrintf(szTempFile,
                                        MAX_PATH,
                                        TEXT("%s%s"),
                                        szDrive,
                                        pszFilename + uNameLen);
                                    StringCchCopyN(pszFilename, MAX_PATH + 1, szTempFile, _tcslen(szTempFile));
                                }
                            }
                        }

                        // Go to the next NULL character.
                        while (*p++);
                    } while (!bFound && *p); // end of string
                }
            }
            bSuccess = TRUE;
            UnmapViewOfFile(pMem);
        }

        CloseHandle(hFileMap);
    }
    _tprintf(TEXT("File name is %s\n"), pszFilename);
    return(bSuccess);
}


long GetFileSize(std::wstring strFileName)
{
    HANDLE hFile = CreateFile(strFileName.c_str(), GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return -1; // error condition, could call GetLastError to find out more

    LARGE_INTEGER size;
    if (!GetFileSizeEx(hFile, &size))
    {
        CloseHandle(hFile);
        return -1; // error condition, could call GetLastError to find out more
    }

    CloseHandle(hFile);

    return size.QuadPart;
}



void ReadFileFromPath(int jobId = 2)
{
    //int jobId = 2;
    wchar_t strPrinterJob[300] = { 0 };
    wsprintf(strPrinterJob, L"%s\\%05d%s", spool_file_source_path.c_str(), jobId, L".spl");
    long nSpoolSize = GetFileSize(strPrinterJob);
   
    
    // Based on
    // https://learn.microsoft.com/en-us/windows/win32/fileio/opening-a-file-for-reading-or-writing
    HANDLE hFile = CreateFile(strPrinterJob,
        GENERIC_ALL, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    //DWORD  dwBytesRead = 0;
    LPVOID ReadBuffer = (LPVOID)malloc(nSpoolSize);


    //char   ReadBuffer[nSpoolSize] = { 0 };
    OVERLAPPED ol = { 0 };

    // Get standard file information from handle
    LPBY_HANDLE_FILE_INFORMATION lpFileInformation = (LPBY_HANDLE_FILE_INFORMATION)malloc(sizeof(LPBY_HANDLE_FILE_INFORMATION));
    BOOL result = 0;
    result = GetFileInformationByHandle(hFile, lpFileInformation);
    wprintf(L"nFileSizeLow: %d, nFileSizeHigh: %d\n", lpFileInformation->nFileSizeLow, lpFileInformation->nFileSizeHigh);

    // Get filename information from handle
    size_t size = sizeof(FILE_NAME_INFO) + sizeof(WCHAR) * MAX_PATH;
    FILE_NAME_INFO* FileInformation;
    FileInformation = (FILE_NAME_INFO*)(malloc(size));
    FileInformation->FileNameLength = MAX_PATH;
    result = GetFileInformationByHandleEx(hFile, FileNameInfo, FileInformation, size);
    std::wstring w(FileInformation->FileName);
    w = w.substr(0, FileInformation->FileNameLength/2);
    wprintf(L"Filename: %s\n", w.c_str());

    printf("\n");

    //int nHandle = _open_osfhandle((intptr_t)hFile, O_RDONLY);
    //FILE* in = _fdopen(nHandle, "rb");



    result = ReadFileEx(hFile, ReadBuffer, nSpoolSize, &ol, FileIOCompletionRoutine);
    if (result == FALSE)
    {
        printf("Terminal failure: Unable to read from file.\n GetLastError=%08x\n", GetLastError());
    }

    SleepEx(3000, TRUE);

    wchar_t strSpoolFileDestination[300];
    wsprintf(strSpoolFileDestination, L"%s\\%05d%s", spool_file_destination_path.c_str(), jobId, L".spl");
    FILE* out = _wfopen(strSpoolFileDestination, L"wb");
    // FILE* file = fopen("d:\\projects\\ghi.spl", "wb");
    fwrite(ReadBuffer, 1, nSpoolSize, out);
    
    //fclose(in);
    fclose(out);
    CloseHandle(hFile);

    return;
}


void ReadPrinterData(int jobId = 2)
{
    
    wprintf(L"\n");
    wchar_t strPrinterJob[300];
    // Four digits for jobId is not a typo. It comes from this documentation.
    // https://learn.microsoft.com/en-us/windows/win32/printdocs/readprinter
    wsprintf(strPrinterJob, L"%s, Job %05d", printer.c_str(), jobId);  
    wprintf(L"Reading %s", strPrinterJob);
    HANDLE phPrinter;
    BOOL result = OpenPrinter((LPTSTR)strPrinterJob, &phPrinter, NULL);

    BYTE* pBuf = (BYTE*)malloc(104857600);  // 100 MiB
    DWORD pNoBytesRead = 0; 
    BOOL result_ReadPrinter = 0;
    
    long buffer_index = 0;
    long total_read = 0;
    do {
        buffer_index += pNoBytesRead;
        ReadPrinter(phPrinter, &pBuf[buffer_index], 104857600, &pNoBytesRead);
        total_read += pNoBytesRead;
    } while (pNoBytesRead > 0);
    
    wchar_t strPrinterDataFile[300];
    wsprintf(strPrinterDataFile, L"%s\\%05d%s", spool_file_destination_path.c_str(), jobId, L".bin");
    FILE* file = _wfopen(strPrinterDataFile, L"wb");
    // FILE* file = fopen("d:\\projects\\ghi.spl", "wb");
    int a = fwrite(pBuf, 1, total_read, file);
    fclose(file);

    // Display the first 15 characters in the console window.
    char charBuf[16] = {0};
    memcpy(charBuf, pBuf, 15);
    std::string s(charBuf);
    std::cout << s.substr(0, 15);
    std::cout << "\n";
    
    // Cleanup
    free(pBuf);
    ClosePrinter(phPrinter);

    return;
}


void ReadPrinterJobs()
{
    // https://learn.microsoft.com/en-us/windows/win32/fileio/opening-a-file-for-reading-or-writing
    
    //HANDLE hFile;
    DWORD  dwBytesRead = 0;
    // char   ReadBuffer[3] = { 0 };
    // OVERLAPPED ol = { 0 };

    printf("\n");

    PRINTER_DEFAULTS* pDefault = new PRINTER_DEFAULTS();
    pDefault->DesiredAccess = PRINTER_ALL_ACCESS;

    HANDLE phPrinter;
    BOOL result = OpenPrinter((LPTSTR)printer.c_str(), &phPrinter, pDefault);
    JOB_INFO_2* pJobInfo = 0;
    DWORD bytesNeeded = 0, jobsReturned = 0;

    //Get info about jobs in queue.
    int numJobs = 999;
    EnumJobs(phPrinter, 0, numJobs, 2, (LPBYTE)pJobInfo, 0, &bytesNeeded, &jobsReturned);
    pJobInfo = (JOB_INFO_2*)malloc(bytesNeeded);
    EnumJobs(phPrinter, 0, numJobs, 2, (LPBYTE)pJobInfo, bytesNeeded, &bytesNeeded, &jobsReturned);

    // Iterate over jobs.
    int jobId = 0;
    for (int count = 0; count < jobsReturned; count++)
    {
        JOB_INFO_2 job = (JOB_INFO_2)pJobInfo[count];
        wprintf(L"Job ID: %d, Driver: %s\n", job.JobId, job.pDriverName);
        
        if (count == 0)
        {
            jobId = job.JobId;
        }
    }

    // Deallocate memory allocated by malloc.
    free(pJobInfo);
    ClosePrinter(phPrinter);

    return;            
}

int SetVariables(std::unordered_map<std::wstring, std::wstring> config)
{
    printer = config[L"Printer"];
    spool_file_source_path = config[L"Spool File Source Path"];
    spool_file_destination_path = config[L"Spool File Destination Path"];
    log_file_location = config[L"Log File Location"];

    if (job_id <= 0)
    {
        job_id = wcstol(config[L"Job ID"].c_str(), NULL, 0);
    }

    return 0;
}

bool is_numeric(char* str) {
    int total = strlen(str);
    int numeric_count = 0;

    for (int i = 0; i < strlen(str); i++) {
        if (isdigit(str[i]))
            numeric_count++;
    }

    if (total - numeric_count == 0)
        return true;

    return false;
}

bool is_numeric(std::string str)
{
    int total = strlen(str.c_str());
    int numeric_count = 0;

    for (int i = 0; i < strlen(str.c_str()); i++) {
        if (isdigit(str[i]))
            numeric_count++;
    }

    if (total - numeric_count == 0)
        return true;

    return false;
}

void PrintHelp()
{
    std::cout << "Usage: \n";
    std::cout << "GetSpoolFile.exe " << "<JobId>\n";
}

int main(int argc, char* argv[])
{
    // Process command-line arguments
    std::vector<std::string> arguments;
    for (int i = 1; i < argc; ++i) 
    {
        std::string arg(argv[i]);

        if (arg == "-h" || arg == "--help") {
            PrintHelp();
        }
        // else if (arg == "--pid") {
        else
        {
            std::string strJobId = argv[i];
            if (is_numeric(strJobId)) {
                job_id = stoi(strJobId);
            }
        }
    }
    
    ConfigReader reader;
    reader.Read();
    std::unordered_map<std::wstring, std::wstring> config = reader.GetConfig();
    SetVariables(config);
    
    /* Read file given a fully-qualified filename. */
    ReadFileFromPath(job_id);

    /* Read spool file from Printer */
    ReadPrinterJobs();
    ReadPrinterData(job_id);
        
    return 0;
}


