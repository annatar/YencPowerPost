// DeleteTaskDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "DeleteTaskDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteTaskDlg dialog


CDeleteTaskDlg::CDeleteTaskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDeleteTaskDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeleteTaskDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bCheckValue = TRUE;
	m_szMessage[0] = '\0';
}


void CDeleteTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteTaskDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteTaskDlg, CDialog)
	//{{AFX_MSG_MAP(CDeleteTaskDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteTaskDlg message handlers

BOOL CDeleteTaskDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetDlgItemText(IDC_STATIC_LABEL, m_szMessage);

	if(m_bCheckValue)
		((CButton*) GetDlgItem(IDC_CHECK_SAVE_TO_HISTORY))->SetCheck(1);
	else
		((CButton*) GetDlgItem(IDC_CHECK_SAVE_TO_HISTORY))->SetCheck(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDeleteTaskDlg::OnOK() 
{
	int nCheckValue = ((CButton*) GetDlgItem(IDC_CHECK_SAVE_TO_HISTORY))->GetCheck();

	if(nCheckValue == 1)
		m_bCheckValue = TRUE;
	else
		m_bCheckValue = FALSE;

	CDialog::OnOK();
}
