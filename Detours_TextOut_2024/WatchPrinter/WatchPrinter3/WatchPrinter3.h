
// WatchPrinter3.h : main header file for the WatchPrinter3 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CWatchPrinter3App:
// See WatchPrinter3.cpp for the implementation of this class
//

class CWatchPrinter3App : public CWinAppEx
{
public:
	CWatchPrinter3App() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CWatchPrinter3App theApp;
