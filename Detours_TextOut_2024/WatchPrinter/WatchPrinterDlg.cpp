#include "stdafx.h"
#include "WatchPrinter.h"
#include "WatchPrinterDlg.h"

#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const UINT UDM_UPDATE_JOB_LIST = RegisterWindowMessage(_T("UDM_UPDATE_JOB_LIST"));

/////////////////////////////////////////////////////////////////////////////
// CWatchPrinterDlg dialog

CWatchPrinterDlg::CWatchPrinterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWatchPrinterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWatchPrinterDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    
    m_rectOrig.SetRectEmpty();

    m_pEventThreadDone    = NULL;
    m_pEventStopRequested = NULL;
}

void CWatchPrinterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWatchPrinterDlg)
	DDX_Control(pDX, IDC_BTN_STOP, m_btnStop);
	DDX_Control(pDX, IDC_BTN_START, m_btnStart);
	DDX_Control(pDX, IDC_CB_PRINTERS, m_cbPrinters);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_LC_JOBINFO, m_lcJobInfo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWatchPrinterDlg, CDialog)
	//{{AFX_MSG_MAP(CWatchPrinterDlg)
	ON_WM_SYSCOMMAND()
  	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_START, OnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, OnStop)
	ON_CBN_SELCHANGE(IDC_CB_PRINTERS, OnSelchangePrinters)
	//}}AFX_MSG_MAP
    ON_REGISTERED_MESSAGE(UDM_UPDATE_JOB_LIST, OnUpdateJobList)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWatchPrinterDlg message handlers

BOOL CWatchPrinterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
 
    GetClientRect(m_rectOrig);
    m_nWidth  = m_rectOrig.Width();
    m_nHeight = m_rectOrig.Height();

    m_lcJobInfo.InsertColumn(0, _T("Job Id"), LVCFMT_RIGHT, 45);
    m_lcJobInfo.InsertColumn(1, _T("User"), LVCFMT_LEFT, 75);
    m_lcJobInfo.InsertColumn(2, _T("From"), LVCFMT_LEFT, 80);
    m_lcJobInfo.InsertColumn(3, _T("Document"), LVCFMT_LEFT, 125);
    m_lcJobInfo.InsertColumn(4, _T("Submitted @"), LVCFMT_LEFT, 80);
    m_lcJobInfo.InsertColumn(5, _T("Pages printed"), LVCFMT_RIGHT, 80);
    m_lcJobInfo.InsertColumn(6, _T("Bytes printed"), LVCFMT_RIGHT, 110);
    m_lcJobInfo.InsertColumn(7, _T("Status"), LVCFMT_LEFT, 150);

    m_pEventThreadDone = new CEvent(TRUE, TRUE);     // signaled
    m_pEventStopRequested = new CEvent(FALSE, TRUE); // non-signaled

    m_ThreadInfo.SetStopRequestedEvent(m_pEventStopRequested->m_hObject);
    m_ThreadInfo.SetThreadDoneEvent(m_pEventThreadDone->m_hObject);
    m_ThreadInfo.SetHwnd(GetSafeHwnd());

    EnumeratePrinters();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//====================================================================

void CWatchPrinterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

//====================================================================

void CWatchPrinterDlg::EnumeratePrinters( void )
{
    DWORD dwNeeded, 
          dwReturned;

    
    EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 1, NULL, 0, &dwNeeded, &dwReturned);
    
    LPBYTE lpBuffer = new BYTE[dwNeeded];
    EnumPrinters(PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS, NULL, 1, lpBuffer, dwNeeded, &dwNeeded, &dwReturned);

    PPRINTER_INFO_1 p1 = (PPRINTER_INFO_1) lpBuffer;
    
    for (DWORD x = 0; x < dwReturned; x++)
    {
        m_cbPrinters.AddString(p1->pName);

        p1++;
    }
    
    delete lpBuffer;
}

//====================================================================

UINT ThreadFunc( LPVOID pParam )
{
    CWatchPrinterDlg *pDlg = (CWatchPrinterDlg *) pParam;
    
    return pDlg->ThreadFunc();
}

//====================================================================

void CWatchPrinterDlg::OnSelchangePrinters() 
{
    m_btnStart.EnableWindow(TRUE);
}

//====================================================================

void CWatchPrinterDlg::OnStart()
{
    m_btnStart.EnableWindow(FALSE);
    m_btnStop.EnableWindow(TRUE);
    m_cbPrinters.EnableWindow(FALSE);

    m_mapJobInfo.Cleanup();
    
    // set the events to non-signaled
    m_pEventStopRequested->ResetEvent();
    m_pEventThreadDone->ResetEvent();


    HANDLE hPrinter;
    CString strPrinter;

    m_cbPrinters.GetWindowText(strPrinter);
    OpenPrinter((LPTSTR) (LPCTSTR) strPrinter, &hPrinter, NULL);
    
    m_ThreadInfo.SetPrinter(hPrinter);


    m_pWinThread = AfxBeginThread(::ThreadFunc, this);
}

//====================================================================

void CWatchPrinterDlg::OnStop() 
{
    // signal and wait for ThreadFunc() to end 
    m_pEventStopRequested->SetEvent();
    WaitForSingleObject(m_pEventThreadDone->m_hObject, 8000U);

    if (m_ThreadInfo.GetPrinter() != INVALID_HANDLE_VALUE)
        ClosePrinter(m_ThreadInfo.GetPrinter());
    
    m_btnStart.EnableWindow(TRUE);
    m_btnStop.EnableWindow(FALSE);
    m_cbPrinters.EnableWindow(TRUE);
}

//====================================================================

void CWatchPrinterDlg::OnCancel() 
{
    OnStop();

    delete m_pEventStopRequested;
    delete m_pEventThreadDone;

    m_mapJobInfo.Cleanup();

    CDialog::OnCancel();
}

//====================================================================

UINT CWatchPrinterDlg::ThreadFunc( void )
{
	PPRINTER_NOTIFY_INFO pNotification = NULL;

	WORD JobFields[] = 
    {
        JOB_NOTIFY_FIELD_PRINTER_NAME,
        JOB_NOTIFY_FIELD_MACHINE_NAME,
        JOB_NOTIFY_FIELD_PORT_NAME,
        JOB_NOTIFY_FIELD_USER_NAME,
        JOB_NOTIFY_FIELD_NOTIFY_NAME,
        JOB_NOTIFY_FIELD_DATATYPE,
        JOB_NOTIFY_FIELD_PRINT_PROCESSOR,
        JOB_NOTIFY_FIELD_PARAMETERS,
        JOB_NOTIFY_FIELD_DRIVER_NAME,
        JOB_NOTIFY_FIELD_DEVMODE,
        JOB_NOTIFY_FIELD_STATUS,
        JOB_NOTIFY_FIELD_STATUS_STRING,
        JOB_NOTIFY_FIELD_DOCUMENT,
        JOB_NOTIFY_FIELD_PRIORITY,
        JOB_NOTIFY_FIELD_POSITION,
        JOB_NOTIFY_FIELD_SUBMITTED,
        JOB_NOTIFY_FIELD_START_TIME,
        JOB_NOTIFY_FIELD_UNTIL_TIME,
        JOB_NOTIFY_FIELD_TIME,
        JOB_NOTIFY_FIELD_TOTAL_PAGES,
        JOB_NOTIFY_FIELD_PAGES_PRINTED,
        JOB_NOTIFY_FIELD_TOTAL_BYTES,
        JOB_NOTIFY_FIELD_BYTES_PRINTED
    };
	PRINTER_NOTIFY_OPTIONS_TYPE	Notifications[1] =					
    {
        {
            JOB_NOTIFY_TYPE,
            0,
            0,
            0,
            sizeof(JobFields) / sizeof(JobFields[0]),
            JobFields
        },
    };
	PRINTER_NOTIFY_OPTIONS NotificationOptions = 
    {
        2,
        PRINTER_NOTIFY_OPTIONS_REFRESH,
        sizeof(Notifications) / sizeof(Notifications[0]),
        Notifications
    };

    // get a handle to a printer change notification object.
    HANDLE hChange = FindFirstPrinterChangeNotification(m_ThreadInfo.GetPrinter(),
                                                        PRINTER_CHANGE_ALL,
                                                        0, 
                                                        &NotificationOptions);

    DWORD dwChange;
    HANDLE aHandles[2];
    aHandles[0] = hChange;
    aHandles[1] = m_ThreadInfo.GetStopRequestedEvent();

    while (hChange != INVALID_HANDLE_VALUE)
    {
        // sleep until a printer change notification wakes this thread or the
        // event becomes set indicating it's time for the thread to end.
        WaitForMultipleObjects(2, aHandles, FALSE, INFINITE);

        if (WaitForSingleObject(hChange, 0U) == WAIT_OBJECT_0)
        {
            FindNextPrinterChangeNotification(hChange, &dwChange, &NotificationOptions, (LPVOID *) &pNotification);

			if (pNotification != NULL)
            {
			    // if a notification overflow occurred,
			    if (pNotification->Flags & PRINTER_NOTIFY_INFO_DISCARDED)
			    {
				    DWORD dwOldFlags = NotificationOptions.Flags;
				    
				    // we must refresh to continue
				    NotificationOptions.Flags = PRINTER_NOTIFY_OPTIONS_REFRESH;

				    FreePrinterNotifyInfo(pNotification);

				    FindNextPrinterChangeNotification(hChange, &dwChange, &NotificationOptions, (LPVOID *) &pNotification);

				    NotificationOptions.Flags = dwOldFlags;
			    }

                // iterate through each notification
                for (DWORD x = 0; x < pNotification->Count; x++)
                {
                    ASSERT(pNotification->aData[x].Type == JOB_NOTIFY_TYPE);

                    CJobInfo *pJobInfo = NULL;

                    // if the job info item does not exist, create a new one
                    if (! m_mapJobInfo.Lookup(pNotification->aData[x].Id, pJobInfo))
                    {
                        pJobInfo = new CJobInfo(pNotification->aData[x].Id);

                        m_mapJobInfo.SetAt(pNotification->aData[x].Id, pJobInfo);
                    }

                    ASSERT(pJobInfo != NULL);
                    pJobInfo->UpdateInfo(&pNotification->aData[x]);

                    ::PostMessage(m_ThreadInfo.GetHwnd(), UDM_UPDATE_JOB_LIST, 0, 0);
                }
            }

			FreePrinterNotifyInfo(pNotification);
			pNotification = NULL;
        }
        else if (WaitForSingleObject(m_ThreadInfo.GetStopRequestedEvent(), 0U) == WAIT_OBJECT_0)
        {
			FindClosePrinterChangeNotification(hChange);
            hChange = INVALID_HANDLE_VALUE;
        }
    }

    // signal the event to let the primary thread know that this thread is done
    SetEvent(m_ThreadInfo.GetThreadDoneEvent());

    return 0;
}

//====================================================================

LRESULT CWatchPrinterDlg::OnUpdateJobList( WPARAM, LPARAM )
{
    m_lcJobInfo.DeleteAllItems();

    POSITION pos = m_mapJobInfo.GetStartPosition();
    while (pos != NULL)
    {
        int nKey;
        CJobInfo *pJobInfo;
        m_mapJobInfo.GetNextAssoc(pos, nKey, pJobInfo);

        ASSERT(pJobInfo != NULL);

        CString strText;
        strText.Format(_T("%03d"), pJobInfo->GetJobId());
        int nItem = m_lcJobInfo.InsertItem(m_lcJobInfo.GetItemCount(), strText);

        m_lcJobInfo.SetItemText(nItem, 1, pJobInfo->GetUser());
        
        strText.Format(_T("%s on %s"), pJobInfo->GetPort(), pJobInfo->GetMachine());
        m_lcJobInfo.SetItemText(nItem, 2, strText);
        
        m_lcJobInfo.SetItemText(nItem, 3, pJobInfo->GetDocument());

        if (pJobInfo->GetTimeSubmitted().wHour != USHRT_MAX)
        {
            SYSTEMTIME st1 = pJobInfo->GetTimeSubmitted(),
                       st2;

            SystemTimeToTzSpecificLocalTime(NULL, &st1, &st2);
            strText.Format(_T("%02u:%02u:%02u"), st2.wHour, st2.wMinute, st2.wSecond);
            m_lcJobInfo.SetItemText(nItem, 4, strText);
        }

        strText.Format(_T("%d of %d"), pJobInfo->GetPagesPrinted(), pJobInfo->GetTotalPages());
        m_lcJobInfo.SetItemText(nItem, 5, strText);

        strText.Format(_T("%d of %d"), pJobInfo->GetBytesPrinted(), pJobInfo->GetTotalBytes());
        m_lcJobInfo.SetItemText(nItem, 6, strText);

        m_lcJobInfo.SetItemText(nItem, 7, pJobInfo->GetStatus());
    }

    return 0;
}

//====================================================================

void CWatchPrinterDlg::RepositionChildControl( CWnd *pWnd, const int dx, const int dy, const UINT uAnchor )
{
    // make sure the dialog exists
    if (NULL != pWnd->m_hWnd)
    {
        CRect   rect;

        pWnd->GetWindowRect(rect);
        ScreenToClient(rect);

        if (uAnchor & resizeVERT)        // 0x0020
            rect.bottom += dy;
        else if (uAnchor & anchorBOTTOM) // 0x0004
            rect.OffsetRect(0, dy);
        
        if (uAnchor & resizeHORZ)        // 0x0010
            rect.right += dx;
        else if (uAnchor & anchorRIGHT)  // 0x0002
            rect.OffsetRect(dx, 0);

        pWnd->MoveWindow(rect);
    }
}

//====================================================================

void CWatchPrinterDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	CDialog::OnGetMinMaxInfo(lpMMI);

    // since OnGetMinMaxInfo() gets called very early, make sure we have valid numbers
    if (! m_rectOrig.IsRectEmpty())
    {
        // establish a minimum size
        lpMMI->ptMinTrackSize.x = m_rectOrig.Width() + (GetSystemMetrics(SM_CXFRAME) * 2);
        lpMMI->ptMinTrackSize.y = m_rectOrig.Height() + GetSystemMetrics(SM_CYCAPTION) + (GetSystemMetrics(SM_CXFRAME) * 2);
    }
}

//====================================================================

void CWatchPrinterDlg::OnSize(UINT nType, int cx, int cy) 
{
    CDialog::OnSize(nType, cx, cy);
    
    // how much did the dialog grow/shrink by?
    int nWidthOffset  = cx - m_nWidth;
    int nHeightOffset = cy - m_nHeight;

    RepositionChildControl(&m_lcJobInfo, nWidthOffset, nHeightOffset, anchorTOP | anchorRIGHT | resizeBOTH);
    RepositionChildControl(&m_btnStart, nWidthOffset, nHeightOffset, anchorTOP | anchorRIGHT);
    RepositionChildControl(&m_btnStop, nWidthOffset, nHeightOffset, anchorTOP | anchorRIGHT);
    RepositionChildControl(&m_btnCancel, nWidthOffset, nHeightOffset, anchorTOP | anchorRIGHT);

    // save dialog's new dimensions
    m_nWidth  = cx;
    m_nHeight = cy;
}
