// TaskProp_FileAndParts.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "TaskProp_FileAndParts.h"
#include "SetMaxLinesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskProp_FileAndParts property page

IMPLEMENT_DYNCREATE(CTaskProp_FileAndParts, CPropertyPage)

CTaskProp_FileAndParts::CTaskProp_FileAndParts() : CPropertyPage(CTaskProp_FileAndParts::IDD)
{
	m_pTaskObject = (CTaskObject*) NULL;
	m_nPartsChecked = 0;
}

CTaskProp_FileAndParts::~CTaskProp_FileAndParts()
{
}

void CTaskProp_FileAndParts::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskProp_FileAndParts)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTaskProp_FileAndParts, CPropertyPage)
	//{{AFX_MSG_MAP(CTaskProp_FileAndParts)
	ON_BN_CLICKED(IDC_BUTTON_TASK_PARTS_CHECK_ALL, OnButtonTaskPartsCheckAll)
	ON_BN_CLICKED(IDC_BUTTON_TASK_PARTS_UNCHECK_ALL, OnButtonTaskPartsUncheckAll)
	ON_BN_CLICKED(IDC_BUTTON_PARTS_MODIFY, OnButtonPartsModify)
	ON_NOTIFY(NM_CLICK, IDC_LIST_TASK_PARTS, OnClickListTaskParts)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskProp_FileAndParts message handlers

BOOL CTaskProp_FileAndParts::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	
	ASSERT(m_pTaskObject != NULL);

	TCHAR szText[128];

	SetDlgItemText(IDC_EDIT_TASK_FILENAME, m_pTaskObject->m_szFilename);

	SetDlgItemText(IDC_EDIT_TASK_SUBJECT, m_pTaskObject->m_szSubject);

	sprintf(szText, "%d", m_pTaskObject->getMaxLines());
	SetDlgItemText(IDC_EDIT_TASK_MAX_LINES, szText);

	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_TASK_PARTS);

	if(m_StateImageList.m_hImageList != NULL)
	{
		// Required because we will be re-invoking the dialog 
		m_StateImageList.DeleteImageList();
	}

	m_StateImageList.Create(IDB_STATEICONS,16,1, RGB(255,0,0));
	pListCtrl->SetImageList(&m_StateImageList, LVSIL_STATE);
	CString strResourceBundleString;
	strResourceBundleString.LoadString(IDS_SELECT_PARTS_TO_POST);
	pListCtrl->InsertColumn(0, LPCTSTR(strResourceBundleString), LVCFMT_LEFT, 192);

	// Limit Subject Text To _MAX_PATH
	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_EDIT_TASK_SUBJECT);
	pEdit->LimitText(_MAX_PATH - 1);

	Refresh_Parts_List(pListCtrl);
	Refresh_Num_Checked_Label();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTaskProp_FileAndParts::Refresh_Num_Checked_Label()
{
	TCHAR szText[64];
	sprintf(szText, "%d Parts", m_nPartsChecked);
	GetDlgItem(IDC_STATIC_N_CHECKED)->SetWindowText(szText);
}

BOOL CTaskProp_FileAndParts::OnApply() 
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_TASK_PARTS);

	// Save Selected Parts
	m_pTaskObject->m_dwaPartsToPost.RemoveAll();
	int nParts = pListCtrl->GetItemCount();

	for(int k = 0; k < nParts; k++)
	{
		if(pListCtrl->GetItemState(k, LVIS_STATEIMAGEMASK) == INDEXTOSTATEIMAGEMASK(2))
		{
			m_pTaskObject->m_dwaPartsToPost.Add(k+1);
		}
	}
	
	GetDlgItemText(IDC_EDIT_TASK_SUBJECT, m_pTaskObject->m_szSubject, _MAX_PATH);

	return CPropertyPage::OnApply();
}

void CTaskProp_FileAndParts::Refresh_Parts_List(CListCtrl* pListCtrl)
{
	int k;
	TCHAR szText[128];
	LV_ITEM lvi;

	CWaitCursor wc;

	if(pListCtrl->GetItemCount() > 0)
		pListCtrl->DeleteAllItems();

	int nPartsRequired = m_pTaskObject->getNbParts();

	sprintf(szText, "%d Parts (File Size: %I64u Bytes)", nPartsRequired, m_pTaskObject->m_ullFileSize);
	SetDlgItemText(IDC_LABEL_FILE_INFO, szText);

	TCHAR szItemText[64];

	m_nPartsChecked = 0;

	for(k = 1; k <= nPartsRequired; k++)
	{
		::ZeroMemory(&lvi, sizeof(LV_ITEM));
		::ZeroMemory(szItemText, 64);

		if(nPartsRequired < 10)
			sprintf(szItemText, "Part (%d/%d)", k, nPartsRequired);
		else if(nPartsRequired < 100)
			sprintf(szItemText, "Part (%02d/%d)", k, nPartsRequired);
		else if(nPartsRequired < 1000)
			sprintf(szItemText, "Part (%03d/%d)", k, nPartsRequired);
		else if(nPartsRequired < 10000)
			sprintf(szItemText, "Part (%04d/%d)", k, nPartsRequired);
		else
			sprintf(szItemText, "Part (%05d/%d)", k, nPartsRequired);

		lvi.mask=LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE | LVIF_STATE;
		lvi.iItem = 0;
		lvi.pszText = (LPSTR) szItemText;
		lvi.iImage=0;
		lvi.lParam = k;
		lvi.stateMask=LVIS_STATEIMAGEMASK;

		if(CM_DWA_SEEK(m_pTaskObject->m_dwaPartsToPost, k) >= 0)
		{
			++m_nPartsChecked;
			lvi.state=INDEXTOSTATEIMAGEMASK(2);
		}
		else lvi.state=INDEXTOSTATEIMAGEMASK(1);

		pListCtrl->InsertItem(&lvi);		
	}
}

void CTaskProp_FileAndParts::OnButtonTaskPartsCheckAll() 
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_TASK_PARTS);

	int nItems = pListCtrl->GetItemCount();

	for(int k = 0; k < nItems; k++)
	{
		if(pListCtrl->GetItemState(k, LVIS_STATEIMAGEMASK) == INDEXTOSTATEIMAGEMASK(1))
			pListCtrl->SetItemState(k, INDEXTOSTATEIMAGEMASK(2), LVIS_STATEIMAGEMASK);
	}

	m_nPartsChecked = nItems;

	Refresh_Num_Checked_Label();
}

void CTaskProp_FileAndParts::OnButtonTaskPartsUncheckAll() 
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_TASK_PARTS);

	int nItems = pListCtrl->GetItemCount();

	for(int k = 0; k < nItems; k++)
	{
		if(pListCtrl->GetItemState(k, LVIS_STATEIMAGEMASK) == INDEXTOSTATEIMAGEMASK(2))
			pListCtrl->SetItemState(k, INDEXTOSTATEIMAGEMASK(1), LVIS_STATEIMAGEMASK);
	}

	m_nPartsChecked = 0;
	Refresh_Num_Checked_Label();
}

void CTaskProp_FileAndParts::OnButtonPartsModify() 
{
	CSetMaxLinesDlg Dlg;

	Dlg.m_nMaxLines = m_pTaskObject->getMaxLines();

	if(Dlg.DoModal() == IDOK)
	{
		if(m_pTaskObject->getMaxLines() != Dlg.m_nMaxLines)
		{
			// Save New Max Lines Setting
			m_pTaskObject->m_nPartSize = Dlg.m_nMaxLines*YENCLEN;

			// Set All Parts To Post
			int nPartsRequired = m_pTaskObject->getNbParts();
			m_pTaskObject->m_dwaPartsToPost.SetSize(0, nPartsRequired);
			for(int k = 1; k <= nPartsRequired; k++)
				m_pTaskObject->m_dwaPartsToPost.Add(k);

			// Set IDC_EDIT_TASK_MAX_LINES
			TCHAR szText[32];
			sprintf(szText, "%d", m_pTaskObject->getMaxLines());
			SetDlgItemText(IDC_EDIT_TASK_MAX_LINES, szText);

			// Refresh Parts List; Select All Parts
			CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_TASK_PARTS);
			Refresh_Parts_List(pListCtrl);
			Refresh_Num_Checked_Label();
		}
	}
}

void CTaskProp_FileAndParts::OnClickListTaskParts(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_TASK_PARTS);

	// This little feature requires version 4.71 of the common controls..
	NMLISTVIEW* pNMLISTVIEW = (NMLISTVIEW*) pNMHDR;

	int idx = pNMLISTVIEW->iItem;

	// Only toggle if on an item and on check list box (<= 16 pixels)
	if((pNMLISTVIEW->ptAction.x <= 16) && (idx >= 0))
	{
		if (pListCtrl->GetItemState(idx,LVIS_STATEIMAGEMASK)!=INDEXTOSTATEIMAGEMASK(1))
		{
			--m_nPartsChecked;
			pListCtrl->SetItemState(idx,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK);
		}
		else
		{
			++m_nPartsChecked;
			pListCtrl->SetItemState(idx,INDEXTOSTATEIMAGEMASK(2),LVIS_STATEIMAGEMASK);
		}
		Refresh_Num_Checked_Label();
	}	
	*pResult = 0;
}

BOOL CTaskProp_FileAndParts::OnSetActive() 
{
#ifdef INCLUDESOUND
	if(m_bSound) PlaySound((LPCTSTR) IDR_WAVE_CLAP, NULL, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
#endif
	return CPropertyPage::OnSetActive();
}
