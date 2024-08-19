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


#define BUFFERSIZE 305
#define BUFSIZE 512

DWORD g_BytesTransferred = 0;
std::wstring printer = L"";
std::wstring spool_file_source_path = L"";
std::wstring spool_file_destination_path = L"";
std::wstring log_file_location = L"";

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
    _tprintf(TEXT("Error code:\t%x\n"), dwErrorCode);
    _tprintf(TEXT("Number of bytes:\t%x\n"), dwNumberOfBytesTransfered);
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


void ReadFileFromPath(LPCWSTR path)
{
    // Based on
    // https://learn.microsoft.com/en-us/windows/win32/fileio/opening-a-file-for-reading-or-writing
    HANDLE hFile = CreateFile(path,
        GENERIC_ALL, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD  dwBytesRead = 0;
    char   ReadBuffer[BUFFERSIZE] = { 0 };
    OVERLAPPED ol = { 0 };

    // Get standard file information from handle
    LPBY_HANDLE_FILE_INFORMATION lpFileInformation = (LPBY_HANDLE_FILE_INFORMATION)malloc(sizeof(LPBY_HANDLE_FILE_INFORMATION));
    BOOL result = 0;
    result = GetFileInformationByHandle(hFile, lpFileInformation);
    wprintf(L"nFileSizeHigh %d\n", lpFileInformation->nFileSizeLow);

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

    int nHandle = _open_osfhandle((intptr_t)hFile, O_RDONLY);
    FILE* in = _fdopen(nHandle, "rb");
    FILE* out = fopen("d:\\projects\\def.spl", "wb");

    result = ReadFileEx(hFile, ReadBuffer, BUFFERSIZE - 1, &ol, FileIOCompletionRoutine);
    if (result == FALSE)
    {
        printf("Terminal failure: Unable to read from file.\n GetLastError=%08x\n", GetLastError());
    }

    SleepEx(3000, TRUE);
    dwBytesRead = g_BytesTransferred;
    CloseHandle(hFile);
}


void ReadPrinterData(std::wstring printerName = L"", int jobId = 2)
{
    
    //std::string printerName = "Bullzip PDF Printer, Job 0042";
    //wchar_t printerName2[] = L"Bullzip PDF Printer";
    wprintf(L"\n");
    wchar_t strPrinterJob[300];
    wsprintf(strPrinterJob, L"%s, Job %04d", printerName.c_str(), jobId);
    wprintf(L"Reading %s", strPrinterJob);
    HANDLE phPrinter;
    BOOL result = OpenPrinter((LPTSTR)strPrinterJob, &phPrinter, NULL);

    BYTE* pBuf = (BYTE*)malloc(1000000);
    DWORD pNoBytesRead = 0; 
    BOOL result_ReadPrinter = ReadPrinter(phPrinter, pBuf, 1000000, &pNoBytesRead);

    wchar_t strPrinterDataFile[300];
    wsprintf(strPrinterDataFile, L"%s\\%04d%s", spool_file_destination_path.c_str(), jobId, L".bin");
    FILE* file = _wfopen(strPrinterDataFile, L"wb");
    // FILE* file = fopen("d:\\projects\\ghi.spl", "wb");
    fwrite(pBuf, 1, pNoBytesRead, file);
    fclose(file);

    char charBuf[16] = {'\0'};
    memcpy(charBuf, pBuf, 15);
    std::string s(charBuf);
    std::cout << s.substr(0, 15);
    std::cout << "\n";
    
    // Cleanup
    free(pBuf);
    ClosePrinter(phPrinter);

    return;
}



void ReadJobs(std::wstring printerName=L"")
{
    // https://learn.microsoft.com/en-us/windows/win32/fileio/opening-a-file-for-reading-or-writing
    
    //HANDLE hFile;
    DWORD  dwBytesRead = 0;
    char   ReadBuffer[BUFFERSIZE] = { 0 };
    OVERLAPPED ol = { 0 };

    printf("\n");

    //LPTSTR printerName = (LPTSTR)L"Bullzip PDF Printer";
    PRINTER_DEFAULTS* pDefault = new PRINTER_DEFAULTS();
    pDefault->DesiredAccess = PRINTER_ALL_ACCESS;

    HANDLE phPrinter;
    BOOL result = OpenPrinter((LPTSTR)printerName.c_str(), &phPrinter, pDefault);
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

    return 0;
}



int main()
{
    ConfigReader reader;
    reader.Read();
    std::unordered_map<std::wstring, std::wstring> config = reader.GetConfig();
    SetVariables(config);
    
    
    /* Read file given a fully-qualified filename. */
    LPCWSTR spoolFilePath = L"C:\\Windows\\System32\\spool\\PRINTERS\\00002.SPL";
    ReadFileFromPath(spoolFilePath);

    /* Read spool file from Printer */
    ReadJobs(L"Bullzip PDF Printer");
    ReadPrinterData(L"Bullzip PDF Printer", 2);

    /* Read the file given a handle */
    /*
    HANDLE hFile = CreateFile(spoolFilePath,
        GENERIC_ALL, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    GetFileNameFromHandle(hFile);
    int nHandle = _open_osfhandle((intptr_t)hFile, O_RDONLY);
    FILE* fp = _fdopen(nHandle, "rb");
    FILE* out = fopen("d:\\projects\\abc.spl", "wb");
    filecopy(out, fp);
    fclose(fp);
    fclose(out);
    */
        
    return 0;

}


