// DeleteHistoryConfirmDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "DeleteHistoryConfirmDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteHistoryConfirmDlg dialog


CDeleteHistoryConfirmDlg::CDeleteHistoryConfirmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeleteHistoryConfirmDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeleteHistoryConfirmDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_szText[0] = '\0';
}


void CDeleteHistoryConfirmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteHistoryConfirmDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteHistoryConfirmDlg, CDialog)
	//{{AFX_MSG_MAP(CDeleteHistoryConfirmDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteHistoryConfirmDlg message handlers

BOOL CDeleteHistoryConfirmDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetDlgItemText(IDC_HIST_DEL_LABEL, m_szText);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
