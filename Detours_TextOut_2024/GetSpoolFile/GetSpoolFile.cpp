// GetSpoolFile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

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

    /*
    // Add a while loop her to allow reading and writing chunks.
    while (true)
    {
        if (dwBytesRead > 0 && dwBytesRead <= BUFFERSIZE - 1)
        {
            // Write
        }
        else if (dwBytesRead == 0)
        {
            // end while
        }
        else
        {
            printf("\n ** Unexpected value for dwBytesRead ** \n");
        }

        // It is always good practice to close the open file handles even though
        // the app will exit here and clean up open handles anyway.
    }
    */
    
    CloseHandle(hFile);
}


void ReadSpoolFileFromPrinter()
{
    // https://learn.microsoft.com/en-us/windows/win32/fileio/opening-a-file-for-reading-or-writing
    
    //HANDLE hFile;
    DWORD  dwBytesRead = 0;
    char   ReadBuffer[BUFFERSIZE] = { 0 };
    OVERLAPPED ol = { 0 };

    printf("\n");

    FILE* out = fopen("d:\\projects\\def.spl", "wb");

    LPTSTR printerName = (LPTSTR)L"Bullzip PDF Printer";
    PRINTER_DEFAULTS* pDefault = new PRINTER_DEFAULTS();
    pDefault->DesiredAccess = PRINTER_ALL_ACCESS;
    // pDefault->DesiredAccess = PRINTER_ACCESS_ADMINISTER;
    // pDefault->DesiredAccess = PRINTER_ACCESS_USE;

    HANDLE phPrinter;
    BOOL result = OpenPrinter(printerName, &phPrinter, pDefault);
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
        JOB_INFO_2 job = pJobInfo[count];
        wprintf(L"Job %d %s\n", job.JobId, job.pDriverName);
        
        if (count == 0)
        {
            jobId = job.JobId;
        }

        int a = 1;

        // Delete job
        // SetJob(phPrinter, pJobInfo[count].JobId, 0, NULL, JOB_CONTROL_DELETE);
    }

    free(pJobInfo);//free now
    

    //std::string printerName = "Bullzip PDF Printer, Job 0042";
    wchar_t printerName2[] = L"Bullzip PDF Printer";
    wchar_t str[80];
    wsprintf(str, L"%s, Job %04d", printerName2, jobId);
    HANDLE phPrinter2;
    BOOL result2 = OpenPrinter((LPTSTR)str, &phPrinter2, NULL);




    //HANDLE phPrinter;
    //BOOL result = OpenPrinter((LPTSTR)L"Bullzip PDF Printer", &phPrinter, NULL);

    printf("phPrinter: %p\n", phPrinter);
    printf("phPrinter2: %p\n", phPrinter2);

    HANDLE hSpoolFile = GetSpoolFileHandle(phPrinter);
    HANDLE hSpoolFile2 = GetSpoolFileHandle(phPrinter2);
    printf("hSpoolFile: %p\n", hSpoolFile);
    printf("hSpoolFile2: %p\n", hSpoolFile2);
    

    pJobInfo = (JOB_INFO_2*)malloc(bytesNeeded);

    LPVOID pBuf = (LPVOID)malloc(1000010);
    DWORD pNoBytesRead = 0; // = (DWORD)malloc(16);
    BOOL result_ReadPrinter = ReadPrinter(phPrinter2, pBuf, 1000000, &pNoBytesRead);
    
    // Consider moving from stack to heap using malloc
    // https://github.com/MicrosoftDocs/cpp-docs/blob/main/docs/code-quality/c6262.md
    BYTE byteBuf[1000020];
    memcpy(byteBuf, pBuf, 1000000);
    FILE* file = fopen("d:\\projects\\ghi.spl", "wb");
    fwrite(byteBuf, 1, pNoBytesRead, file);
    fclose(file);
    // free(byteBuf);

    char charBuf[16];
    memcpy(charBuf, pBuf, 10);
    std::string s(charBuf);
    std::cout << s.substr(0, 10);
    std::cout << "\n";
    // free(charBuf);

    // Cleanup
    free(pBuf);
    ClosePrinter(phPrinter);
            
}





int main()
{
    /* Read file given a fully-qualified filename. */
    LPCWSTR spoolFilePath = L"C:\\Windows\\System32\\spool\\PRINTERS\\00002.SPL";
    ReadFileFromPath(spoolFilePath);

    /* Read spool file from Printer */
    ReadSpoolFileFromPrinter();

    /* Read the file given a handle */
    HANDLE hFile = CreateFile(spoolFilePath,
        GENERIC_ALL, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    GetFileNameFromHandle(hFile);
    int nHandle = _open_osfhandle((intptr_t)hFile, O_RDONLY);
    FILE* fp = _fdopen(nHandle, "rb");
    FILE* out = fopen("d:\\projects\\abc.spl", "wb");
    filecopy(out, fp);
    fclose(fp);
    fclose(out);
        
    return 0;

}


