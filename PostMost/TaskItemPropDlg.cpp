// TaskItemPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "TaskItemPropDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskItemPropDlg dialog


CTaskItemPropDlg::CTaskItemPropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTaskItemPropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTaskItemPropDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTaskItemPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskItemPropDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTaskItemPropDlg, CDialog)
	//{{AFX_MSG_MAP(CTaskItemPropDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskItemPropDlg message handlers
