// PostMost.h : main header file for the POSTMOST application
//

#if !defined(AFX_POSTMOST_H__1E486435_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
#define AFX_POSTMOST_H__1E486435_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

//#ifndef TITLE
#define TITLE "YENC-POWER-POST-A&A-v11b"
//#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CPostMostApp:
// See PostMost.cpp for the implementation of this class
//

class CPostMostView;

class CPostMostApp : public CWinApp
{
public:
	CPostMostApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPostMostApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	TCHAR			m_szAppDir[_MAX_PATH+1];
	CSplashWindow* m_pSplashWindow;
	CImageList      m_ImageList;
	CPostMostView*  m_pView;

	int m_nAboutBoxSoundNumber;

	void ApplySkin(const char* skin);

	//{{AFX_MSG(CPostMostApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSTMOST_H__1E486435_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
