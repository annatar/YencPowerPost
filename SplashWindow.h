#if !defined(AFX_SPLASHWINDOW_H__15AB2B51_CAE0_11D2_88DE_00A0CC24E368__INCLUDED_)
#define AFX_SPLASHWINDOW_H__15AB2B51_CAE0_11D2_88DE_00A0CC24E368__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplashWindow.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSplashWindow window

class CSplashWindow : public CWnd
{
// Construction
public:
	CSplashWindow();
	BOOL Create();

// Attributes
public:
	CWnd m_wndInvisible;
	UINT m_nTimerID;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashWindow)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSplashWindow();
	CBitmap m_Bitmap;

	// Generated message map functions
protected:
	//{{AFX_MSG(CSplashWindow)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_DYNCREATE(CSplashWindow);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLASHWINDOW_H__15AB2B51_CAE0_11D2_88DE_00A0CC24E368__INCLUDED_)
