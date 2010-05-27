// AddGroupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "AddGroupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddGroupDlg dialog


CAddGroupDlg::CAddGroupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddGroupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddGroupDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_GroupName = _T("alt.binaries.");
}


void CAddGroupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddGroupDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddGroupDlg, CDialog)
	//{{AFX_MSG_MAP(CAddGroupDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddGroupDlg message handlers

BOOL CAddGroupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_EDIT_GROUP)->SetWindowText(m_GroupName);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddGroupDlg::OnOK() 
{
	GetDlgItem(IDC_EDIT_GROUP)->GetWindowText(m_GroupName);
	if(m_GroupName.GetLength() == 0)
	{
		AfxMessageBox("Er, ... You didn't enter a group name!", MB_ICONINFORMATION);
		return;
	}
	if(m_GroupName.GetLength() > 132)
	{
		AfxMessageBox("Damn!  That's a big group name.  Sorry, but, 132 Characters Max", MB_ICONINFORMATION);
		return;
	}
	CDialog::OnOK();
}
