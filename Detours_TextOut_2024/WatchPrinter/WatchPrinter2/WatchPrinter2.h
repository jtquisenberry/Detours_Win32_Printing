
// WatchPrinter2.h : main header file for the WatchPrinter2 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CWatchPrinter2App:
// See WatchPrinter2.cpp for the implementation of this class
//

class CWatchPrinter2App : public CWinAppEx
{
public:
	CWatchPrinter2App() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CWatchPrinter2App theApp;
