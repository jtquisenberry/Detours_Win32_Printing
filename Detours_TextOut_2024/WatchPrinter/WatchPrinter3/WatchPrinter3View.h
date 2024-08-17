
// WatchPrinter3View.h : interface of the CWatchPrinter3View class
//

#pragma once


class CWatchPrinter3View : public CView
{
protected: // create from serialization only
	CWatchPrinter3View() noexcept;
	DECLARE_DYNCREATE(CWatchPrinter3View)

// Attributes
public:
	CWatchPrinter3Doc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CWatchPrinter3View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in WatchPrinter3View.cpp
inline CWatchPrinter3Doc* CWatchPrinter3View::GetDocument() const
   { return reinterpret_cast<CWatchPrinter3Doc*>(m_pDocument); }
#endif

