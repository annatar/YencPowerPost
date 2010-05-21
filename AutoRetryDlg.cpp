// AutoRetryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "AutoRetryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAutoRetryDlg dialog

#define ID_DLG_RETRY_TIMER 8192

CAutoRetryDlg::CAutoRetryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAutoRetryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoRetryDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_szMessage[0] = '\0';
	m_nSeconds = 10;
	m_nTimerID = 0;

}


void CAutoRetryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoRetryDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoRetryDlg, CDialog)
	//{{AFX_MSG_MAP(CAutoRetryDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDSKIP, OnSkip)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoRetryDlg message handlers

BOOL CAutoRetryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetDlgItemText(IDC_STATIC_INFO, m_szMessage);

	m_nTimerID = SetTimer(ID_DLG_RETRY_TIMER, 1000, NULL);

	SetTimerText();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAutoRetryDlg::OnCancel() 
{
	KillTimer(m_nTimerID);
	CDialog::OnCancel();
}

void CAutoRetryDlg::OnOK() 
{
	KillTimer(m_nTimerID);	
	CDialog::OnOK();
}

void CAutoRetryDlg::SetTimerText()
{
	TCHAR szText[64];

	sprintf(szText, "Auto-Retry in %d Seconds", m_nSeconds);

	SetDlgItemText(IDC_STATIC_RETRY_INFO, szText);
}

void CAutoRetryDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == ID_DLG_RETRY_TIMER)
	{
		--m_nSeconds;
		if(m_nSeconds <= 0)
		{
			OnOK();
			return;
		}
		SetTimerText();
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CAutoRetryDlg::OnSkip() 
{
	// TODO: Add your control notification handler code here
	KillTimer(m_nTimerID);
	//CDialog::OnCancel();
	EndDialog(IDSKIP);
}
