// SplashWindow.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "SplashWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplashWindow

CSplashWindow::CSplashWindow()
{
}

CSplashWindow::~CSplashWindow()
{
}


BEGIN_MESSAGE_MAP(CSplashWindow, CWnd)
	//{{AFX_MSG_MAP(CSplashWindow)
	ON_WM_TIMER()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CSplashWindow, CWnd)
/////////////////////////////////////////////////////////////////////////////
// CSplashWindow message handlers

BOOL CSplashWindow::Create()
{
	//m_Bitmap.LoadBitmap(IDB_BITMAP_SPLASH);
	m_Bitmap.LoadBitmap(IDB_BITMAP_ABOUT);
	BITMAP bm;	
	m_Bitmap.GetObject(sizeof(BITMAP), &bm);
	
	// First create an invisible window
	m_wndInvisible.CreateEx(WS_EX_TOPMOST, AfxRegisterWndClass(CS_CLASSDC), _T(""), WS_POPUP, 0, 0, bm.bmWidth, bm.bmHeight, NULL, NULL);
	
	// Create the the splash window with invisible parent as parent
	BOOL bRetVal = CWnd::CreateEx(WS_EX_TOPMOST|WS_EX_CLIENTEDGE, AfxRegisterWndClass(CS_CLASSDC), 			_T(""), WS_POPUP, 0, 0, bm.bmWidth, bm.bmHeight, m_wndInvisible.m_hWnd, NULL);

	CenterWindow();

	RECT rectFrom, rectTo;

	GetWindowRect(&rectTo);
	
	int mid_x = rectTo.left + ((rectTo.right - rectTo.left) / 2);
	int mid_y = rectTo.top + ((rectTo.bottom - rectTo.top) / 2);

	rectFrom.left = mid_x - 2;
	rectFrom.right = mid_x + 2;
	rectFrom.top = mid_y - 2;
	rectFrom.bottom = mid_y + 2;

	::CM_DrawWireRects(&rectFrom, &rectTo, 8, 30);

	ShowWindow(SW_SHOW);	
	UpdateWindow();		
	m_nTimerID = SetTimer(1, 3000, NULL);	
	ASSERT(m_nTimerID);	
	return bRetVal;
}

BOOL CSplashWindow::PreTranslateMessage(MSG* pMsg) 
{
	ASSERT(pMsg != NULL);		
	if (pMsg->message == WM_KEYDOWN ||
		pMsg->message == WM_SYSKEYDOWN ||			
		pMsg->message == WM_LBUTTONDOWN ||
		pMsg->message == WM_RBUTTONDOWN ||			
		pMsg->message == WM_MBUTTONDOWN )	
	{
		//Destroy the timer and splash window		
		KillTimer(m_nTimerID);
		m_wndInvisible.DestroyWindow(); 			
		delete this;		
		return 1;	
	}	 	
	return CWnd::PreTranslateMessage(pMsg);	
}

void CSplashWindow::OnTimer(UINT nIDEvent) 
{
	if (m_nTimerID == nIDEvent)	
	{
		KillTimer(m_nTimerID);		
		m_wndInvisible.DestroyWindow(); 			
		delete this;
		return;	
	}	 
	
	CWnd::OnTimer(nIDEvent);
}

void CSplashWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CDC memDC;
	memDC.CreateCompatibleDC( &dc );
	CBitmap *pBmpOld = memDC.SelectObject( &m_Bitmap );
	CRect rcWnd;	
	GetWindowRect( &rcWnd );
	dc.BitBlt(0, 0, rcWnd.Width(), rcWnd.Height(), &memDC, 0, 0,SRCCOPY);
	memDC.SelectObject( pBmpOld );	
}
