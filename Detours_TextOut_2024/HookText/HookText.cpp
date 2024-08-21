// Defines the entry point for the DLL application.
#pragma once
#pragma comment(lib, "detours.lib")

#include "pch.h"
#include "HookText.h"
#include "Logger.h"
#include <stdio.h>
#include <string>


// Module-level variables
FILE* pFile;
HDC hDC_current = 0x0;
HDC hdc01;
LPPRINTDLG lppd01;
Logger logger;


HDC g_hDC;
HWND g_hwnd;
HANDLE g_phPrinter;
int g_nPrintAttempts = 0;
int g_nSaveDC = 0;
int g_StartPageCounter = 0;


// Function declarations



/** Target pointers for the uninstrumented API. *********************************************************
 * Here are the declarations of the original functions. The arguments of the prototype are the data types
 * of the arguments of the original function. It is possible to call the original function by the pointers 
 * defined below.
 ********************************************************************************************************/

// Start Printing
int(WINAPI* pPrintDlgW)(LPPRINTDLGW) = PrintDlgW;
long(WINAPI* pPrintDlgExW)(LPPRINTDLGEXW) = PrintDlgExW;
BOOL(WINAPI* pAddJobA)(HANDLE, DWORD, LPBYTE, DWORD, LPDWORD) = AddJobA;
BOOL(WINAPI* pAddJobW)(HANDLE, DWORD, LPBYTE, DWORD, LPDWORD) = AddJobW;
 
// Document Control
int(WINAPI* pStartDocA)(HDC, CONST DOCINFOA*) = StartDocA;
int(WINAPI* pStartDocW)(HDC, CONST DOCINFOW*) = StartDocW;
int(WINAPI* pStartPage)(HDC) = StartPage;
int(WINAPI* pEndPage)(HDC) = EndPage;
int(WINAPI* pEndDoc)(HDC) = EndDoc;

// Text Drawing
BOOL(WINAPI* pExtTextOutA)(HDC, int, int, UINT, CONST RECT*, LPCSTR, UINT, CONST INT*) = ExtTextOutA;
BOOL(WINAPI* pExtTextOutW)(HDC, int, int, UINT, CONST RECT*, LPCTSTR, UINT, CONST INT*) = ExtTextOutW;
BOOL(WINAPI* pTextOutA)(HDC, int, int, LPCSTR, int) = TextOutA;
BOOL(WINAPI* pTextOutW)(HDC, int, int, LPCTSTR, int) = TextOutW;
int(WINAPI* pDrawTextA)(HDC, LPCSTR, int, LPRECT, UINT) = DrawTextA;
int(WINAPI* pDrawTextW)(HDC, LPCTSTR, int, LPRECT, UINT) = DrawTextW;
int(WINAPI* pDrawTextExA)(HDC, LPSTR, int, LPRECT, UINT, LPDRAWTEXTPARAMS) = DrawTextExA;
int(WINAPI* pDrawTextExW)(HDC, LPTSTR, int, LPRECT, UINT, LPDRAWTEXTPARAMS) = DrawTextExW;
BOOL(WINAPI* pPolyTextOutA)(HDC, CONST POLYTEXTA*, int) = PolyTextOutA;
BOOL(WINAPI* pPolyTextOutW)(HDC, CONST POLYTEXTW*, int) = PolyTextOutW;



void WINAPI PrintPrintDlgExW(LPPRINTDLGEXW lpprintdlgexw01)
{
	fwprintf(pFile, L"hDC =                  %p\n", lpprintdlgexw01->hDC);
	fwprintf(pFile, L"lStructSize =          %d\n", lpprintdlgexw01->lStructSize);
	fwprintf(pFile, L"hwndOwner =            %p\n", lpprintdlgexw01->hwndOwner);
	fwprintf(pFile, L"hDevMode =             %p\n", lpprintdlgexw01->hDevMode);
	fwprintf(pFile, L"hDevName s=            %p\n", lpprintdlgexw01->hDevNames);
	fwprintf(pFile, L"Flags =                %s\n", (IntToBinaryStringW(lpprintdlgexw01->Flags)).c_str());
	fwprintf(pFile, L"Flags2 =               %d\n", lpprintdlgexw01->Flags2);
	fwprintf(pFile, L"ExclusionFlags =       %d\n", lpprintdlgexw01->ExclusionFlags);
	fwprintf(pFile, L"nPageRanges =          %d\n", lpprintdlgexw01->nPageRanges);
	fwprintf(pFile, L"nMaxPageRanges =       %d\n", lpprintdlgexw01->nMaxPageRanges);
	fwprintf(pFile, L"nMinPage =             %d\n", lpprintdlgexw01->nMinPage);
	fwprintf(pFile, L"nMaxPage =             %d\n", lpprintdlgexw01->nMaxPage);
	fwprintf(pFile, L"nCopies =              %d\n", lpprintdlgexw01->nCopies);
	fwprintf(pFile, L"hInstance =            %p\n", lpprintdlgexw01->hInstance);
	fwprintf(pFile, L"nlpPrintTemplateName = %s\n", lpprintdlgexw01->lpPrintTemplateName);
	fwprintf(pFile, L"nlpCallback =          %p\n", lpprintdlgexw01->lpCallback);
	fwprintf(pFile, L"nPropertyPages =       %d\n", lpprintdlgexw01->nPropertyPages);
	fwprintf(pFile, L"lphPropertyPages =     %p\n", lpprintdlgexw01->lphPropertyPages);
	fwprintf(pFile, L"nStartPage =           %d\n", lpprintdlgexw01->nStartPage);
	fwprintf(pFile, L"dwResultAction =       %d\n", lpprintdlgexw01->dwResultAction);
}

void WINAPI PrintPrintDlg(LPPRINTDLG lpprintdlg)
{
	fwprintf(pFile, L"Flags =           %#010x\n", lpprintdlg->Flags);
	fwprintf(pFile, L"hDC =                 %p\n", lpprintdlg->hDC);
	fwprintf(pFile, L"hDevMode =            %p\n", lpprintdlg->hDevMode);
	fwprintf(pFile, L"hInstance =           %p\n", lpprintdlg->hInstance);
	fwprintf(pFile, L"hPrintTemplate =      %p\n", lpprintdlg->hPrintTemplate);
	fwprintf(pFile, L"hSetupTemplate =      %p\n", lpprintdlg->hSetupTemplate);
	fwprintf(pFile, L"hwndOwner =           %p\n", lpprintdlg->hwndOwner);
	fwprintf(pFile, L"lCustData =           %p\n", lpprintdlg->lCustData);
	fwprintf(pFile, L"lpfnPrintHook =       %p\n", lpprintdlg->lpfnPrintHook);
	fwprintf(pFile, L"lpfnSetupHook =       %p\n", lpprintdlg->lpfnSetupHook);
	fwprintf(pFile, L"lpPrintTemplateName = %s\n", lpprintdlg->lpPrintTemplateName);
	fwprintf(pFile, L"lpSetupTemplateName = %s\n", lpprintdlg->lpSetupTemplateName);
	fwprintf(pFile, L"lStructSize =         %d\n", lpprintdlg->lStructSize);
	fwprintf(pFile, L"nCopies =             %d\n", lpprintdlg->nCopies);
	fwprintf(pFile, L"nFromPage =           %d\n", lpprintdlg->nFromPage);
	fwprintf(pFile, L"nMaxPage =            %d\n", lpprintdlg->nMaxPage);
	fwprintf(pFile, L"nMinPage =            %d\n", lpprintdlg->nMinPage);
	fwprintf(pFile, L"nToPage =             %d\n", lpprintdlg->nToPage);
}


void WINAPI PrintDevMode(DEVMODE* devmode)
{
	fwprintf(pFile, L"dmDeviceName =         %s\n", devmode->dmDeviceName);
	fwprintf(pFile, L"dmSpecVersion =        %d\n", devmode->dmSpecVersion);
	fwprintf(pFile, L"dmDriverVersion =      %d\n", devmode->dmDriverVersion);
	fwprintf(pFile, L"dmSize =               %d\n", devmode->dmSize);
	fwprintf(pFile, L"dmDriverExtra =        %d\n", devmode->dmDriverExtra);
	fwprintf(pFile, L"dmFields =             %d\n", devmode->dmFields);
	fwprintf(pFile, L"dmOrientation =        %d\n", devmode->dmOrientation);
	fwprintf(pFile, L"dmPaperSize =          %d\n", devmode->dmPaperSize);
	fwprintf(pFile, L"dmPaperLength =        %d\n", devmode->dmPaperLength);
	fwprintf(pFile, L"dmPaperWidth =         %d\n", devmode->dmPaperWidth);
	fwprintf(pFile, L"dmScale =              %d\n", devmode->dmScale);
	fwprintf(pFile, L"dmCopies =             %d\n", devmode->dmCopies);
	fwprintf(pFile, L"dmDefaultSource =      %d\n", devmode->dmDefaultSource);
	fwprintf(pFile, L"dmPrintQuality =       %d\n", devmode->dmPrintQuality);
	fwprintf(pFile, L"dmPosition.x =         %d\n", devmode->dmPosition.x);
	fwprintf(pFile, L"dmPosition.y =         %d\n", devmode->dmPosition.y);
	fwprintf(pFile, L"dmDisplayOrientation = %d\n", devmode->dmDisplayOrientation);
	fwprintf(pFile, L"dmDisplayFixedOutput = %d\n", devmode->dmDisplayFixedOutput);
	fwprintf(pFile, L"dmColor =              %d\n", devmode->dmColor);
	fwprintf(pFile, L"dmDuplex =             %d\n", devmode->dmDuplex);
	fwprintf(pFile, L"dmYResolution =        %d\n", devmode->dmYResolution);
	fwprintf(pFile, L"dmTTOption =           %d\n", devmode->dmTTOption);
	fwprintf(pFile, L"dmCollate =            %d\n", devmode->dmCollate);
	fwprintf(pFile, L"dmFormName =           %s\n", devmode->dmFormName);
	fwprintf(pFile, L"dmLogPixels =          %d\n", devmode->dmLogPixels);
	fwprintf(pFile, L"dmBitsPerPel =         %d\n", devmode->dmBitsPerPel);
	fwprintf(pFile, L"dmPelsWidth =          %d\n", devmode->dmPelsWidth);
	fwprintf(pFile, L"dmPelsHeight =         %d\n", devmode->dmPelsHeight);
	fwprintf(pFile, L"dmDisplayFlags =       %d\n", devmode->dmDisplayFlags);
	fwprintf(pFile, L"dmNup =                %d\n", devmode->dmNup);
	fwprintf(pFile, L"dmDisplayFrequency =   %d\n", devmode->dmDisplayFrequency);
	fwprintf(pFile, L"dmICMMethod =          %d\n", devmode->dmICMMethod);
	fwprintf(pFile, L"dmICMIntent =          %d\n", devmode->dmICMIntent);
	fwprintf(pFile, L"dmMediaType =          %d\n", devmode->dmMediaType);
	fwprintf(pFile, L"dmDitherType =         %d\n", devmode->dmDitherType);
	fwprintf(pFile, L"dmReserved1 =          %d\n", devmode->dmReserved1);
	fwprintf(pFile, L"dmReserved2 =          %d\n", devmode->dmReserved2);
	fwprintf(pFile, L"dmPanningWidth =       %d\n", devmode->dmPanningWidth);
	fwprintf(pFile, L"dmPanningHeight =      %d\n", devmode->dmPanningHeight);
}

void PrintDevNames(DEVNAMES* devnames)
{
	fwprintf(pFile, L"wDriverOffset =        %d\n", devnames->wDriverOffset);
	fwprintf(pFile, L"wDeviceOffset =        %d\n", devnames->wDeviceOffset);
	fwprintf(pFile, L"wOutputOffset =        %d\n", devnames->wOutputOffset);
	fwprintf(pFile, L"wDefault =             %d\n", devnames->wDefault);
}

void PrintPageRange(PRINTPAGERANGE* pageranges)
{
	fwprintf(pFile, L"nFromPage =            %d\n", pageranges->nFromPage);
	fwprintf(pFile, L"nToPage =              %d\n", pageranges->nToPage);
}

/*************************************************************************
 * Detour / Proxy functions
 * All Windows APIs use calling convention WINAPI.
 *************************************************************************/

long WINAPI MyPrintDlgExW(LPPRINTDLGEXW lpprintdlgexw)
{
	// Call the real PrintDlgExW
	int RETURN_PrintDlgExW = pPrintDlgExW(lpprintdlgexw);

	// Save the handle of the window that called PrintDlgExW
	g_hwnd = lpprintdlgexw->hwndOwner;

	// If this is the first attempt to print, then save the handle to the Device Context
	// for later use.
	g_nPrintAttempts++;
	if (g_nPrintAttempts == 1)
	{
		g_hDC = lpprintdlgexw->hDC;
	}

	//Save the state of the Device context.
	g_nSaveDC = SaveDC(lpprintdlgexw->hDC);

	fwprintf(pFile, L"\n");
	fwprintf(pFile, L"-----------------------------------------------------------\n");
	fwprintf(pFile, L"PrintDlgExW\n");
	fwprintf(pFile, L"-----------------------------------------------------------\n");

	// Update hDC here so it gets passed to subsequent functions.
	lpprintdlgexw->hDC = g_hDC;

	PrintPrintDlgExW(lpprintdlgexw);
	DEVMODE* devmode = (DEVMODE*)GlobalLock(lpprintdlgexw->hDevMode);
	PrintDevMode(devmode);
	DEVNAMES* devnames = (DEVNAMES*)::GlobalLock(lpprintdlgexw->hDevNames);
	PrintDevNames(devnames);
	PRINTPAGERANGE* pagerange = (PRINTPAGERANGE*)::GlobalLock(lpprintdlgexw->lpPageRanges);
	PrintPageRange(pagerange);

	// Call Unlocks on objects allocated with GlobalLock
	::GlobalUnlock(lpprintdlgexw->hDevMode);
	::GlobalUnlock(lpprintdlgexw->hDevNames);
	::GlobalUnlock(lpprintdlgexw->lpPageRanges);

	return RETURN_PrintDlgExW;
}

int WINAPI MyStartPage(HDC hDC)
{
	int RETURN_MyStartPage = pStartPage(hDC);

	fwprintf(pFile, L"\n");
	fwprintf(pFile, L"-----------------------------------------------------------\n");
	fwprintf(pFile, L"StartPage\n");
	fwprintf(pFile, L"-----------------------------------------------------------\n");
	fwprintf(pFile, L"hDC =                  %p\n", hDC);

	return RETURN_MyStartPage;
}

int WINAPI MyEndPage(HDC hDC)
{
	int RETURN_MyEndPage = pEndPage(hDC);

	fwprintf(pFile, L"\n");
	fwprintf(pFile, L"-----------------------------------------------------------\n");
	fwprintf(pFile, L"EndPage\n");
	fwprintf(pFile, L"-----------------------------------------------------------\n");
	fwprintf(pFile, L"hDC =                    %p\n", hDC);

	return RETURN_MyEndPage;
}

int WINAPI MyEndDoc(HDC hDC)
{
	int RETURN_EndDoc = pEndDoc(hDC);

	fwprintf(pFile, L"\n");
	fwprintf(pFile, L"-----------------------------------------------------------\n");
	fwprintf(pFile, L"EndDoc\n");
	fwprintf(pFile, L"-----------------------------------------------------------\n");
	fwprintf(pFile, L"hDC = %p\n", hDC);

	return RETURN_EndDoc;
}

int WINAPI MyPrintDlgW(LPPRINTDLGW lpprintdlgw)
{
	int RETURN_PrintDlgW = pPrintDlgW(lpprintdlgw);
	
	fwprintf(pFile, L"\n");
	fwprintf(pFile, L"-----------------------------------------------------------\n");
	fwprintf(pFile, L"PrintDlgW\n");
	fwprintf(pFile, L"-----------------------------------------------------------\n");
	
	PrintPrintDlg(lpprintdlgw);
	DEVMODE* devmode = (DEVMODE*)GlobalLock(lpprintdlgw->hDevMode);
	PrintDevMode(devmode);
	DEVNAMES* devnames = (DEVNAMES*)::GlobalLock(lpprintdlgw->hDevNames);
	PrintDevNames(devnames);

	::GlobalUnlock(lpprintdlgw->hDevMode);
	::GlobalUnlock(lpprintdlgw->hDevNames);
	
	return RETURN_PrintDlgW;
}

BOOL WINAPI MyAddJobW(HANDLE hPrinter, DWORD Level, LPBYTE pData, DWORD cbBuf, LPDWORD pcbNeeded)
{

	BOOL RETURN_AddJobW = 0;
	RETURN_AddJobW = pAddJobW(hPrinter, Level, pData, cbBuf, pcbNeeded);



	//JOB_INFO_2 *pJob = NULL;
	//
	//	// If the GetJobW function fails, it will return the size needed to store the print job.
	//	// That job can be used in a second attempt at the function.
	//	DWORD pcbNeeded, pcbReturned;
	//
	//	//Call get Job.
	//	BOOL RETURN_GetJobW;		
	//	RETURN_GetJobW = GetJobW(global_phPrinter, RETURN_StartDocW, 2, (LPBYTE)pJob, 0, &pcbNeeded);	
	//
	//	//Allocate a block of memory to pJob.
	//	//It might be better to use ::GlobalLock.
	//	pJob = (JOB_INFO_2 *)malloc(pcbNeeded);
	//
	//	RETURN_GetJobW = GetJobW(global_phPrinter, RETURN_StartDocW, 2, (LPBYTE)pJob, pcbNeeded, &pcbReturned);
	//	

	fwprintf(pFile, L"\n\nAddJobW\n------------------\n");
	fwprintf(pFile, L"hPrinter = %d", hPrinter);
	fwprintf(pFile, L"Level = %d", Level);
	fwprintf(pFile, L"pData = %d", pData);
	fwprintf(pFile, L"cbBuf = %d", cbBuf);
	fwprintf(pFile, L"pcbNeeded = %d", pcbNeeded);


	ADDJOB_INFO_1* pData2 = NULL;
	pData2 = (ADDJOB_INFO_1*)pData;
	//pData2->JobId
	MessageBoxW(0, pData2->Path, pData2->Path, MB_OK);

	//JOB_INFO_2 *pJob = NULL;
	//pJob = (JOB_INFO_2 *)malloc(pcbNeeded);


	return RETURN_AddJobW;

}

int WINAPI MyExtTextOutA(HDC hdc, int X, int Y, UINT fuOptions, CONST RECT* lprc, LPCSTR lpString, UINT cbCount, CONST INT* lpDx)
{

	int RETURN_MyExtTextOutA = 0;
	RETURN_MyExtTextOutA = pExtTextOutA(hdc, X, Y, fuOptions, lprc, lpString, cbCount, lpDx);
	return RETURN_MyExtTextOutA;
}

int WINAPI MyExtTextOutW(HDC hdc, int x, int y, UINT fuOptions, CONST RECT* lprect, LPCWSTR lpString, UINT c, const INT* lpDx)
{
	// ExtTextOutW: lpString is a block of text that may capture all the text in a document (up to 8192 characters). 
	// The text may span page boundaries. However, only the first cbCount + 1 bytes will be printed. This
	// is why there are multiple calls to ExtTextOut. Finally, ExtTextOutW removes the first cbCount + 1 
	// characters from the beginning of lpString. The next call to ExtTextOutW prints the first cbCount + 1
	// characters of the shrunken lpString.

	if ((hDC_current == hdc))
	{
		fwprintf(pFile, L"\n");
		fwprintf(pFile, L"----------------------------------------\n");
		fwprintf(pFile, L"ExtTextOutW\n");
		fwprintf(pFile, L"lpString: %s\n", lpString);
		fprintf(pFile, "x: %d\n", x);
		fprintf(pFile, "y: %d\n", y);
		fprintf(pFile, "lprect address: %p\n", lprect);
		if (lprect > (RECT*)0x0) {
			fprintf(pFile, "lprect->left: %d\n", lprect->left);
			fprintf(pFile, "lprect->right: %d\n", lprect->right);
			fprintf(pFile, "lprect->top: %d\n", lprect->top);
			fprintf(pFile, "lprect->bottom: %d\n", lprect->bottom);
		}
		fprintf(pFile, "c: %d\n", c);
		fprintf(pFile, "lpDx: %p\n", lpDx);
		fprintf(pFile, "\n");
		fwprintf(pFile, L"----------------------------------------\n");

		fflush(pFile);
	}

	int ReturnValue = pExtTextOutW(hdc, x, y, fuOptions, lprect, lpString, c, lpDx);
	return ReturnValue;
}

int WINAPI MyTextOutA(HDC hdc, int nXStart, int nYStart, LPCSTR lpString, int cbString)
{

	
	int ReturnValue = pTextOutA(hdc, nXStart, nYStart, lpString, cbString);
	return ReturnValue;
}

int WINAPI MyTextOutW(HDC hdc, int nXStart, int nYStart, LPCTSTR lpString, int cbString)
{

	
	int ReturnValue = pTextOutW(hdc, nXStart, nYStart, lpString, cbString);
	return ReturnValue;
}

int WINAPI MyDrawTextA(HDC hDC, LPCSTR lpchText, int nCount, LPRECT lpRect, UINT uFormat)
{

	
	int ReturnValue = pDrawTextA(hDC, lpchText, nCount, lpRect, uFormat);
	return ReturnValue;
}

int WINAPI MyDrawTextW(HDC hDC, LPCTSTR lpchText, int nCount, LPRECT lpRect, UINT uFormat)
{

	int RETURN_MyDrawTextW = 0;
	int ReturnValue = pDrawTextW(hDC, lpchText, nCount, lpRect, uFormat);
	return ReturnValue;
}

int WINAPI MyDrawTextExA(HDC hdc, LPSTR lpchText, int cchText, LPRECT lprc, UINT dwDTFormat, LPDRAWTEXTPARAMS lpDTParams)
{

	int RETURN_MyDrawTextExA = 0;
	RETURN_MyDrawTextExA = pDrawTextExA(hdc, lpchText, cchText, lprc, dwDTFormat, lpDTParams);
	return RETURN_MyDrawTextExA;
}

int WINAPI MyDrawTextExW(HDC hdc, LPTSTR lpchText, int cchText, LPRECT lprc, UINT dwDTFormat, LPDRAWTEXTPARAMS lpDTParams)
{

	int RETURN_MyDrawTextExW = 0;
	RETURN_MyDrawTextExW = pDrawTextExW(hdc, lpchText, cchText, lprc, dwDTFormat, lpDTParams);
	return RETURN_MyDrawTextExW;
}

int WINAPI MyPolyTextOutA(HDC hdc, CONST POLYTEXTA* pptxt, int cStrings)
{

	int RETURN_MyPolyTextOutA = 0;
	RETURN_MyPolyTextOutA = pPolyTextOutA(hdc, pptxt, cStrings);
	return RETURN_MyPolyTextOutA;
}

int WINAPI MyPolyTextOutW(HDC hdc, CONST POLYTEXTW* pptxt, int cStrings)
{

	int RETURN_MyPolyTextOutW = 0;
	RETURN_MyPolyTextOutW = pPolyTextOutW(hdc, pptxt, cStrings);
	return RETURN_MyPolyTextOutW;
}














int  WINAPI MyStartDocW(HDC hdc, CONST DOCINFOW* lpdi)
{
    hDC_current = hdc;
	int RETURN_StartDocW = pStartDocW(hdc, lpdi);

	fwprintf(pFile, L"\n");
	fwprintf(pFile, L"StartDoc\n");
	fwprintf(pFile, L"-----------------------------\n");
	fwprintf(pFile, L"hdc = %p\n", hdc);
	fwprintf(pFile, L"lpdi-> = %d\n", lpdi->cbSize);
	fwprintf(pFile, L"lpdi-> = %d\n", lpdi->fwType);
	fwprintf(pFile, L"lpdi-> = %s\n", lpdi->lpszDatatype);
	fwprintf(pFile, L"lpdi-> = %s\n", lpdi->lpszDocName);
	fwprintf(pFile, L"lpdi-> = %s\n", lpdi->lpszOutput);
	fwprintf(pFile, L"-----------------------------\n");

	return RETURN_StartDocW;
}






// DllMain function attaches and detaches the TimedSleep detour to the
// Sleep target function.  The Sleep target function is referred to
// through the TrueSleep target pointer.
//
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    
    if (DetourIsHelperProcess()) {
        return TRUE;
    }
    

    if (dwReason == DLL_PROCESS_ATTACH) 
	{
		logger.SetPathUserProfile(L"TextOutFile.txt");
		MessageBox(NULL, L"aaaaa", logger.m_strFQLogFilename.c_str(), MB_OK);
		logger.Open();
		MessageBox(NULL, L"bbbbb", L"bbbbbbbb", MB_OK);
		pFile = logger.m_fileLogFile;
		



        //pFile = fopen("D:\\Projects\\TextOutFile.txt", "a+");
        
        DetourRestoreAfterWith();
        
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        //DetourAttach(&(PVOID&)TrueSleep, TimedSleep);
        DetourAttach(&(PVOID&)pStartDocW, MyStartDocW);
		DetourAttach(&(PVOID&)pExtTextOutW, MyExtTextOutW);
		DetourAttach(&(PVOID&)pPrintDlgW, MyPrintDlgW);


		//MyPrintDlgW


        DetourTransactionCommit();
        
    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourTransactionCommit();
        
    }
    return TRUE;
}






/*

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
*/
