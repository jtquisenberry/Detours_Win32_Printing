// WatchPrinterDlg.h : header file
//

#if !defined(AFX_WATCHPRINTERDLG_H__150079A7_803D_4E1C_804E_301C01EBBF58__INCLUDED_)
#define AFX_WATCHPRINTERDLG_H__150079A7_803D_4E1C_804E_301C01EBBF58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ThreadInfo.h"
#include "MapEx.h"
#include "JobInfo.h"

/////////////////////////////////////////////////////////////////////////////
// CWatchPrinterDlg dialog

class CWatchPrinterDlg : public CDialog
{
// Construction
public:
	CWatchPrinterDlg(CWnd* pParent = NULL);	// standard constructor

    UINT ThreadFunc( void );

// Dialog Data
	//{{AFX_DATA(CWatchPrinterDlg)
	enum { IDD = IDD_WATCHPRINTER_DIALOG };
	CButton	m_btnStop;
	CButton	m_btnStart;
	CComboBox	m_cbPrinters;
	CButton	m_btnCancel;
	CListCtrl	m_lcJobInfo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatchPrinterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CWatchPrinterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnSelchangePrinters();
	virtual void OnCancel();
	//}}AFX_MSG
    afx_msg LRESULT OnUpdateJobList( WPARAM, LPARAM );
	DECLARE_MESSAGE_MAP()

private:
    CRect m_rectOrig;
    int m_nWidth;
    int m_nHeight;

    CEvent *m_pEventThreadDone;
    CEvent *m_pEventStopRequested;
    CWinThread *m_pWinThread;
    CThreadInfo m_ThreadInfo;    
    
    CMapEx<int, int, CJobInfo*, CJobInfo*> m_mapJobInfo;

    enum AnchorFlags 
    {
        anchorLEFT   = 0x0001,
        anchorTOP    = 0x0001,
        anchorRIGHT  = 0x0002,
        anchorBOTTOM = 0x0004,
        resizeHORZ   = 0x0010,
        resizeVERT   = 0x0020,
        resizeBOTH   = (resizeHORZ | resizeVERT)
	};

    void EnumeratePrinters( void );
    void RepositionChildControl( CWnd *pWnd, const int dx, const int dy, const UINT uAnchor );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATCHPRINTERDLG_H__150079A7_803D_4E1C_804E_301C01EBBF58__INCLUDED_)
