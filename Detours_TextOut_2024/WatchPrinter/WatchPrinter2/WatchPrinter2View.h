
// WatchPrinter2View.h : interface of the CWatchPrinter2View class
//

#pragma once


class CWatchPrinter2View : public CView
{
protected: // create from serialization only
	CWatchPrinter2View() noexcept;
	DECLARE_DYNCREATE(CWatchPrinter2View)

// Attributes
public:
	CWatchPrinter2Doc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CWatchPrinter2View();
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

#ifndef _DEBUG  // debug version in WatchPrinter2View.cpp
inline CWatchPrinter2Doc* CWatchPrinter2View::GetDocument() const
   { return reinterpret_cast<CWatchPrinter2Doc*>(m_pDocument); }
#endif

