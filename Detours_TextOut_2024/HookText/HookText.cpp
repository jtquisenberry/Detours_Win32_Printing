// dllmain.cpp : Defines the entry point for the DLL application.
#pragma comment(lib, "detours.lib")

#include "pch.h"
#include <windows.h>
#include "detours.h"
#include <stdio.h>

static LONG dwSlept = 0;

FILE* pFile;
HDC hDC_current = 0x0;



HDC hdc01;
LPPRINTDLG lppd01;
BOOL RETURN_PrintDlg;
LPPRINTDLGEXW lpprintdlgexw02;
LPPRINTDLGEXW global_lpprintdlgexw01;
LPPRINTDLGEXW global_lpprintdlgexw02;
HDC global_hDC01;
HWND global_hwnd01;
HANDLE global_phPrinter;
int intPrintAttempts = 1;
int int_SaveDC01;



//int global_TextOutCounter01 = 0;
int global_StartPageCounter = 0;






//Declaration of custom functions.
void ConvertDecToBin(int IntegerToConvert, char char_Output[]);


// Target pointer for the uninstrumented Sleep API.
//
static VOID(WINAPI* TrueSleep)(DWORD dwMilliseconds) = Sleep;
//int(WINAPI* pStartDocW)(HDC, CONST DOCINFOW*) = StartDocW;

//Here are the declarations of the original functions. The arguments of the prototype are just the data types
//of the arguments of the original function.
//It is possible to call the original function by the pointers defined below.
int(WINAPI* pStartDocW)(HDC, CONST DOCINFOW*) = StartDocW;
int(WINAPI* pShellAboutW)(HWND, LPCWSTR, LPCWSTR, HICON) = ShellAboutW;
int(WINAPI* pPrintDlgW)(LPPRINTDLGW) = PrintDlgW;
long(WINAPI* pPrintDlgExW)(LPPRINTDLGEXW) = PrintDlgExW;
int(WINAPI* pStartPage)(HDC) = StartPage;
int(WINAPI* pEndPage)(HDC) = EndPage;
int(WINAPI* pEndDoc)(HDC) = EndDoc;

BOOL(WINAPI* pAddJobW)(HANDLE, DWORD, LPBYTE, DWORD, LPDWORD) = AddJobW;
DWORD(WINAPI* pStartDocPrinterW)(HANDLE, DWORD, LPBYTE) = StartDocPrinterW;

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




void ConvertDecToBin(int IntegerToConvert, char char_Output[])
{

	char char_Temp[90] = "";

	for (int i = 31; i >= 0; i--)
	{
		int bit = ((IntegerToConvert >> i) & 1);
		sprintf(char_Temp, "%s%d", char_Temp, bit);

		if ((i % 4 == 0) & (i != 0))
		{
			sprintf(char_Temp, "%s,", char_Temp);
		}

	}

	sprintf(char_Output, "%s", char_Temp);

	return;
}









// Detour function that replaces the Sleep API.
//
VOID WINAPI TimedSleep(DWORD dwMilliseconds)
{
    // Save the before and after times around calling the Sleep API.
    DWORD dwBeg = GetTickCount();
    TrueSleep(dwMilliseconds);
    DWORD dwEnd = GetTickCount();

    InterlockedExchangeAdd(&dwSlept, dwEnd - dwBeg);
}





long WINAPI MyPrintDlgExW(LPPRINTDLGEXW lpprintdlgexw01)
{
	//
	//if (intPrintAttempts == 1)
	//{
	//	global_lpprintdlgexw02 = lpprintdlgexw01;
	//}
	//else
	//{
	//	lpprintdlgexw01 = global_lpprintdlgexw02;
	//}


	long RETURN_PrintDlgExW = 0;

	char char_lStructSize[250];
	char char_hwndOwner[250];
	char char_hDevMode[250];
	char char_hDC[250];
	char char_ExclusionFlags[250];
	char char_nPageRanges[250];
	char char_nMaxPageRanges[250];
	char char_nMinPage[250];
	char char_nMaxPage[250];
	char char_nCopies[250];
	char char_hInstance[250];
	char char_nPropertyPages[250];
	char char_nStartPage[250];
	char char_dwResultAction[250];


	//Begin Outputs
	fwprintf(pFile, L"\nLPPRINTDLGEXW Values Before PrintDlgExW\n");
	fwprintf(pFile, L"----------------------\n\n");
	fwprintf(pFile, L"lStructSize = %d\n", lpprintdlgexw01->lStructSize);
	fwprintf(pFile, L"hwndOwner = %d\n", lpprintdlgexw01->hwndOwner);


	fwprintf(pFile, L"hDevMode = %d\n", lpprintdlgexw01->hDevMode);
	DEVMODE* lpprintdlgexw_devmode01 = (DEVMODE*)GlobalLock(lpprintdlgexw01->hDevMode);
	//ASSERT(lpprintdlgexw_devmode01);

	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDeviceName[CCHDEVICENAME]=%s\n", lpprintdlgexw_devmode01->dmDeviceName);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmSpecVersion=%d\n", lpprintdlgexw_devmode01->dmSpecVersion);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDriverVersion=%d\n", lpprintdlgexw_devmode01->dmDriverVersion);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmSize=%d\n", lpprintdlgexw_devmode01->dmSize);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDriverExtra=%d\n", lpprintdlgexw_devmode01->dmDriverExtra);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmFields=%d\n", lpprintdlgexw_devmode01->dmFields);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmOrientation=%d\n", lpprintdlgexw_devmode01->dmOrientation);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPaperSize=%d\n", lpprintdlgexw_devmode01->dmPaperSize);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPaperLength=%d\n", lpprintdlgexw_devmode01->dmPaperLength);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPaperWidth=%d\n", lpprintdlgexw_devmode01->dmPaperWidth);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmScale=%d\n", lpprintdlgexw_devmode01->dmScale);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmCopies=%d\n", lpprintdlgexw_devmode01->dmCopies);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDefaultSource=%d\n", lpprintdlgexw_devmode01->dmDefaultSource);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPrintQuality=%d\n", lpprintdlgexw_devmode01->dmPrintQuality);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPosition.x=%d\n", lpprintdlgexw_devmode01->dmPosition.x);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPosition.y=%d\n", lpprintdlgexw_devmode01->dmPosition.y);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDisplayOrientation=%d\n", lpprintdlgexw_devmode01->dmDisplayOrientation);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDisplayFixedOutput=%d\n", lpprintdlgexw_devmode01->dmDisplayFixedOutput);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmColor=%d\n", lpprintdlgexw_devmode01->dmColor);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDuplex=%d\n", lpprintdlgexw_devmode01->dmDuplex);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmYResolution=%d\n", lpprintdlgexw_devmode01->dmYResolution);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmTTOption=%d\n", lpprintdlgexw_devmode01->dmTTOption);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmCollate=%d\n", lpprintdlgexw_devmode01->dmCollate);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmFormName[CCHFORMNAME]=%s\n", lpprintdlgexw_devmode01->dmFormName);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmLogPixels=%d\n", lpprintdlgexw_devmode01->dmLogPixels);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmBitsPerPel=%d\n", lpprintdlgexw_devmode01->dmBitsPerPel);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPelsWidth=%d\n", lpprintdlgexw_devmode01->dmPelsWidth);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPelsHeight=%d\n", lpprintdlgexw_devmode01->dmPelsHeight);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDisplayFlags=%d\n", lpprintdlgexw_devmode01->dmDisplayFlags);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmNup=%d\n", lpprintdlgexw_devmode01->dmNup);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDisplayFrequency=%d\n", lpprintdlgexw_devmode01->dmDisplayFrequency);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmICMMethod=%d\n", lpprintdlgexw_devmode01->dmICMMethod);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmICMIntent=%d\n", lpprintdlgexw_devmode01->dmICMIntent);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmMediaType=%d\n", lpprintdlgexw_devmode01->dmMediaType);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDitherType=%d\n", lpprintdlgexw_devmode01->dmDitherType);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmReserved1=%d\n", lpprintdlgexw_devmode01->dmReserved1);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmReserved2=%d\n", lpprintdlgexw_devmode01->dmReserved2);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPanningWidth=%d\n", lpprintdlgexw_devmode01->dmPanningWidth);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPanningHeight=%d\n", lpprintdlgexw_devmode01->dmPanningHeight);

	//Output the handle to DEVNAMES. Then output the string located at each of the offsets.
	fwprintf(pFile, L"hDevNames=%d\n", lpprintdlgexw01->hDevNames);
	DEVNAMES* lpprintdlgexw_devnames01 = (DEVNAMES*)::GlobalLock(lpprintdlgexw01->hDevNames);
	fwprintf(pFile, L"hDevNames->wDriverOffset=%s\n", (LPCTSTR)lpprintdlgexw_devnames01 + lpprintdlgexw_devnames01->wDriverOffset);
	fwprintf(pFile, L"hDevNames->wDeviceOffset=%s\n", (LPCTSTR)lpprintdlgexw_devnames01 + lpprintdlgexw_devnames01->wDeviceOffset);
	fwprintf(pFile, L"hDevNames->wOutputOffset=%s\n", (LPCTSTR)lpprintdlgexw_devnames01 + lpprintdlgexw_devnames01->wOutputOffset);
	fwprintf(pFile, L"hDevNames->wDefault=%s\n", (LPCTSTR)lpprintdlgexw_devnames01 + lpprintdlgexw_devnames01->wDefault);

	//Output the handle to the Device Context in both decimal and hexadecimal.
	fwprintf(pFile, L"hDC = %d = %h\n", lpprintdlgexw01->hDC, lpprintdlgexw01->hDC);

	//Output Flags as a binary formatted string.
	int int_Flags = 0;
	int_Flags = lpprintdlgexw01->Flags;
	WCHAR wsz_Flags[100];
	char char_Flags[100] = "";
	ConvertDecToBin(int_Flags, char_Flags);
	MultiByteToWideChar(CP_ACP, 0, char_Flags, strlen(char_Flags) + 1, wsz_Flags, sizeof(wsz_Flags) / sizeof(wsz_Flags[0]));
	fwprintf(pFile, L"Flags = %s\n", wsz_Flags);

	//Flags2 is unused in the API, so it will always be 0. There is no need to 
	//send Flags2 to the binary rendition function.
	fwprintf(pFile, L"Flags2 = %d\n", lpprintdlgexw01->Flags2);
	fwprintf(pFile, L"ExclusionFlags = %d\n", lpprintdlgexw01->ExclusionFlags);
	fwprintf(pFile, L"nPageRanges = %d\n", lpprintdlgexw01->nPageRanges);
	fwprintf(pFile, L"nMaxPageRanges = %d\n", lpprintdlgexw01->nMaxPageRanges);


	//LPPRINTPAGERANGE lpPageRanges;
	PRINTPAGERANGE* lpprintdlgex_lppageranges01 = (PRINTPAGERANGE*)::GlobalLock(lpprintdlgexw01->lpPageRanges);
	fwprintf(pFile, L"nPRINTPAGERANGE->nFromPage = %d\n", lpprintdlgex_lppageranges01->nFromPage);
	fwprintf(pFile, L"nPRINTPAGERANGE->nToPage = %d\n", lpprintdlgex_lppageranges01->nToPage);


	fwprintf(pFile, L"nMinPage = %d\n", lpprintdlgexw01->nMinPage);
	fwprintf(pFile, L"nMaxPage = %d\n", lpprintdlgexw01->nMaxPage);
	fwprintf(pFile, L"nCopies = %d\n", lpprintdlgexw01->nCopies);
	fwprintf(pFile, L"hInstance = %d\n", (int)lpprintdlgexw01->hInstance);
	fwprintf(pFile, L"nlpPrintTemplateName = %s\n", lpprintdlgexw01->lpPrintTemplateName);

	//Output the handle to the abort print function. It would be nice to output
	//the name of the function too.
	fwprintf(pFile, L"nlpCallback = %d\n", lpprintdlgexw01->lpCallback);

	//Output the handle to PropertyPages
	fwprintf(pFile, L"nPropertyPages = %d\n", lpprintdlgexw01->nPropertyPages);
	fwprintf(pFile, L"lphPropertyPages = %d\n", lpprintdlgexw01->lphPropertyPages);

	//It may be that the problem reading a member of PROPSHEETPAGE is that the values have not been initialized.
	//Or maybe structures are destroyed prior to the attempt to read.
	//PROPSHEETPAGE* lpprintdlgex_propertypages = (PROPSHEETPAGE*)::GlobalLock(lpprintdlgexw01->lphPropertyPages);

	//fwprintf(pFile, L"nPROPERTYPAGES->dwSize = %d\n",lpprintdlgex_propertypages->dwSize);

	//	//Output Flags as a binary formatted string.
	//	int int_PROPERTYPAGESFlags = 0;
	//	int_PROPERTYPAGESFlags = lpprintdlgex_propertypages->dwFlags;
	//	WCHAR wsz_PROPERTYPAGESFlags[100];
	//	char char_PROPERTYPAGESFlags[100] = "";
	//	ConvertDecToBin(int_PROPERTYPAGESFlags, char_PROPERTYPAGESFlags);
	//	MultiByteToWideChar( CP_ACP, 0, char_PROPERTYPAGESFlags, strlen(char_PROPERTYPAGESFlags)+1, wsz_PROPERTYPAGESFlags, sizeof(wsz_PROPERTYPAGESFlags)/sizeof(wsz_PROPERTYPAGESFlags[0]) );
	//
	//fwprintf(pFile, L"nPROPERTYPAGES->dwFlags = %d = %s\n",lpprintdlgex_propertypages->dwFlags, wsz_PROPERTYPAGESFlags);
	//fwprintf(pFile, L"nPROPERTYPAGES->hInstance = %d\n",lpprintdlgex_propertypages->hInstance);
	//fwprintf(pFile, L"nPROPERTYPAGES->pszTemplate = %s\n",lpprintdlgex_propertypages->pszTemplate);
	//fwprintf(pFile, L"nPROPERTYPAGES->pResource = %d\n",lpprintdlgex_propertypages->pResource);
	//fwprintf(pFile, L"nPROPERTYPAGES->hIcon = %d\n",lpprintdlgex_propertypages->hIcon);
	//fwprintf(pFile, L"nPROPERTYPAGES->pszIcon = %s\n",lpprintdlgex_propertypages->pszIcon);
	//fwprintf(pFile, L"nPROPERTYPAGES->pszTitle = %s\n",lpprintdlgex_propertypages->pszTitle);
	//fwprintf(pFile, L"nPROPERTYPAGES->pfnDlgProc = %d\n",lpprintdlgex_propertypages->pfnDlgProc);
	//fwprintf(pFile, L"nPROPERTYPAGES->lParam = %d\n",lpprintdlgex_propertypages->lParam);
	//fwprintf(pFile, L"nPROPERTYPAGES->pfnCallback = %d\n",lpprintdlgex_propertypages->pfnCallback);
	//fwprintf(pFile, L"nPROPERTYPAGES->pcRefParent = %d\n",lpprintdlgex_propertypages->pcRefParent);
	//fwprintf(pFile, L"nPROPERTYPAGES->pszHeaderTitle = %s\n",lpprintdlgex_propertypages->pszHeaderTitle);
	//fwprintf(pFile, L"nPROPERTYPAGES->pszHeaderSubTitle = %s\n",lpprintdlgex_propertypages->pszHeaderSubTitle);
	//fwprintf(pFile, L"nPROPERTYPAGES->pszbmHeader = %s\n",lpprintdlgex_propertypages->pszbmHeader);
	//fwprintf(pFile, L"nPROPERTYPAGES->pszbmSubTitle = %s\n",lpprintdlgex_propertypages->pszHeaderSubTitle);
	//fwprintf(pFile, L"nPROPERTYPAGES->hbmHeader = %d\n",lpprintdlgex_propertypages->hbmHeader);
	//fwprintf(pFile, L"nPROPERTYPAGES->hActCtx = %d\n",lpprintdlgex_propertypages->hActCtx);


	fwprintf(pFile, L"nStartPage = %d\n", lpprintdlgexw01->nStartPage);
	fwprintf(pFile, L"dwResultAction = %d\n", lpprintdlgexw01->dwResultAction);

	// I don't think this line does anything.
	//	global_lpprintdlgexw01=lpprintdlgexw01;


//Call the unlocks
	::GlobalUnlock(lpprintdlgexw01->hDevMode);
	::GlobalUnlock(lpprintdlgexw01->hDevNames);
	::GlobalUnlock(lpprintdlgexw01->lpPageRanges);


	//Call the real PrintDlgExW
	RETURN_PrintDlgExW = pPrintDlgExW(lpprintdlgexw01);
	//RETURN_PrintDlgExW = pPrintDlgExW(lpprintdlgexw01);

	//Save the handle of the window that called PrintDlgExW
	global_hwnd01 = lpprintdlgexw01->hwndOwner;

	//if this is the first attempt to print, then save the handle to the Device Context
	//for later use.
	if (intPrintAttempts == 1)
	{
		global_hDC01 = lpprintdlgexw01->hDC;
	}

	//Save the state of the Device context.
	int_SaveDC01 = SaveDC(lpprintdlgexw01->hDC);




	//GlobalLock 
	//DEVMODE* lpprintdlgexw_devmode02 = (DEVMODE*)GlobalLock(lpprintdlgexw01->hDevMode);
	//ASSERT(pDevMode);


//Begin Outputs
	fwprintf(pFile, L"\nLPPRINTDLGEXW Values AFTER PrintDlgExW\n");
	fwprintf(pFile, L"----------------------\n\n");
	fwprintf(pFile, L"lStructSize = %d\n", lpprintdlgexw01->lStructSize);
	fwprintf(pFile, L"hwndOwner = %d\n", lpprintdlgexw01->hwndOwner);


	fwprintf(pFile, L"hDevMode = %d\n", lpprintdlgexw01->hDevMode);
	DEVMODE* lpprintdlgexw_devmode02 = (DEVMODE*)GlobalLock(lpprintdlgexw01->hDevMode);
	//ASSERT(lpprintdlgexw_devmode01);

	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDeviceName[CCHDEVICENAME]=%s\n", lpprintdlgexw_devmode02->dmDeviceName);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmSpecVersion=%d\n", lpprintdlgexw_devmode02->dmSpecVersion);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDriverVersion=%d\n", lpprintdlgexw_devmode02->dmDriverVersion);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmSize=%d\n", lpprintdlgexw_devmode02->dmSize);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDriverExtra=%d\n", lpprintdlgexw_devmode02->dmDriverExtra);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmFields=%d\n", lpprintdlgexw_devmode02->dmFields);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmOrientation=%d\n", lpprintdlgexw_devmode02->dmOrientation);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPaperSize=%d\n", lpprintdlgexw_devmode02->dmPaperSize);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPaperLength=%d\n", lpprintdlgexw_devmode02->dmPaperLength);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPaperWidth=%d\n", lpprintdlgexw_devmode02->dmPaperWidth);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmScale=%d\n", lpprintdlgexw_devmode02->dmScale);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmCopies=%d\n", lpprintdlgexw_devmode02->dmCopies);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDefaultSource=%d\n", lpprintdlgexw_devmode02->dmDefaultSource);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPrintQuality=%d\n", lpprintdlgexw_devmode02->dmPrintQuality);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPosition.x=%d\n", lpprintdlgexw_devmode02->dmPosition.x);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPosition.y=%d\n", lpprintdlgexw_devmode02->dmPosition.y);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDisplayOrientation=%d\n", lpprintdlgexw_devmode02->dmDisplayOrientation);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDisplayFixedOutput=%d\n", lpprintdlgexw_devmode02->dmDisplayFixedOutput);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmColor=%d\n", lpprintdlgexw_devmode02->dmColor);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDuplex=%d\n", lpprintdlgexw_devmode02->dmDuplex);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmYResolution=%d\n", lpprintdlgexw_devmode02->dmYResolution);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmTTOption=%d\n", lpprintdlgexw_devmode02->dmTTOption);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmCollate=%d\n", lpprintdlgexw_devmode02->dmCollate);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmFormName[CCHFORMNAME]=%s\n", lpprintdlgexw_devmode02->dmFormName);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmLogPixels=%d\n", lpprintdlgexw_devmode02->dmLogPixels);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmBitsPerPel=%d\n", lpprintdlgexw_devmode02->dmBitsPerPel);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPelsWidth=%d\n", lpprintdlgexw_devmode02->dmPelsWidth);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPelsHeight=%d\n", lpprintdlgexw_devmode02->dmPelsHeight);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDisplayFlags=%d\n", lpprintdlgexw_devmode02->dmDisplayFlags);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmNup=%d\n", lpprintdlgexw_devmode02->dmNup);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDisplayFrequency=%d\n", lpprintdlgexw_devmode02->dmDisplayFrequency);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmICMMethod=%d\n", lpprintdlgexw_devmode02->dmICMMethod);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmICMIntent=%d\n", lpprintdlgexw_devmode02->dmICMIntent);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmMediaType=%d\n", lpprintdlgexw_devmode02->dmMediaType);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDitherType=%d\n", lpprintdlgexw_devmode02->dmDitherType);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmReserved1=%d\n", lpprintdlgexw_devmode02->dmReserved1);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmReserved2=%d\n", lpprintdlgexw_devmode02->dmReserved2);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPanningWidth=%d\n", lpprintdlgexw_devmode02->dmPanningWidth);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPanningHeight=%d\n", lpprintdlgexw_devmode02->dmPanningHeight);

	//Output the handle to DEVNAMES. Then output the string located at each of the offsets.
	fwprintf(pFile, L"hDevNames=%d\n", lpprintdlgexw01->hDevNames);
	DEVNAMES* lpprintdlgexw_devnames02 = (DEVNAMES*)::GlobalLock(lpprintdlgexw01->hDevNames);
	fwprintf(pFile, L"hDevNames->wDriverOffset=%s\n", (LPCTSTR)lpprintdlgexw_devnames01 + lpprintdlgexw_devnames02->wDriverOffset);
	fwprintf(pFile, L"hDevNames->wDeviceOffset=%s\n", (LPCTSTR)lpprintdlgexw_devnames01 + lpprintdlgexw_devnames02->wDeviceOffset);
	fwprintf(pFile, L"hDevNames->wOutputOffset=%s\n", (LPCTSTR)lpprintdlgexw_devnames01 + lpprintdlgexw_devnames02->wOutputOffset);
	fwprintf(pFile, L"hDevNames->wDefault=%s\n", (LPCTSTR)lpprintdlgexw_devnames01 + lpprintdlgexw_devnames02->wDefault);

	//Output the handle to the Device Context in both decimal and hexadecimal.
	fwprintf(pFile, L"hDC = %d = %h\n", lpprintdlgexw01->hDC, lpprintdlgexw01->hDC);

	//Output Flags as a binary formatted string.
	int int_Flags2 = 0;
	int_Flags2 = lpprintdlgexw01->Flags;
	WCHAR wsz_Flags2[100];
	char char_Flags2[100] = "";
	ConvertDecToBin(int_Flags2, char_Flags2);
	MultiByteToWideChar(CP_ACP, 0, char_Flags2, strlen(char_Flags2) + 1, wsz_Flags2, sizeof(wsz_Flags2) / sizeof(wsz_Flags2[0]));
	fwprintf(pFile, L"Flags = %s\n", wsz_Flags2);

	//Flags2 is unused in the API, so it will always be 0. There is no need to 
	//send Flags2 to the binary rendition function.
	fwprintf(pFile, L"Flags2 = %d\n", lpprintdlgexw01->Flags2);
	fwprintf(pFile, L"ExclusionFlags = %d\n", lpprintdlgexw01->ExclusionFlags);
	fwprintf(pFile, L"nPageRanges = %d\n", lpprintdlgexw01->nPageRanges);
	fwprintf(pFile, L"nMaxPageRanges = %d\n", lpprintdlgexw01->nMaxPageRanges);


	//LPPRINTPAGERANGE lpPageRanges;
	PRINTPAGERANGE* lpprintdlgex_lppageranges02 = (PRINTPAGERANGE*)::GlobalLock(lpprintdlgexw01->lpPageRanges);
	fwprintf(pFile, L"nPRINTPAGERANGE->nFromPage = %d\n", lpprintdlgex_lppageranges02->nFromPage);
	fwprintf(pFile, L"nPRINTPAGERANGE->nToPage = %d\n", lpprintdlgex_lppageranges02->nToPage);


	fwprintf(pFile, L"nMinPage = %d\n", lpprintdlgexw01->nMinPage);
	fwprintf(pFile, L"nMaxPage = %d\n", lpprintdlgexw01->nMaxPage);
	fwprintf(pFile, L"nCopies = %d\n", lpprintdlgexw01->nCopies);
	fwprintf(pFile, L"hInstance = %d\n", (int)lpprintdlgexw01->hInstance);
	fwprintf(pFile, L"nlpPrintTemplateName = %s\n", lpprintdlgexw01->lpPrintTemplateName);

	//Output the handle to the abort print function. It would be nice to output
	//the name of the function too.
	fwprintf(pFile, L"nlpCallback = %d\n", lpprintdlgexw01->lpCallback);

	//Output the handle to PropertyPages
	fwprintf(pFile, L"nPropertyPages = %d\n", lpprintdlgexw01->nPropertyPages);
	fwprintf(pFile, L"lphPropertyPages = %d\n", lpprintdlgexw01->lphPropertyPages);

	//It may be that the problem reading a member of PROPSHEETPAGE is that the values have not been initialized.
	//Or maybe structures are destroyed prior to the attempt to read.
	//PROPSHEETPAGE* lpprintdlgex_propertypages = (PROPSHEETPAGE*)::GlobalLock(lpprintdlgexw01->lphPropertyPages);

	//fwprintf(pFile, L"nPROPERTYPAGES->dwSize = %d\n",lpprintdlgex_propertypages->dwSize);

	//	//Output Flags as a binary formatted string.
	//	int int_PROPERTYPAGESFlags = 0;
	//	int_PROPERTYPAGESFlags = lpprintdlgex_propertypages->dwFlags;
	//	WCHAR wsz_PROPERTYPAGESFlags[100];
	//	char char_PROPERTYPAGESFlags[100] = "";
	//	ConvertDecToBin(int_PROPERTYPAGESFlags, char_PROPERTYPAGESFlags);
	//	MultiByteToWideChar( CP_ACP, 0, char_PROPERTYPAGESFlags, strlen(char_PROPERTYPAGESFlags)+1, wsz_PROPERTYPAGESFlags, sizeof(wsz_PROPERTYPAGESFlags)/sizeof(wsz_PROPERTYPAGESFlags[0]) );
	//
	//fwprintf(pFile, L"nPROPERTYPAGES->dwFlags = %d = %s\n",lpprintdlgex_propertypages->dwFlags, wsz_PROPERTYPAGESFlags);
	//fwprintf(pFile, L"nPROPERTYPAGES->hInstance = %d\n",lpprintdlgex_propertypages->hInstance);
	//fwprintf(pFile, L"nPROPERTYPAGES->pszTemplate = %s\n",lpprintdlgex_propertypages->pszTemplate);
	//fwprintf(pFile, L"nPROPERTYPAGES->pResource = %d\n",lpprintdlgex_propertypages->pResource);
	//fwprintf(pFile, L"nPROPERTYPAGES->hIcon = %d\n",lpprintdlgex_propertypages->hIcon);
	//fwprintf(pFile, L"nPROPERTYPAGES->pszIcon = %s\n",lpprintdlgex_propertypages->pszIcon);
	//fwprintf(pFile, L"nPROPERTYPAGES->pszTitle = %s\n",lpprintdlgex_propertypages->pszTitle);
	//fwprintf(pFile, L"nPROPERTYPAGES->pfnDlgProc = %d\n",lpprintdlgex_propertypages->pfnDlgProc);
	//fwprintf(pFile, L"nPROPERTYPAGES->lParam = %d\n",lpprintdlgex_propertypages->lParam);
	//fwprintf(pFile, L"nPROPERTYPAGES->pfnCallback = %d\n",lpprintdlgex_propertypages->pfnCallback);
	//fwprintf(pFile, L"nPROPERTYPAGES->pcRefParent = %d\n",lpprintdlgex_propertypages->pcRefParent);
	//fwprintf(pFile, L"nPROPERTYPAGES->pszHeaderTitle = %s\n",lpprintdlgex_propertypages->pszHeaderTitle);
	//fwprintf(pFile, L"nPROPERTYPAGES->pszHeaderSubTitle = %s\n",lpprintdlgex_propertypages->pszHeaderSubTitle);
	//fwprintf(pFile, L"nPROPERTYPAGES->pszbmHeader = %s\n",lpprintdlgex_propertypages->pszbmHeader);
	//fwprintf(pFile, L"nPROPERTYPAGES->pszbmSubTitle = %s\n",lpprintdlgex_propertypages->pszHeaderSubTitle);
	//fwprintf(pFile, L"nPROPERTYPAGES->hbmHeader = %d\n",lpprintdlgex_propertypages->hbmHeader);
	//fwprintf(pFile, L"nPROPERTYPAGES->hActCtx = %d\n",lpprintdlgex_propertypages->hActCtx);


	fwprintf(pFile, L"nStartPage = %d\n", lpprintdlgexw01->nStartPage);
	fwprintf(pFile, L"dwResultAction = %d\n", lpprintdlgexw01->dwResultAction);



	//update hDC here so it gets passed to subsequent functions.
	lpprintdlgexw01->hDC = global_hDC01;




	return RETURN_PrintDlgExW;

}

//Must specify the calling convention. 
//All Win32 APIs use WINAPI.
//Here are the user-defined replacement functions.
int WINAPI MyShellAboutW(HWND hWnd, LPCWSTR szApp, LPCWSTR szOtherStuff, HICON hIcon)
{
	MessageBoxA(0, (LPCSTR)"This is my hooked about box.", (LPCSTR)"Hooked About (Caption)", MB_OK | MB_ICONINFORMATION);

	long RETURN_PrintDlgExW = 0;
	//global_lpprintdlgexw01;
	RETURN_PrintDlgExW = pPrintDlgExW(global_lpprintdlgexw01);

	char char_global_lpprintdlgexw01[250];
	sprintf(char_global_lpprintdlgexw01, "hDC = %d", global_lpprintdlgexw01->hDC);
	MessageBoxA(0, (LPCSTR)char_global_lpprintdlgexw01, "ShellAbout", MB_OK);

	//	global_lpprintdlgexw01 = global_lpprintdlgexw01;
	return pShellAboutW(hWnd, szApp, szOtherStuff, hIcon);
}

int WINAPI MyStartPage(HDC hDC)
{

	int RETURN_MyStartPage = 0;
	global_StartPageCounter++;
	//global_TextOutCounter01 = 0;

	//Append mode
	pFile = fopen("E:\\VCPlusPlus\\ShellAboutDetour01\\RunMe\\TextOutFile.txt", "a+");
	fwprintf(pFile, L"StartPage\n\n");


	RETURN_MyStartPage = pStartPage(hDC);

	fwprintf(pFile, L"\n\nStartPage\n----------------------\n");
	fwprintf(pFile, L"hDC = %d\n", hDC);

	return RETURN_MyStartPage;
}

int WINAPI MyEndPage(HDC hDC)
{
	//hDC = global_hDC01;
	int RETURN_MyEndPage = 0;

	RETURN_MyEndPage = pEndPage(hDC);

	fwprintf(pFile, L"\n\nEndPage\n----------------------\n");
	fwprintf(pFile, L"hDC = %d\n", hDC);


	fwprintf(pFile, L"\n\nEnd of Page\n\n");
	fclose(pFile);
	//global_TextOutCounter01++;

	return RETURN_MyEndPage;
}

int WINAPI MyEndDoc(HDC hDC)
{
	//hDC = global_hDC01;
	int RETURN_EndDoc = 0;

	RETURN_EndDoc = pEndDoc(hDC);

	fwprintf(pFile, L"\n\nEndDoc\n----------------------\n");
	fwprintf(pFile, L"hDC = %d\n", hDC);

	return RETURN_EndDoc;
}



DWORD WINAPI MyStartDocPrinterW(HANDLE hPrinter, DWORD Level, LPBYTE pDocInfo)
{
	MessageBoxA(0, "Test", "Test", MB_OK);
	return 0;
}

int WINAPI MyPrintDlgW(LPPRINTDLGW lpprintdlgw01)
{
	int RETURN_PrintDlgW = 0;

	//lpprintdlgw01->Flags = 2097151; // = 1111,1111,1111,1111,1111,1
	RETURN_PrintDlgW = pPrintDlgW(lpprintdlgw01);

	
	fwprintf(pFile, L"\n\n");
	fwprintf(pFile, L"----------------------------\n");
	fwprintf(pFile, L"PrintDlgW\n");
	fwprintf(pFile, L"----------------------------\n");
	fwprintf(pFile, L"Flags = %#010x\n", lpprintdlgw01->Flags);
	fwprintf(pFile, L"hDC = %p\n", lpprintdlgw01->hDC);
	fwprintf(pFile, L"hDevMode = %p\n", lpprintdlgw01->hDevMode);
	fwprintf(pFile, L"hInstance = %p\n", lpprintdlgw01->hInstance);
	fwprintf(pFile, L"hPrintTemplate = %p\n", lpprintdlgw01->hPrintTemplate);
	fwprintf(pFile, L"hSetupTemplate = %p\n", lpprintdlgw01->hSetupTemplate);
	fwprintf(pFile, L"hwndOwner = %p\n", lpprintdlgw01->hwndOwner);
	fwprintf(pFile, L"lCustData = %p\n", lpprintdlgw01->lCustData);
	fwprintf(pFile, L"lpfnPrintHook = %p\n", lpprintdlgw01->lpfnPrintHook);
	fwprintf(pFile, L"lpfnSetupHook = %p\n", lpprintdlgw01->lpfnSetupHook);
	fwprintf(pFile, L"lpPrintTemplateName = %s\n", lpprintdlgw01->lpPrintTemplateName);
	fwprintf(pFile, L"lpSetupTemplateName = %s\n", lpprintdlgw01->lpSetupTemplateName);
	fwprintf(pFile, L"lStructSize = %d\n", lpprintdlgw01->lStructSize);
	fwprintf(pFile, L"nCopies = %d\n", lpprintdlgw01->nCopies);
	fwprintf(pFile, L"nFromPage = %d\n", lpprintdlgw01->nFromPage);
	fwprintf(pFile, L"nMaxPage = %d\n", lpprintdlgw01->nMaxPage);
	fwprintf(pFile, L"nMinPage = %d\n", lpprintdlgw01->nMinPage);
	fwprintf(pFile, L"nToPage = %d\n", lpprintdlgw01->nToPage);


	
	DEVMODE* lpprintdlgexw_devmode01 = (DEVMODE*)GlobalLock(lpprintdlgw01->hDevMode);
	

	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDeviceName [CCHDEVICENAME]=%s\n", lpprintdlgexw_devmode01->dmDeviceName);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmSpecVersion=%d\n", lpprintdlgexw_devmode01->dmSpecVersion);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDriverVersion=%d\n", lpprintdlgexw_devmode01->dmDriverVersion);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmSize=%d\n", lpprintdlgexw_devmode01->dmSize);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDriverExtra=%d\n", lpprintdlgexw_devmode01->dmDriverExtra);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmFields=%d\n", lpprintdlgexw_devmode01->dmFields);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmOrientation=%d\n", lpprintdlgexw_devmode01->dmOrientation);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPaperSize=%d\n", lpprintdlgexw_devmode01->dmPaperSize);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPaperLength=%d\n", lpprintdlgexw_devmode01->dmPaperLength);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPaperWidth=%d\n", lpprintdlgexw_devmode01->dmPaperWidth);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmScale=%d\n", lpprintdlgexw_devmode01->dmScale);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmCopies=%d\n", lpprintdlgexw_devmode01->dmCopies);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDefaultSource=%d\n", lpprintdlgexw_devmode01->dmDefaultSource);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPrintQuality=%d\n", lpprintdlgexw_devmode01->dmPrintQuality);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPosition.x=%d\n", lpprintdlgexw_devmode01->dmPosition.x);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPosition.y=%d\n", lpprintdlgexw_devmode01->dmPosition.y);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDisplayOrientation=%d\n", lpprintdlgexw_devmode01->dmDisplayOrientation);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDisplayFixedOutput=%d\n", lpprintdlgexw_devmode01->dmDisplayFixedOutput);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmColor=%d\n", lpprintdlgexw_devmode01->dmColor);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDuplex=%d\n", lpprintdlgexw_devmode01->dmDuplex);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmYResolution=%d\n", lpprintdlgexw_devmode01->dmYResolution);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmTTOption=%d\n", lpprintdlgexw_devmode01->dmTTOption);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmCollate=%d\n", lpprintdlgexw_devmode01->dmCollate);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmFormName[CCHFORMNAME]=%s\n", lpprintdlgexw_devmode01->dmFormName);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmLogPixels=%d\n", lpprintdlgexw_devmode01->dmLogPixels);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmBitsPerPel=%d\n", lpprintdlgexw_devmode01->dmBitsPerPel);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPelsWidth=%d\n", lpprintdlgexw_devmode01->dmPelsWidth);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPelsHeight=%d\n", lpprintdlgexw_devmode01->dmPelsHeight);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDisplayFlags=%d\n", lpprintdlgexw_devmode01->dmDisplayFlags);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmNup=%d\n", lpprintdlgexw_devmode01->dmNup);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDisplayFrequency=%d\n", lpprintdlgexw_devmode01->dmDisplayFrequency);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmICMMethod=%d\n", lpprintdlgexw_devmode01->dmICMMethod);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmICMIntent=%d\n", lpprintdlgexw_devmode01->dmICMIntent);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmMediaType=%d\n", lpprintdlgexw_devmode01->dmMediaType);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmDitherType=%d\n", lpprintdlgexw_devmode01->dmDitherType);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmReserved1=%d\n", lpprintdlgexw_devmode01->dmReserved1);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmReserved2=%d\n", lpprintdlgexw_devmode01->dmReserved2);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPanningWidth=%d\n", lpprintdlgexw_devmode01->dmPanningWidth);
	fwprintf(pFile, L"LPPRINTDLGEXW->DEVMODE->dmPanningHeight=%d\n", lpprintdlgexw_devmode01->dmPanningHeight);

	//Output the handle to DEVNAMES. Then output the string located at each of the offsets.
	//fwprintf(pFile, L"hDevNames=%d\n", lpprintdlgw01->hDevNames);
	DEVNAMES* lpprintdlgexw_devnames01 = (DEVNAMES*)::GlobalLock(lpprintdlgw01->hDevNames);
	fwprintf(pFile, L"hDevNames->wDriverOffset=%s\n", (LPCTSTR)lpprintdlgexw_devnames01 + lpprintdlgexw_devnames01->wDriverOffset);
	fwprintf(pFile, L"hDevNames->wDeviceOffset=%s\n", (LPCTSTR)lpprintdlgexw_devnames01 + lpprintdlgexw_devnames01->wDeviceOffset);
	fwprintf(pFile, L"hDevNames->wOutputOffset=%s\n", (LPCTSTR)lpprintdlgexw_devnames01 + lpprintdlgexw_devnames01->wOutputOffset);
	fwprintf(pFile, L"hDevNames->wDefault=%s\n", (LPCTSTR)lpprintdlgexw_devnames01 + lpprintdlgexw_devnames01->wDefault);

	//Output the handle to the Device Context in both decimal and hexadecimal.
	fwprintf(pFile, L"hDC = %d = %h\n", lpprintdlgw01->hDC, lpprintdlgw01->hDC);








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

	int RETURN_MyExtTextOutW = 0;
	
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

	RETURN_MyExtTextOutW = pExtTextOutW(hdc, x, y, fuOptions, lprect, lpString, c, lpDx);
	return RETURN_MyExtTextOutW;
}

int WINAPI MyTextOutA(HDC hdc, int nXStart, int nYStart, LPCSTR lpString, int cbString)
{

	int RETURN_MyTextOutA = 0;
	RETURN_MyTextOutA = pTextOutA(hdc, nXStart, nYStart, lpString, cbString);
	return RETURN_MyTextOutA;
}

int WINAPI MyTextOutW(HDC hdc, int nXStart, int nYStart, LPCTSTR lpString, int cbString)
{

	int RETURN_MyTextOutW = 0;
	RETURN_MyTextOutW = pTextOutW(hdc, nXStart, nYStart, lpString, cbString);
	return RETURN_MyTextOutW;
}

int WINAPI MyDrawTextA(HDC hDC, LPCSTR lpchText, int nCount, LPRECT lpRect, UINT uFormat)
{

	int RETURN_MyDrawTextA = 0;
	RETURN_MyDrawTextA = pDrawTextA(hDC, lpchText, nCount, lpRect, uFormat);
	return RETURN_MyDrawTextA;
}

int WINAPI MyDrawTextW(HDC hDC, LPCTSTR lpchText, int nCount, LPRECT lpRect, UINT uFormat)
{

	int RETURN_MyDrawTextW = 0;
	RETURN_MyDrawTextW = pDrawTextW(hDC, lpchText, nCount, lpRect, uFormat);
	return RETURN_MyDrawTextW;
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
    

    if (dwReason == DLL_PROCESS_ATTACH) {
        
        
        pFile = fopen("D:\\Projects\\TextOutFile.txt", "a+");
        
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
        DetourDetach(&(PVOID&)TrueSleep, TimedSleep);
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
