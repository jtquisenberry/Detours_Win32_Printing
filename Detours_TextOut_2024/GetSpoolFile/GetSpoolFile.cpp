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

void  Read(HANDLE hFile)
{
    // https://learn.microsoft.com/en-us/windows/win32/fileio/opening-a-file-for-reading-or-writing
    
    //HANDLE hFile;
    DWORD  dwBytesRead = 0;
    char   ReadBuffer[BUFFERSIZE] = { 0 };
    OVERLAPPED ol = { 0 };

    printf("\n");

    // Read one character less than the buffer size to save room for
    // the terminating NULL character. 
    int nHandle = _open_osfhandle((intptr_t)hFile, O_RDONLY);
    FILE* fp = _fdopen(nHandle, "rb");
    FILE* out = fopen("d:\\projects\\def.spl", "wb");
    
    /*
    size_t n, m;
    unsigned char buff[8192];
    do {
        n = fread(buff, 1, sizeof buff, fp);
        int a = 1;
        if (n) m = fwrite(buff, 1, n, out);
        else   m = 0;
    } while ((n > 0)); //&& (n == m));
    if (m) perror("copy");
    */

    LPTSTR printerName = (LPTSTR)L"Bullzip PDF Printer";
    PRINTER_DEFAULTS* pDefault = new PRINTER_DEFAULTS();
    // pDefault->DesiredAccess = PRINTER_ALL_ACCESS;
    // pDefault->DesiredAccess = PRINTER_ACCESS_ADMINISTER;
    pDefault->DesiredAccess = PRINTER_ACCESS_USE;


    HANDLE phPrinter;
    BOOL result = OpenPrinter(printerName, &phPrinter, pDefault);

    JOB_INFO_2* pJobInfo = 0;
    DWORD bytesNeeded = 0, jobsReturned = 0;


    //Get info about jobs in queue.
    int numJobs = 999;
    EnumJobs(phPrinter, 0, numJobs, 2, (LPBYTE)pJobInfo, 0, &bytesNeeded, &jobsReturned);
    pJobInfo = (JOB_INFO_2*)malloc(bytesNeeded);
    EnumJobs(phPrinter, 0, numJobs, 2, (LPBYTE)pJobInfo, bytesNeeded, &bytesNeeded, &jobsReturned);

    //Loop and delete each waiting job
    for (int count = 0; count < jobsReturned; count++)
    {
        JOB_INFO_2 job = pJobInfo[count];
        std::cout << job.JobId;
        int a = 1;
        
        /*
        cout << "Deleting JobID  " << pJobInfo[count].JobId;
        if (SetJob(hPrinter, pJobInfo[count].JobId, 0, NULL, JOB_CONTROL_DELETE) != 0)
        {
            cout << "...... Deleted OK" << endl;
        }
        else
        {
            cout << "...... Failed to Delete" << endl;
        }
        */
    }

    free(pJobInfo);//free now
    // ClosePrinter(phPrinter);


    HANDLE phPrinter2;
    BOOL result2 = OpenPrinter((LPTSTR)L"Bullzip PDF Printer, Job 0042", &phPrinter2, NULL);




    //HANDLE phPrinter;
    //BOOL result = OpenPrinter((LPTSTR)L"Bullzip PDF Printer", &phPrinter, NULL);

    printf("phPrinter: %p\n", phPrinter);
    printf("phPrinter2: %p\n", phPrinter2);

    HANDLE hSpoolFile = GetSpoolFileHandle(phPrinter);
    HANDLE hSpoolFile2 = GetSpoolFileHandle(phPrinter2);
    printf("hSpoolFile: %p\n", hSpoolFile);
    printf("hSpoolFile2: %p\n", hSpoolFile2);
    

    pJobInfo = (JOB_INFO_2*)malloc(bytesNeeded);

    LPVOID pBuf = (LPVOID)malloc(64);
    DWORD pNoBytesRead = (DWORD)malloc(16);
    BOOL xxx = ReadPrinter(phPrinter2, &pBuf, 16, &pNoBytesRead);
    char charBuf[1024];
    memcpy(charBuf, &pBuf, 8);
    sprintf(charBuf, "%c", pBuf);
    std::string s(charBuf);
    

    //fclose(out);


    //BOOL result = ReadFileEx(hFile, ReadBuffer, BUFFERSIZE - 1, &ol, FileIOCompletionRoutine);
    ReadFileEx(hFile, ReadBuffer, BUFFERSIZE - 1, &ol, FileIOCompletionRoutine);
    int a = 1;

    /*
    SleepEx(5000, TRUE);
    dwBytesRead = g_BytesTransferred;
    // This is the section of code that assumes the file is ANSI text. 
    // Modify this block for other data types if needed.

    if (dwBytesRead > 0 && dwBytesRead <= BUFFERSIZE - 1)
    {
        ReadBuffer[dwBytesRead] = '\0'; // NULL character

        _tprintf(TEXT("Data read from %s (%d bytes): \n"), argv[1], dwBytesRead);
        printf("%s\n", ReadBuffer);
    }
    else if (dwBytesRead == 0)
    {
        _tprintf(TEXT("No data read from file %s\n"), argv[1]);
    }
    else
    {
        printf("\n ** Unexpected value for dwBytesRead ** \n");
    }

    // It is always good practice to close the open file handles even though
    // the app will exit here and clean up open handles anyway.
    */
    CloseHandle(hFile);
}





int main()
{
    // std::cout << "Hello World!\n";

    HANDLE hFile = CreateFile(L"C:\\Windows\\System32\\spool\\PRINTERS\\00002.SPL", 
        GENERIC_ALL, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    // GetFileNameFromHandle(hFile);
    Read(hFile);
    /*
    int nHandle = _open_osfhandle((intptr_t)hFile, O_RDONLY);
    FILE* fp = _fdopen(nHandle, "rb");
    FILE* out = fopen("d:\\projects\\abc.spl", "wb");
    filecopy(out, fp);
    fclose(fp);
    fclose(out);
    */

    /*
    void fileopen_and_copy(char* dest, char* src)
    {
        FILE* infile = fopen(src, "rb");
        FILE* outfile = fopen(dest, "wb");

        filecopy(outfile, infile);

        fclose(infile);
        fclose(outfile);
    }
    */



    
    int a = 1;











}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
