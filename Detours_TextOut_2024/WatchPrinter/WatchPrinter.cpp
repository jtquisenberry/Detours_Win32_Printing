#include "stdafx.h"
#include "WatchPrinter.h"
#include "WatchPrinterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWatchPrinterApp

BEGIN_MESSAGE_MAP(CWatchPrinterApp, CWinApp)
	//{{AFX_MSG_MAP(CWatchPrinterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWatchPrinterApp construction

CWatchPrinterApp::CWatchPrinterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWatchPrinterApp object

CWatchPrinterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWatchPrinterApp initialization

BOOL CWatchPrinterApp::InitInstance()
{
	CWatchPrinterDlg dlg;

	m_pMainWnd = &dlg;
	
    dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
