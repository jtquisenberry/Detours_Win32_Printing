
// WatchPrinter3View.cpp : implementation of the CWatchPrinter3View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "WatchPrinter3.h"
#endif

#include "WatchPrinter3Doc.h"
#include "WatchPrinter3View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWatchPrinter3View

IMPLEMENT_DYNCREATE(CWatchPrinter3View, CView)

BEGIN_MESSAGE_MAP(CWatchPrinter3View, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CWatchPrinter3View construction/destruction

CWatchPrinter3View::CWatchPrinter3View() noexcept
{
	// TODO: add construction code here

}

CWatchPrinter3View::~CWatchPrinter3View()
{
}

BOOL CWatchPrinter3View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CWatchPrinter3View drawing

void CWatchPrinter3View::OnDraw(CDC* /*pDC*/)
{
	CWatchPrinter3Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

void CWatchPrinter3View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CWatchPrinter3View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CWatchPrinter3View diagnostics

#ifdef _DEBUG
void CWatchPrinter3View::AssertValid() const
{
	CView::AssertValid();
}

void CWatchPrinter3View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWatchPrinter3Doc* CWatchPrinter3View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWatchPrinter3Doc)));
	return (CWatchPrinter3Doc*)m_pDocument;
}
#endif //_DEBUG


// CWatchPrinter3View message handlers
