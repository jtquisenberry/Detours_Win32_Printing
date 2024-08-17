// WatchPrinter.h : main header file for the WATCHPRINTER application
//

#if !defined(AFX_WATCHPRINTER_H__42172D7F_48AF_49C6_8B1E_82308FF94A53__INCLUDED_)
#define AFX_WATCHPRINTER_H__42172D7F_48AF_49C6_8B1E_82308FF94A53__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CWatchPrinterApp:
// See WatchPrinter.cpp for the implementation of this class
//

class CWatchPrinterApp : public CWinApp
{
public:
	CWatchPrinterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchPrinterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWatchPrinterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATCHPRINTER_H__42172D7F_48AF_49C6_8B1E_82308FF94A53__INCLUDED_)
