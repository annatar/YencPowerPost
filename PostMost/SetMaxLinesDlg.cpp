// SetMaxLinesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "SetMaxLinesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetMaxLinesDlg dialog


CSetMaxLinesDlg::CSetMaxLinesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetMaxLinesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetMaxLinesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSetMaxLinesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetMaxLinesDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetMaxLinesDlg, CDialog)
	//{{AFX_MSG_MAP(CSetMaxLinesDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetMaxLinesDlg message handlers

BOOL CSetMaxLinesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	TCHAR szNumber[24];
	sprintf(szNumber, "%d", m_nMaxLines);

	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_EDIT_NEW_MAX_LINES);
	pEdit->LimitText(5);
	pEdit->SetWindowText(szNumber);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetMaxLinesDlg::OnOK() 
{
	TCHAR szNumber[24];

	GetDlgItemText(IDC_EDIT_NEW_MAX_LINES, szNumber, 16);

	if(strlen(szNumber) == 0)
	{
		AfxMessageBox("Enter a Number between One and Ten Thousand\r\nor Click Cancel", MB_ICONSTOP);
		return;
	}

	m_nMaxLines = atoi(szNumber);

	if((m_nMaxLines < 1000) || (m_nMaxLines > 32000))
	{
		AfxMessageBox("Number Out Of Range!\r\n\r\nEnter a Number between 1,000 and 32,000\r\nor Click Cancel", MB_ICONSTOP);
		return;
	}

	CDialog::OnOK();
}
