// EncodingProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MainFrm.h"
#include "PostMost.h"
#include "PostMostDoc.h"
#include "PostMostView.h"
#include "EncodingProgressDlg.h"
#include "EncoderThread.h"
#include "par2format.h"

#include <sys/timeb.h>
#include <sys/types.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEncodingProgressDlg dialog


CEncodingProgressDlg::CEncodingProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEncodingProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEncodingProgressDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pView = (CPostMostView*) NULL;
	m_uiTimerID = 0;
	m_rectAnimateFrom.SetRectEmpty();
	m_nTimeToGoLastUpdated = CTime::GetCurrentTime();
	m_bForceUpdateTimeToGo = TRUE; // So it is updated on new dialog invocation

	last_worker = NULL;
	last_batch = -1;
	//last_bytes = 0;
	//last_time = 0;

}


void CEncodingProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEncodingProgressDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEncodingProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CEncodingProgressDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEncodingProgressDlg message handlers

void CEncodingProgressDlg::OnOK() 
{
	OnCancel();
}

void CEncodingProgressDlg::OnCancel() 
{
#ifdef INCLUDESOUND
	if(m_pView->m_Settings.m_bSound)
		PlaySound((LPCTSTR) IDR_WAVE_VAULT, NULL, SND_RESOURCE | SND_ASYNC);
#endif

	if(m_pView != (CPostMostView*) NULL)
		m_pView->m_pEncodeDlg = (CEncodingProgressDlg*) NULL;

	Stop_Update_Timer();

	ShowWindow(SW_HIDE);

	if(!m_rectAnimateFrom.IsRectEmpty())
	{
		// Update Rectangle in case main window moved..
		CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
		pMainFrame->GetButtonRect(ID_POST_THREADS,m_rectAnimateFrom);

		CRect rectWindow;
		GetWindowRect(&rectWindow);
		::CM_DrawWireRects(&rectWindow, &m_rectAnimateFrom, 16, 15);
	}

	CDialog::OnCancel();
}
/*
CBitmap* CEncodingProgressDlg::CreateBitmapForDlgItem(int nID)
{
	CWnd* pTargetWnd = GetDlgItem(nID);
	CDC* pWndCDC = pTargetWnd->GetDC();
	CBitmap* pNewCBitmap = new CBitmap();
	RECT TargetRect;
	pTargetWnd->GetClientRect(&TargetRect);

	// TCHAR szMessage[64];
	// sprintf(szMessage, "TICK = %08X", GetTickCount());

	if(pNewCBitmap->CreateCompatibleBitmap(pWndCDC, TargetRect.right, TargetRect.bottom) == TRUE)
	{
		CDC MemDC;
		MemDC.CreateCompatibleDC(pWndCDC);
		
		// Create Font
		LOGFONT lf;
		::ZeroMemory(&lf, sizeof(LOGFONT));
		lf.lfHeight = - MulDiv( 7, MemDC.GetDeviceCaps(LOGPIXELSX), 72);
		lf.lfWidth             = 0;
		lf.lfEscapement        = 0;
		lf.lfOrientation       = 0;
		lf.lfWeight            = FW_NORMAL;
		lf.lfItalic            = FALSE;
		lf.lfUnderline         = FALSE;
		lf.lfStrikeOut         = FALSE;
		lf.lfCharSet           = DEFAULT_CHARSET;
		lf.lfOutPrecision      = OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision     = CLIP_DEFAULT_PRECIS;
		lf.lfQuality           = DEFAULT_QUALITY;
		lf.lfPitchAndFamily    = VARIABLE_PITCH | FF_MODERN;

		m_StatusFont.CreateFontIndirect( &lf );

		BITMAP bmpInfo;
		pNewCBitmap->GetBitmap(&bmpInfo);
		int w = bmpInfo.bmWidth;
		int h = bmpInfo.bmHeight;

		CBitmap* pOldBMP = MemDC.SelectObject(pNewCBitmap);
		MemDC.FillSolidRect(0, 0, w, h, RGB(0,0,0));
		MemDC.SetTextColor(RGB(223,223,223));
		MemDC.TextOut(4,(h/2) - 12, "Initializing ...", 16);
		MemDC.SelectObject(pOldBMP);
	} 
	else 
	{ 
		delete pNewCBitmap; 
		pNewCBitmap = (CBitmap*) NULL; 
	}
	pTargetWnd->ReleaseDC(pWndCDC);

	return(pNewCBitmap);
}
*/

BOOL CEncodingProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CenterWindow();

	if(!m_rectAnimateFrom.IsRectEmpty())
	{
		CRect rectWindow;
		GetWindowRect(&rectWindow);
		::CM_DrawWireRects(&m_rectAnimateFrom, &rectWindow, 16, 15);
	}

	CListCtrl* list = (CListCtrl*)GetDlgItem(IDC_LIST_ENCODING);
	list->InsertColumn(0, "Volumes .par2", LVCFMT_LEFT, 320, -1);

	OnTimer(IDTIMER_UPDATE_PROGRESS);
	Start_Update_Timer();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEncodingProgressDlg::Start_Update_Timer()
{
	m_uiTimerID = SetTimer(IDTIMER_UPDATE_PROGRESS, 1000, NULL);
}

void CEncodingProgressDlg::Stop_Update_Timer()
{
	KillTimer(m_uiTimerID);
}

void CEncodingProgressDlg::OnTimer(UINT nIDEvent) 
{

	bool enable = true;
	CEncoderThread* worker = NULL;

	if(nIDEvent != IDTIMER_UPDATE_PROGRESS) enable = false;
	else
	{
		CPostMostView* view = m_pView;
		if (view == NULL) enable = false;
		else
		{
			worker = view->m_pEncodingThread;
			if (worker == NULL) enable = false;
			else enable = true;
		}
	}

	GetDlgItem(IDC_PROGRESS_BATCH)->EnableWindow(enable);
	GetDlgItem(IDC_PROGRESS_BATCH2)->EnableWindow(enable);
	GetDlgItem(IDC_EDIT_STATUS)->EnableWindow(enable);
	GetDlgItem(IDC_LIST_ENCODING)->EnableWindow(enable);
	
	if (enable)
	{
		CSingleLock lock(&(worker->m_Mutex),TRUE);
		static int last_batch = -1;

		par2::u64 bytes = worker->stat_octets_processed;
		timeb btime;
		ftime(&btime);
		int curtime = btime.time*1000+btime.millitm;

		if (last_batch != worker->stat_numbatch)
		{
			last_batch = worker->stat_numbatch;
			((CProgressCtrl*) GetDlgItem(IDC_PROGRESS_BATCH))->SetRange(0,worker->stat_TotalFiles);
			((CProgressCtrl*) GetDlgItem(IDC_PROGRESS_BATCH2))->SetRange(0,1000);

			CListCtrl* list = (CListCtrl*)GetDlgItem(IDC_LIST_ENCODING);

			list->DeleteAllItems();

			for (int f=0;f<worker->stat_VolumeFiles.GetSize();f++)
			{
				list->InsertItem(f,worker->stat_VolumeFiles.GetAt(f));
			}

			for (int t=0;t<NSAMPLE;t++)
			{
				last_bytes[t] = bytes;
				last_time[t] = curtime;
			}
		}

		((CProgressCtrl*) GetDlgItem(IDC_PROGRESS_BATCH))->SetPos(worker->stat_CurrentFileNum);
		((CProgressCtrl*) GetDlgItem(IDC_PROGRESS_BATCH2))->SetPos(worker->stat_CurrentPer1000);

		int timediff = (int)(curtime-last_time[0]);
		if (timediff>0)
		{
			par2::u64 bdiff = (bytes-last_bytes[0]);
			CString buf;
			buf.Format("%.2f MB/s",((double)((__int64)bdiff)*1000)/(double(timediff)*1024*1024));
			SetDlgItemText(IDC_TEXT_SPEED,buf);
		}
		for (int t=0;t<NSAMPLE-1;t++)
		{
			last_bytes[t]=last_bytes[t+1];
			last_time[t]=last_time[t+1];
		}
		last_bytes[NSAMPLE-1]=bytes;
		last_time[NSAMPLE-1]=curtime;

		if (worker->stat_CurrentFile[0]!='\0')
		{
			
			//CString buf;
			//buf.Format("Processing file %s...",worker->stat_CurrentFile);
			//SetDlgItemText(IDC_EDIT_STATUS,buf);
			SetDlgItemText(IDC_EDIT_STATUS,worker->stat_CurrentFile);
		}
		else
		{
			SetDlgItemText(IDC_EDIT_STATUS,"Ready");
			SetDlgItemText(IDC_TEXT_SPEED,"");
		}
	}
	else
	{
		SetDlgItemText(IDC_TEXT_SPEED,"");
	}
		// Update Time Elapsed
		//CTime curTime = CTime::GetCurrentTime();
		//CTimeSpan timeDiff = curTime - (m_pView->m_Time_Started);
		//sprintf(szTT, "%02d:%02d:%02d", timeDiff.GetHours(), timeDiff.GetMinutes(), timeDiff.GetSeconds());
		//GetDlgItem(IDC_STATIC_TIME_ELAPSED)->SetWindowText(szTT);

		// Update Time To Go
		/*
		CTimeSpan timeDiff2 = curTime - m_nTimeToGoLastUpdated;
		if((timeDiff2.GetTotalSeconds() >= 10) || (m_bForceUpdateTimeToGo == TRUE))
		{
			m_bForceUpdateTimeToGo = FALSE;

			double dd_seconds_elapsed;
			double dd_seconds_total;
			double dd_seconds_remaining;

			if(nBatchPos == 0)
			{
				sprintf(szLabel, "N/A");
			}
			else if(nBatchPos == nBatchSize)
			{
				sprintf(szLabel, "00:00:00");
			}
			else
			{
				dd_seconds_elapsed = (double) timeDiff.GetSeconds();
				dd_seconds_elapsed += (double) (timeDiff.GetMinutes() * 60);
				dd_seconds_elapsed += (double) (timeDiff.GetHours() * 3600);

				dd_seconds_total = (dd_seconds_elapsed / dd);
				dd_seconds_remaining = dd_seconds_total - dd_seconds_elapsed;

				if(dd_seconds_remaining > (double) 3600000)
				{
					strcpy(szLabel, "> 1,000 Hours!");
				}
				int nHH, nMM, nSS;
				nSS = (int) dd_seconds_remaining;
				nHH = nSS / 3600;
				nSS = nSS - (nHH * 3600);
				nMM = nSS / 60;
				nSS = nSS - (nMM * 60);
				
				sprintf(szLabel, "%02d:%02d:%02d", nHH, nMM, nSS);
			}

			SetDlgItemText(IDC_STATIC_TIME_TOGO, szLabel);
			m_nTimeToGoLastUpdated = curTime;
		}
		*/

		// Update Transmission Graph
		UpdateTransmissionGraph();
}

void CEncodingProgressDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	delete this;
}

void CEncodingProgressDlg::UpdateTransmissionGraph()
{
/*
	TransmissionStats nTransmissionHistory[300];
	int nTransmissionHistoryTotal[300];

	if(m_pView->m_ThreadMarshal.GetTransmissionHistoryThreaded(nTransmissionHistory) == FALSE)
		return;

	int n,t;
	int nMaxData = 0;

	for(n = 0; n < 300; n++)
	{
		nTransmissionHistoryTotal[n]=0;
		for (t=0;t<NPOSTTHREAD;t++)
		  nTransmissionHistoryTotal[n]+=nTransmissionHistory[n].thread[t];
		if(nTransmissionHistoryTotal[n] > nMaxData)
			nMaxData = nTransmissionHistoryTotal[n];
	}

	// Update Bitmap Image
	CBitmap* pBitmap = m_BitmapLabel.GetCBitmap();
	if(pBitmap == NULL) 
		return;

	BITMAP bmpInfo;
	pBitmap->GetBitmap(&bmpInfo);
	int w = bmpInfo.bmWidth;
	int h = bmpInfo.bmHeight;
	int graph_w = w - 24;
	int k, j;

	TCHAR szPeekSpeed[1024];
	TCHAR* curp = szPeekSpeed;
	TCHAR* lastp = szPeekSpeed;
	curp += sprintf(curp, "PEAK: %d.%d K/sec  -  CURRENT: %d.%02d K/sec TOTAL (", 
		nMaxData >> 10, (nMaxData % 1024) / 103,
		nTransmissionHistoryTotal[1] >> 10, (nTransmissionHistoryTotal[1] % 1024) / 103 );
	lastp = curp;
	for (t=0;t<NPOSTTHREAD;t++)
	{
	  curp += sprintf(curp, " %d.%02d", 
		  nTransmissionHistory[1].thread[t] >> 10, (nTransmissionHistory[1].thread[t] % 1024) / 103 );
	  if (nTransmissionHistory[1].thread[t]>0) lastp=curp;
	}
	sprintf(lastp, " )");
	CDC MemDC;
	CDC* pWndDC = m_BitmapLabel.GetDC();
	MemDC.CreateCompatibleDC(pWndDC);
	m_BitmapLabel.ReleaseDC(pWndDC);

	// Select Font And Bitmap
	CFont* pOldFont = MemDC.SelectObject(&m_StatusFont);
	CBitmap* pOldBMP = MemDC.SelectObject(pBitmap);

	// Set Text Color, Background Color and Background Mode
	MemDC.SetTextColor(RGB(255,255,255));
	MemDC.SetBkColor(RGB(255,0,0));
	MemDC.SetBkMode(TRANSPARENT);

	// Erase background to black
	MemDC.FillSolidRect(0, 0, w, h, RGB(0,0,0));

	// Display Peak K/Sec
	MemDC.TextOut(0, 0, szPeekSpeed, strlen(szPeekSpeed));

	// Display Graph outline
	MemDC.SelectObject(GetStockObject(WHITE_PEN));

	CPoint ptGraphUR(w - 4, 16);
	CPoint ptGraphLR(w - 4, h - 4);
	CPoint ptGraphLL(4, h - 4);

	MemDC.MoveTo(ptGraphLL);
	MemDC.LineTo(ptGraphLR);
	MemDC.LineTo(ptGraphUR);

	CPen yellowPen;
	yellowPen.CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
	CPen darkPen;
	darkPen.CreatePen(PS_SOLID, 1, RGB(0,128,0));
	CPen dottedWhitePen;
	dottedWhitePen.CreatePen(PS_DOT, 1, RGB(255,255,255));

	// Draw Grid Lines
	CPen* pOldPen = MemDC.SelectObject(&darkPen);

	DWORD dwGraphHeight = (DWORD) h - 20;
	k = ((int) dwGraphHeight) / 4;
	for(j = 0; j < 4; j++)
	{
		MemDC.MoveTo(ptGraphLL.x, ptGraphUR.y + (j * k));
		MemDC.LineTo(ptGraphUR.x, ptGraphUR.y + (j * k));
	}

	// Display Stall Timeout Position on Graph
	MemDC.SelectObject(&dottedWhitePen);
	n = m_pView->m_Settings.m_nMinuteStallTimeout * 60;
	MemDC.MoveTo((ptGraphUR.x - 1) - ((n - 1) * 2), ptGraphLR.y);
	MemDC.LineTo((ptGraphUR.x - 1) - ((n - 1) * 2), ptGraphUR.y);

	// Display Statistics Graph
	int x;
	DWORD rY;
	DWORD uDenom;

	uDenom = ((nMaxData + 1) / dwGraphHeight);
	if(uDenom == 0) uDenom = 1;

	CPen threadPen;
	threadPen.CreatePen(PS_SOLID, 1, RGB(128, 128, 0));

	MemDC.SelectObject(&threadPen);
	for (t=0;t<NPOSTTHREAD;t++)
		for(n = 1; n < 300; n++)
		{
			x = (ptGraphUR.x - 1) - ((n - 1) * 2);
			if(x <= 0) 
				break;

			rY = nTransmissionHistory[n].thread[t] / uDenom;

			if(n == 1)
				MemDC.MoveTo(x, ptGraphLR.y - rY);
			else
				MemDC.LineTo(x, ptGraphLR.y - rY);
		}


	MemDC.SelectObject(&yellowPen);

	for(n = 1; n < 300; n++)
	{
		x = (ptGraphUR.x - 1) - ((n - 1) * 2);
		if(x <= 0) 
			break;

		rY = nTransmissionHistoryTotal[n] / uDenom;

		if(n == 1)
			MemDC.MoveTo(x, ptGraphLR.y - rY);
		else
			MemDC.LineTo(x, ptGraphLR.y - rY);
	}


	// Deselect Pen, Font and Bitmap
	if(pOldPen != NULL) MemDC.SelectObject(pOldPen);
	MemDC.SelectObject(pOldBMP);
	MemDC.SelectObject(pOldFont);

	// Tell window to redraw itself
	m_BitmapLabel.Invalidate(FALSE);
*/
}
