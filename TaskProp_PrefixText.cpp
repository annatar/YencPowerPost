// TaskProp_PrefixText.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "TaskProp_PrefixText.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskProp_PrefixText property page

IMPLEMENT_DYNCREATE(CTaskProp_PrefixText, CPropertyPage)

CTaskProp_PrefixText::CTaskProp_PrefixText() : CPropertyPage(CTaskProp_PrefixText::IDD)
{
	m_pTaskObject = (CTaskObject*) NULL;
	m_bSound = TRUE;
}

CTaskProp_PrefixText::~CTaskProp_PrefixText()
{
}

void CTaskProp_PrefixText::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskProp_PrefixText)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTaskProp_PrefixText, CPropertyPage)
	//{{AFX_MSG_MAP(CTaskProp_PrefixText)
	ON_EN_CHANGE(IDC_EDIT_TASK_ITEM_PREFIX_TEXT, OnChangeEditTaskItemPrefixText)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskProp_PrefixText message handlers

BOOL CTaskProp_PrefixText::OnApply() 
{
	GetDlgItem(IDC_EDIT_TASK_ITEM_PREFIX_TEXT)->GetWindowText(m_pTaskObject->m_strTextPrefix);

	int nCheckValue = ((CButton*) GetDlgItem(IDC_CHECK_TASK_POST_PREFIX_TEXT))->GetCheck();
	if(nCheckValue == 0)
		m_pTaskObject->m_bPostPrefixText = FALSE;
	else
		m_pTaskObject->m_bPostPrefixText = TRUE;
	
	return CPropertyPage::OnApply();
}

BOOL CTaskProp_PrefixText::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_EDIT_TASK_ITEM_PREFIX_TEXT);
	pEdit->SetWindowText((LPCTSTR) m_pTaskObject->m_strTextPrefix);
	pEdit->SetSel(-1, -1);

	if(m_pTaskObject->m_bPostPrefixText)
		((CButton*) GetDlgItem(IDC_CHECK_TASK_POST_PREFIX_TEXT))->SetCheck(1);
	else
		((CButton*) GetDlgItem(IDC_CHECK_TASK_POST_PREFIX_TEXT))->SetCheck(0);

	if(m_pTaskObject->m_strTextPrefix.GetLength() == 0)
		GetDlgItem(IDC_CHECK_TASK_POST_PREFIX_TEXT)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTaskProp_PrefixText::OnChangeEditTaskItemPrefixText() 
{
	CWnd* pWnd = GetDlgItem(IDC_CHECK_TASK_POST_PREFIX_TEXT);
	if(pWnd->GetWindowTextLength() == 0)
		pWnd->EnableWindow(FALSE);
	else
		pWnd->EnableWindow(TRUE);

}

BOOL CTaskProp_PrefixText::OnSetActive() 
{
#ifdef INCLUDESOUND
	if(m_bSound) PlaySound((LPCTSTR) IDR_WAVE_CLAP, NULL, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
#endif
	return CPropertyPage::OnSetActive();
}
