// TaskProp_Log.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "TaskProp_Log.h"
#include "DeleteHistoryConfirmDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskProp_Log property page

IMPLEMENT_DYNCREATE(CTaskProp_Log, CPropertyPage)

CTaskProp_Log::CTaskProp_Log() : CPropertyPage(CTaskProp_Log::IDD)
{
	m_pTaskObject = (CTaskObject*) NULL;
	m_bSound = TRUE;
}

CTaskProp_Log::~CTaskProp_Log()
{
}

void CTaskProp_Log::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskProp_Log)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTaskProp_Log, CPropertyPage)
	//{{AFX_MSG_MAP(CTaskProp_Log)
	ON_BN_CLICKED(IDC_BUTTON_TASK_ITEM_CLEAR_LOG, OnButtonTaskItemClearLog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskProp_Log message handlers

BOOL CTaskProp_Log::OnApply() 
{	
	return CPropertyPage::OnApply();
}

BOOL CTaskProp_Log::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_TASK_LOG);

	pListCtrl->InsertColumn(0, "Log Item Text", LVCFMT_LEFT, 512);

	int nLogItems = m_pTaskObject->m_Log.GetSize();

	if(nLogItems > 0)
	{
		for(int k = 0; k < nLogItems; k++)
			pListCtrl->InsertItem(k, (LPCTSTR) m_pTaskObject->m_Log.GetAt(k));
	}
	else GetDlgItem(IDC_BUTTON_TASK_ITEM_CLEAR_LOG)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTaskProp_Log::OnButtonTaskItemClearLog() 
{
	if(m_bSound) PlaySound((LPCTSTR) IDR_WAVE_HISTORY_ERASER, NULL, SND_ASYNC | SND_RESOURCE);

	CDeleteHistoryConfirmDlg Dlg;
	strcpy(Dlg.m_szText, "Delete Task Log History?");

	if(Dlg.DoModal() == IDOK)
	{
		CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_TASK_LOG);
		pListCtrl->DeleteAllItems();
		//m_pTaskObject->m_Log.RemoveAll();
		m_pTaskObject->ResetLog();

		int nLogItems = m_pTaskObject->m_Log.GetSize();

		if(nLogItems > 0)
		{
			for(int k = 0; k < nLogItems; k++)
				pListCtrl->InsertItem(k, (LPCTSTR) m_pTaskObject->m_Log.GetAt(k));
		}
		else GetDlgItem(IDC_BUTTON_TASK_ITEM_CLEAR_LOG)->EnableWindow(FALSE);
	}
}

BOOL CTaskProp_Log::OnSetActive() 
{
#ifdef INCLUDESOUND
	if(m_bSound) PlaySound((LPCTSTR) IDR_WAVE_CLAP, NULL, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
#endif
	return CPropertyPage::OnSetActive();
}
