// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__1E486439_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
#define AFX_MAINFRM_H__1E486439_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	
	WINDOWPLACEMENT		m_WindowPlacement;


// Operations
public:
	BOOL SaveWindowPlacement();
	BOOL LoadWindowPlacement();

	void GetButtonRect(UINT bid,LPRECT rvRect);
	//void GetPropertiesButtonRect(LPRECT rvRect);
	//void GetProgressDlgButtonRect(LPRECT rvRect);
	//void GetEncodesDlgButtonRect(LPRECT rvRect);
	//void GetHistoryButtonRect(LPRECT rvRect);
	//void GetHelpButtonRect(LPRECT rvRect);

	void SetProgressIndicator(LPCTSTR szIndicatorText);

	BOOL NotifyIcon(UINT uMessage, int nIconID);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();

	void        SysTrayEnable(BOOL bEnabled);

	void        SetStatusText(LPCTSTR szText);

	BOOL        m_bTrayIconActive;
	HICON		m_hTrayIcon_READY;
	HICON		m_hTrayIcon_WAITING;
	HICON		m_hTrayIcon_POSTING;
	HICON		m_hTrayIcon_ERROR;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg void OnSysCommand( UINT, LONG );
	afx_msg LRESULT OnIconNotification( WPARAM, LPARAM );
	afx_msg void OnSendToTray( WPARAM, LPARAM );
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__1E486439_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
