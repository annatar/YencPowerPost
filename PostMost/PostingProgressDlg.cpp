// PostingProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MainFrm.h"
#include "PostMost.h"
#include "PostMostDoc.h"
#include "PostMostView.h"
#include "PostingProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPostingProgressDlg dialog


CPostingProgressDlg::CPostingProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPostingProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPostingProgressDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pView = (CPostMostView*) NULL;
	m_uiTimerID = 0;
	m_rectAnimateFrom.SetRectEmpty();
	m_nTimeToGoLastUpdated = CTime::GetCurrentTime();
	m_bForceUpdateTimeToGo = TRUE; // So it is updated on new dialog invocation
}


void CPostingProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPostingProgressDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPostingProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CPostingProgressDlg)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_BWCAP, OnChangeEditBwcap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPostingProgressDlg message handlers

void CPostingProgressDlg::OnOK() 
{
	OnCancel();
}

void CPostingProgressDlg::OnCancel() 
{
	if(m_pView != (CPostMostView*) NULL)
		m_pView->m_pProgressDlg = (CPostingProgressDlg*) NULL;

	Stop_Update_Timer();

	ShowWindow(SW_HIDE);

	if(!m_rectAnimateFrom.IsRectEmpty())
	{
		// Update Rectangle in case main window moved..
		CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
		pMainFrame->GetButtonRect(ID_POST_THREADS,m_rectAnimateFrom);

	}

	CDialog::OnCancel();
}

CBitmap* CPostingProgressDlg::CreateBitmapForDlgItem(int nID)
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


BOOL CPostingProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CenterWindow();

	if(!m_rectAnimateFrom.IsRectEmpty())
	{
	}

	GetDlgItem(IDC_EDIT_SERVER)->SetWindowText(m_pView->m_Settings.m_szServerName);

	CString theTime = m_pView->m_Time_Started.Format("%a %H:%M:%S");
	GetDlgItem(IDC_STATIC_TIME_STARTED)->SetWindowText((LPCTSTR) theTime);

	// Subclass IDC_STATS with member variable m_BitmapLabel
	CBitmap* pNewCBitmap = CreateBitmapForDlgItem(IDC_STATIC_GRAPH);
	m_BitmapLabel.SubclassDlgItem(IDC_STATIC_GRAPH, this);
	m_BitmapLabel.SetCBitmap(pNewCBitmap);

	CString S;
	S.Format("%d",m_pView->m_ThreadMarshal.m_Settings.m_nBandwidthCap);
	GetDlgItem(IDC_EDIT_BWCAP)->SetWindowText((LPCTSTR)S);


	OnTimer(IDTIMER_UPDATE_PROGRESS);
	Start_Update_Timer();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPostingProgressDlg::Start_Update_Timer()
{
	m_uiTimerID = SetTimer(IDTIMER_UPDATE_PROGRESS, 1000, NULL);
}

void CPostingProgressDlg::Stop_Update_Timer()
{
	KillTimer(m_uiTimerID);
}

void CPostingProgressDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == IDTIMER_UPDATE_PROGRESS)
	{
		int nPosFile=0;
		int nPosArticle=0;
		int nCurrentPart=0;
		int nCurrentPartOf=1;
		UINT nBatchSize=1, nBatchPos=1;

		char szTT[64];
		char szCurrentFile[320];
		char szCurrentArticle[320];

		strcpy(szCurrentFile, "");
		strcpy(szCurrentArticle, "");

		static const int thread_cur_article_id[NPOSTTHREAD] =
		{IDC_EDIT_CUR_ARTICLE
		,IDC_EDIT_CUR_ARTICLE2
		,IDC_EDIT_CUR_ARTICLE3
		,IDC_EDIT_CUR_ARTICLE4
		,IDC_EDIT_CUR_ARTICLE5
		,IDC_EDIT_CUR_ARTICLE6
		,IDC_EDIT_CUR_ARTICLE7
		,IDC_EDIT_CUR_ARTICLE8
		,IDC_EDIT_CUR_ARTICLE9
		,IDC_EDIT_CUR_ARTICLE10
		,IDC_EDIT_CUR_ARTICLE11
		,IDC_EDIT_CUR_ARTICLE12};
		static const int thread_progress_article_id[NPOSTTHREAD] = 
		{IDC_PROGRESS_ARTICLE1
		,IDC_PROGRESS_ARTICLE2
		,IDC_PROGRESS_ARTICLE3
		,IDC_PROGRESS_ARTICLE4
		,IDC_PROGRESS_ARTICLE5
		,IDC_PROGRESS_ARTICLE6
		,IDC_PROGRESS_ARTICLE7
		,IDC_PROGRESS_ARTICLE8
		,IDC_PROGRESS_ARTICLE9
		,IDC_PROGRESS_ARTICLE10
		,IDC_PROGRESS_ARTICLE11
		,IDC_PROGRESS_ARTICLE12};
		static const int thread_groups_id[NPOSTTHREAD] = 
		{IDC_EDIT_GROUPS
		,IDC_EDIT_GROUPS2
		,IDC_EDIT_GROUPS3
		,IDC_EDIT_GROUPS4
		,IDC_EDIT_GROUPS5
		,IDC_EDIT_GROUPS6
		,IDC_EDIT_GROUPS7
		,IDC_EDIT_GROUPS8
		,IDC_EDIT_GROUPS9
		,IDC_EDIT_GROUPS10
		,IDC_EDIT_GROUPS11
		,IDC_EDIT_GROUPS12};
		static const int thread_progress_file_id[NPOSTTHREAD] = 
		{IDC_PROGRESS_FILE1
		,IDC_PROGRESS_FILE2
		,IDC_PROGRESS_FILE3
		,IDC_PROGRESS_FILE4
		,IDC_PROGRESS_FILE5
		,IDC_PROGRESS_FILE6
		,IDC_PROGRESS_FILE7
		,IDC_PROGRESS_FILE8
		,IDC_PROGRESS_FILE9
		,IDC_PROGRESS_FILE10
		,IDC_PROGRESS_FILE11
		,IDC_PROGRESS_FILE12};

		for (int i=0;i<NPOSTTHREAD;i++)
		{
			if(m_pView->m_ThreadMarshal.GetProgress(i,szCurrentFile, nPosFile, szCurrentArticle, nPosArticle, nCurrentPart, nCurrentPartOf))
			{
				SetDlgItemText(thread_cur_article_id[i],szCurrentArticle);
				((CProgressCtrl*) GetDlgItem(thread_progress_article_id[i]))->SetPos(nPosArticle);
				((CProgressCtrl*) GetDlgItem(thread_progress_file_id[i]))->SetPos(nPosFile);
				m_pView->m_ThreadMarshal.GetCurrentTargetNewsgroupsToWindow(0,GetDlgItem(thread_groups_id[i]));
			}
			strcpy(szCurrentFile, "");
			strcpy(szCurrentArticle, "");
		}

		m_pView->m_ThreadMarshal.GetGlobalProgress(nBatchSize, nBatchPos);

		double dd = (double) nBatchPos / (double) nBatchSize;
		if(dd > 1.0) dd = 1.0;

		((CProgressCtrl*) GetDlgItem(IDC_PROGRESS_BATCH))->SetPos((int) (dd * 100.0));

		// Update Current Part Label
		TCHAR szLabel[64];
		//sprintf(szTT, "%d%% of ", nPosArticle);

		//::CM_BuildArticleName(szLabel, szTT, szCurrentFile, nCurrentPart, nCurrentPartOf);
		//GetDlgItem(IDC_STATIC_PART_INFO)->SetWindowText(szLabel);

		//sprintf(szLabel, "%d%%", nPosFile);
		//GetDlgItem(IDC_STATIC_FILE_INFO)->SetWindowText(szLabel);

		// Update Time Elapsed
		CTime curTime = CTime::GetCurrentTime();
		CTimeSpan timeDiff = curTime - (m_pView->m_Time_Started);
		sprintf(szTT, "%02d:%02d:%02d", timeDiff.GetHours(), timeDiff.GetMinutes(), timeDiff.GetSeconds());
		GetDlgItem(IDC_STATIC_TIME_ELAPSED)->SetWindowText(szTT);

		// Update Time To Go
		CTimeSpan timeDiff2 = curTime - m_nTimeToGoLastUpdated;
		if((timeDiff2.GetTotalSeconds() >= 1) || (m_bForceUpdateTimeToGo == TRUE))
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
	}

	// Update Transmission Graph
	UpdateTransmissionGraph();
}

void CPostingProgressDlg::PostNcDestroy() 
{
	CDialog::PostNcDestroy();
	delete this;
}

void CPostingProgressDlg::UpdateTransmissionGraph()
{

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
}

void CPostingProgressDlg::OnChangeEditBwcap() 
{
	CString S;
	int k;
	GetDlgItem(IDC_EDIT_BWCAP)->GetWindowText(S);
	k = atoi((LPCTSTR) S);
	if(k < 0) k=0;
	m_pView->m_ThreadMarshal.UpdateCap(k);
}
