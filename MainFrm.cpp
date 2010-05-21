// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "PostMost.h"

#include "MainFrm.h"
#include "PostMostDoc.h"
#include "PostMostView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_ICONNOTIFY, OnIconNotification)
	ON_MESSAGE(WM_SENDTO_TRAY, OnSendToTray)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_TASK,
	ID_INDICATOR_FILES,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bTrayIconActive = FALSE;
	m_hTrayIcon_READY = AfxGetApp()->LoadIcon(IDI_ICON_TRAY_READY);
	m_hTrayIcon_WAITING = AfxGetApp()->LoadIcon(IDI_ICON_TRAY_WAITING);
	m_hTrayIcon_POSTING = AfxGetApp()->LoadIcon(IDI_ICON_TRAY_POSTING);
	m_hTrayIcon_ERROR = AfxGetApp()->LoadIcon(IDI_ICON_TRAY_ERROR);

}

CMainFrame::~CMainFrame()
{
	
	::DestroyIcon(m_hTrayIcon_READY);
	::DestroyIcon(m_hTrayIcon_WAITING);
	::DestroyIcon(m_hTrayIcon_POSTING);
	::DestroyIcon(m_hTrayIcon_ERROR);

}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText(TITLE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// Load Window Placement Information
	if(LoadWindowPlacement() == TRUE) SetWindowPlacement(&m_WindowPlacement);

	//m_wndToolBar.LoadBitmap(IDB_SKIN2_TOOLBAR);
	SIZE bsize, isize;
	isize.cx=20;
	isize.cy=20;
	bsize.cx=isize.cx+7;
	bsize.cy=isize.cy+6;
	//m_wndToolBar.SetSizes(bsize,isize);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	SetProgressIndicator("");
	return 0;
}

void CMainFrame::OnSendToTray( WPARAM wParam, LPARAM lParam )
{
	ShowWindow(SW_MINIMIZE);
	ShowWindow(SW_HIDE);
}

LRESULT CMainFrame::OnIconNotification( WPARAM wParam, LPARAM lParam)
{
	if((wParam == ID_ICONNOTIFY) && (lParam == WM_LBUTTONDOWN))
	{
		if(IsIconic())
		{
			ShowWindow(SW_SHOW);
			ShowWindow(SW_RESTORE);			
		}
	}
	return Default();
}

void CMainFrame::SetStatusText(LPCTSTR szText)
{
	m_wndStatusBar.SetPaneText(0, szText);
	TRACE("Status Text: %s\n", szText);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
BOOL CMainFrame::LoadWindowPlacement()
{
	// Get window placement setting from Registry
	CString strKeyValue = AfxGetApp()->GetProfileString("Settings","WindowPos");

	// Return FALSE if nothing found in Registry
	if(strKeyValue.IsEmpty()) return FALSE;

	// Parse the 10 param for the WINDOWPLACEMENT structure
	int nCount = _stscanf(strKeyValue,"%i:%i:%i:%i:%i:%i:%i:%i:%i:%i",
		&m_WindowPlacement.flags,
		&m_WindowPlacement.showCmd,
		&m_WindowPlacement.ptMinPosition.x,
		&m_WindowPlacement.ptMinPosition.y,
		&m_WindowPlacement.ptMaxPosition.x,
		&m_WindowPlacement.ptMaxPosition.y,
		&m_WindowPlacement.rcNormalPosition.left,
		&m_WindowPlacement.rcNormalPosition.top,
		&m_WindowPlacement.rcNormalPosition.right,
		&m_WindowPlacement.rcNormalPosition.bottom);
	
	// Verify All 10 elements were read
	if(nCount != 10) return FALSE;

	// Verify window placement (Not off screen or anything like that)
	int Screen_Width  = ::GetSystemMetrics(SM_CXSCREEN);
	int Screen_Height = ::GetSystemMetrics(SM_CYSCREEN);

	if((m_WindowPlacement.rcNormalPosition.left < 0) ||
       (m_WindowPlacement.rcNormalPosition.left >= Screen_Width))
		m_WindowPlacement.rcNormalPosition.left = 0;

	if((m_WindowPlacement.rcNormalPosition.right < 0) ||
       (m_WindowPlacement.rcNormalPosition.right >= Screen_Width))
		m_WindowPlacement.rcNormalPosition.right = Screen_Width - 1;

	if(m_WindowPlacement.rcNormalPosition.left >= m_WindowPlacement.rcNormalPosition.right)
	{
		m_WindowPlacement.rcNormalPosition.left = 0;
		m_WindowPlacement.rcNormalPosition.right = Screen_Width - 1;
	}

	if((m_WindowPlacement.rcNormalPosition.top < 0) ||
       (m_WindowPlacement.rcNormalPosition.top >= Screen_Height))
		m_WindowPlacement.rcNormalPosition.top = 0;

	if((m_WindowPlacement.rcNormalPosition.bottom < 0) ||
       (m_WindowPlacement.rcNormalPosition.bottom >= Screen_Height))
		m_WindowPlacement.rcNormalPosition.bottom = Screen_Height / 2;

	if(m_WindowPlacement.rcNormalPosition.top >= m_WindowPlacement.rcNormalPosition.bottom)
	{
		m_WindowPlacement.rcNormalPosition.top = 0;
		m_WindowPlacement.rcNormalPosition.bottom = Screen_Height / 2;
	}

	// Success
	return TRUE;
}

BOOL CMainFrame::SaveWindowPlacement()
{
	CString strKeyValue;

	if(IsZoomed()) m_WindowPlacement.flags |= WPF_RESTORETOMAXIMIZED;

	strKeyValue.Format("%i:%i:%i:%i:%i:%i:%i:%i:%i:%i",
		m_WindowPlacement.flags,
		m_WindowPlacement.showCmd,
		m_WindowPlacement.ptMinPosition.x,
		m_WindowPlacement.ptMinPosition.y,
		m_WindowPlacement.ptMaxPosition.x,
		m_WindowPlacement.ptMaxPosition.y,
		m_WindowPlacement.rcNormalPosition.left,
		m_WindowPlacement.rcNormalPosition.top,
		m_WindowPlacement.rcNormalPosition.right,
		m_WindowPlacement.rcNormalPosition.bottom);

	return( AfxGetApp()->WriteProfileString("Settings","WindowPos", strKeyValue) );
}

void CMainFrame::OnClose() 
{
	CPostMostView* pView = (CPostMostView*) GetActiveView();
	if(pView->m_bRunning == TRUE)
	{
		if(AfxMessageBox("Posting In Progress!\n\nStop Posting And Quit?", MB_ICONEXCLAMATION | MB_OKCANCEL) == IDCANCEL)
			return;
		
		pView->OnPostStop();
	}

	pView->OnEncodeStop();


	// Save Window Placement before exiting
	if(IsIconic() == FALSE)
	{
		if(GetWindowPlacement(&m_WindowPlacement) == TRUE)
			SaveWindowPlacement();
	}

	pView->SaveTasksToQueue();

	pView->RemoveAllTasks(FALSE);

#ifdef INCLUDESOUND
	if(pView->m_Settings.m_bSound)
	{
		CWaitCursor wc;
		PlaySound((LPCTSTR) IDR_WAVE_BELL, NULL, SND_RESOURCE | SND_ASYNC | SND_SYNC);
		::Sleep(1500);
	}
#endif

	pView->SaveColumnWidthsToSettings();
	pView->m_Settings.SaveToRegistry();
	pView->StopWaitTimer();

	// Remove Tray Notification Icon
	NotifyIcon(NIM_DELETE, IDI_ICON_TRAY_READY);

	CFrameWnd::OnClose();
}

void CMainFrame::GetButtonRect(UINT bid, LPRECT rvRect)
{
	CToolBarCtrl& theToolbarCtrl = m_wndToolBar.GetToolBarCtrl();
	theToolbarCtrl.GetItemRect(theToolbarCtrl.CommandToIndex(bid), rvRect);
	theToolbarCtrl.ClientToScreen(rvRect);
}
/*
void CMainFrame::GetPropertiesButtonRect(LPRECT rvRect)
{
	CToolBarCtrl& theToolbarCtrl = m_wndToolBar.GetToolBarCtrl();
	theToolbarCtrl.GetItemRect(4, rvRect);
	theToolbarCtrl.ClientToScreen(rvRect);
}

void CMainFrame::GetProgressDlgButtonRect(LPRECT rvRect)
{
	CToolBarCtrl& theToolbarCtrl = m_wndToolBar.GetToolBarCtrl();
	theToolbarCtrl.GetItemRect(17, rvRect);
	theToolbarCtrl.ClientToScreen(rvRect);
}

void CMainFrame::GetEncodesDlgButtonRect(LPRECT rvRect)
{
	CToolBarCtrl& theToolbarCtrl = m_wndToolBar.GetToolBarCtrl();
	theToolbarCtrl.GetItemRect(18, rvRect);
	theToolbarCtrl.ClientToScreen(rvRect);
}

void CMainFrame::GetHistoryButtonRect(LPRECT rvRect)
{
	CToolBarCtrl& theToolbarCtrl = m_wndToolBar.GetToolBarCtrl();
	theToolbarCtrl.GetItemRect(2, rvRect);
	theToolbarCtrl.ClientToScreen(rvRect);
}

void CMainFrame::GetHelpButtonRect(LPRECT rvRect)
{
	CToolBarCtrl& theToolbarCtrl = m_wndToolBar.GetToolBarCtrl();
	theToolbarCtrl.GetItemRect(19, rvRect);
	theToolbarCtrl.ClientToScreen(rvRect);
}
*/
void CMainFrame::SetProgressIndicator(LPCTSTR szIndicatorText)
{
	m_wndStatusBar.SetPaneText(1, szIndicatorText);
}

void CMainFrame::SysTrayEnable(BOOL bEnabled)
{
	if(bEnabled)
	{
		// Want SysTray Icon
		if(!m_bTrayIconActive)
			NotifyIcon(NIM_ADD, IDI_ICON_TRAY_READY);
	}
	else
	{
		// Want No SysTray Icon
		if(m_bTrayIconActive)
			NotifyIcon(NIM_DELETE, IDI_ICON_TRAY_READY);
	}
}

BOOL CMainFrame::NotifyIcon(UINT uMessage, int nIconID)
{
	CPostMostApp* pApp = (CPostMostApp*) AfxGetApp();
	NOTIFYICONDATA nid;

	// Do nothing if icon handle not set
	if(m_hTrayIcon_READY == (HICON) NULL) return FALSE;
	if(m_hTrayIcon_POSTING == (HICON) NULL) return FALSE;
	if(m_hTrayIcon_ERROR == (HICON) NULL) return FALSE;
	
	switch(uMessage)
	{
	case NIM_ADD:
		nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		m_bTrayIconActive = TRUE;
		break;
	case NIM_MODIFY:
		nid.uFlags = NIF_ICON | NIF_TIP;
		break;
	case NIM_DELETE:
		nid.uFlags = 0;
		m_bTrayIconActive = FALSE;
		break;
	default:
		break;
	}

	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.uID = ID_ICONNOTIFY;
	nid.hWnd = m_hWnd;
	nid.uCallbackMessage = WM_ICONNOTIFY;

	switch(nIconID)
	{
	case IDI_ICON_TRAY_READY:
		nid.hIcon = m_hTrayIcon_READY;
		break;
	case IDI_ICON_TRAY_WAITING:
		nid.hIcon = m_hTrayIcon_WAITING;
		break;
	case IDI_ICON_TRAY_POSTING:
		nid.hIcon = m_hTrayIcon_POSTING;
		break;
	case IDI_ICON_TRAY_ERROR:
		nid.hIcon = m_hTrayIcon_ERROR;
		break;
	}
	::CopyMemory(nid.szTip, TITLE, strlen(TITLE)+1);

	return Shell_NotifyIcon(uMessage, &nid);
}

void CMainFrame::OnSysCommand( UINT nID, LONG lParam)
{
	if(m_bTrayIconActive)
	{
		if(nID == SC_MINIMIZE)
		{
			// CFrameWnd::OnSysCommand(nID, lParam);
			PostMessage(WM_SENDTO_TRAY, 0, 0);
			return;
		}
		else
		if((nID == SC_RESTORE) || (nID == SC_MAXIMIZE))
		{
			// Ensure window has WS_VISIBLE style bit
			CFrameWnd::OnSysCommand(nID, lParam);
			return;
		}
	}
	/*
	else
	{
		switch(nID & 0xFFF0)
		{
			// Handle 'About' command
			case IDM_SYSMENU_SRV_ABOUT:
				((CCOSMICSRV_App*) AfxGetApp())->OnAppAbout();
				return;

			// Handle 'Start' command
			case IDM_SYSMENU_SRV_START:
				((CCOSMICSRV_View*) GetActiveView())->Server_Start();
				return;

			// Handle 'Stop' command
			case IDM_SYSMENU_SRV_STOP:
				((CCOSMICSRV_View*) GetActiveView())->Server_Stop();
				return;

			// Handle 'Properties' command
			case IDM_SYSMENU_SRV_PROP:
				((CCOSMICSRV_View*) GetActiveView())->Server_Properties();
				return;

			case IDM_SYSMENU_SRV_TELNET:
				((CCOSMICSRV_View*) GetActiveView())->Server_Telnet();
				return;

			default:
				// TRACE for debugging purposes
				TRACE2("OnSysCommand: nID = %d, lParam = %d\n", nID, lParam);
				break;

		}
	}
	*/
	// Do default handling for anything else...
	CFrameWnd::OnSysCommand(nID, lParam);
}
