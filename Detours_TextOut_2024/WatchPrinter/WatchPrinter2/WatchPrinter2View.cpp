
// WatchPrinter2View.cpp : implementation of the CWatchPrinter2View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "WatchPrinter2.h"
#endif

#include "WatchPrinter2Doc.h"
#include "WatchPrinter2View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWatchPrinter2View

IMPLEMENT_DYNCREATE(CWatchPrinter2View, CView)

BEGIN_MESSAGE_MAP(CWatchPrinter2View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CWatchPrinter2View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CWatchPrinter2View construction/destruction

CWatchPrinter2View::CWatchPrinter2View() noexcept
{
	// TODO: add construction code here

}

CWatchPrinter2View::~CWatchPrinter2View()
{
}

BOOL CWatchPrinter2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CWatchPrinter2View drawing

void CWatchPrinter2View::OnDraw(CDC* /*pDC*/)
{
	CWatchPrinter2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CWatchPrinter2View printing


void CWatchPrinter2View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CWatchPrinter2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CWatchPrinter2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CWatchPrinter2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CWatchPrinter2View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CWatchPrinter2View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CWatchPrinter2View diagnostics

#ifdef _DEBUG
void CWatchPrinter2View::AssertValid() const
{
	CView::AssertValid();
}

void CWatchPrinter2View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWatchPrinter2Doc* CWatchPrinter2View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWatchPrinter2Doc)));
	return (CWatchPrinter2Doc*)m_pDocument;
}
#endif //_DEBUG


// CWatchPrinter2View message handlers
