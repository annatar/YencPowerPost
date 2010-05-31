// PropPage_Groups.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "PropPage_Groups.h"

#include "AddGroupDlg.h"
#include "MainFrm.h"
#include "PostMostDoc.h"
#include "PostMostView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Groups property page

IMPLEMENT_DYNCREATE(CPropPage_Groups, CPropertyPage)

CPropPage_Groups::CPropPage_Groups() : CPropertyPage(CPropPage_Groups::IDD)
{
	//{{AFX_DATA_INIT(CPropPage_Groups)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pSettings = (CProgramSettings*) NULL;
	m_pSettings_ApplyTo = (CProgramSettings*) NULL;
}

CPropPage_Groups::~CPropPage_Groups()
{
}

void CPropPage_Groups::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPage_Groups)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPage_Groups, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPage_Groups)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnButtonRemove)
	ON_NOTIFY(NM_CLICK, IDC_LIST_GROUPS, OnClickListGroups)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GROUPS, OnDblclkListGroups)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_GROUPS, OnKeydownListGroups)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Groups message handlers

BOOL CPropPage_Groups::OnApply() 
{
	if(ApplyNewSettings())
	{
		if(m_pSettings_ApplyTo != NULL)
			m_pSettings_ApplyTo->Copy(*m_pSettings);
		else
		{
			CString strResourceBundleString;
			strResourceBundleString.LoadString(IDS_APP_ERROR_NO_OBJECT_TO_APPLY_SETTINGS);
			AfxMessageBox(LPCTSTR(strResourceBundleString), MB_ICONSTOP);
			return FALSE;
		}
		return CPropertyPage::OnApply();
	}
	return FALSE;
}

BOOL CPropPage_Groups::ApplyNewSettings()
{	
	int k;
	TCHAR szGroupName[256];

	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_GROUPS);

	m_pSettings->m_GroupList.RemoveAll();

	// Save All Groups In List
	for(k = 0; k < pListCtrl->GetItemCount(); k++)
	{
		pListCtrl->GetItemText(k, 0, szGroupName, 256);
		::CM_CSA_INSERT(m_pSettings->m_GroupList, szGroupName);
	}

	return TRUE;
}

void CPropPage_Groups::OnButtonAdd() 
{
	CAddGroupDlg dlg;
	TCHAR szTT[256];
	TCHAR szGroupName[128];
	
	LVFINDINFO info;
	LV_ITEM lvi;
	int nIndex;

	if(dlg.DoModal() == IDOK)
	{
		dlg.m_GroupName.MakeLower();
		strcpy(szGroupName, (LPCTSTR) dlg.m_GroupName);

		// Filter Newsgroups
		if(!CM_NewsgroupSelectionAllowed(szGroupName)) return;

		// See if it's in the list already
		CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_GROUPS);

		info.flags = LVFI_STRING;
		info.psz = szGroupName;

		nIndex = pListCtrl->FindItem(&info);
		if(nIndex != -1)
		{
			sprintf(szTT, "Newsgroups: %s\r\n\r\nUhh.. Listen..\r\nThat group is already in the list..", szGroupName);
			AfxMessageBox(szTT, MB_ICONINFORMATION);
			return;
		}

		// Ok, Add it...

		::ZeroMemory(&lvi, sizeof(LV_ITEM));

		lvi.mask=LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE | LVIF_STATE;
		lvi.iItem = 0;
		lvi.pszText = (LPSTR) szGroupName;
		lvi.iImage=0;
		lvi.lParam = 0;
		lvi.stateMask=LVIS_STATEIMAGEMASK;
		lvi.state=INDEXTOSTATEIMAGEMASK(1);		// Unchecked initially
		pListCtrl->InsertItem(&lvi);

		SetModified(TRUE);
	}
}

void CPropPage_Groups::OnButtonRemove() 
{
	int k;
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_GROUPS);

	int nSelected = pListCtrl->GetSelectedCount();

	if(nSelected == 0)
	{
		CString strResourceBundleString;
		strResourceBundleString.LoadString(IDS_NO_NEWSGROUP_SELECTED);
		AfxMessageBox(LPCTSTR(strResourceBundleString), MB_ICONINFORMATION); 
		return;
	}

	TCHAR szTT[64];
	sprintf(szTT, "Delete %d selected newsgroups from list?", nSelected);

	if(AfxMessageBox(szTT, MB_ICONQUESTION | MB_OKCANCEL) == IDOK)
	{
		int nCount = pListCtrl->GetItemCount();
		for(k = nCount; k >= 0; k--)
		{
			if(pListCtrl->GetItemState(k, LVIS_SELECTED) == LVIS_SELECTED)
				pListCtrl->DeleteItem(k);
		}
	}
	SetModified(TRUE);
}

BOOL CPropPage_Groups::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	int k = m_pSettings->m_GroupList.GetSize();

	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_GROUPS);

	CString strResourceBundleString;
	strResourceBundleString.LoadString(IDS_NEWSGROUP);
	pListCtrl->InsertColumn(0, LPCTSTR(strResourceBundleString), LVCFMT_LEFT, 320, -1);

	AddGroups(pListCtrl);

	GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPropPage_Groups::OnSetActive() 
{
	CMainFrame* pMainFrame = (CMainFrame*) (AfxGetApp()->m_pMainWnd);
	CPostMostView* pView = (CPostMostView*) pMainFrame->GetActiveView();

#ifdef INCLUDESOUND
	if(pView->m_Settings.m_bSound)
		PlaySound((LPCTSTR) IDR_WAVE_CLAP, NULL, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
#endif

	return CPropertyPage::OnSetActive();
}

void CPropPage_Groups::AddGroups(CListCtrl* pListCtrl)
{
	CWaitCursor wc;

	pListCtrl->LockWindowUpdate();

	for(int n = 0; n < m_pSettings->m_GroupList.GetSize(); n++)
	{
		pListCtrl->InsertItem(n, (LPCTSTR) m_pSettings->m_GroupList.GetAt(n));
	}
	pListCtrl->UnlockWindowUpdate();
}
/*
void CPropPage_Groups::OnKeydownListGroups(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_GROUPS);

	if (pLVKeyDow->wVKey == (WORD) 32)
	{

		POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
		
		if(pos != NULL)
		{
			int idx = pListCtrl->GetNextSelectedItem(pos);
			if (pListCtrl->GetItemState(idx,LVIS_STATEIMAGEMASK)!=INDEXTOSTATEIMAGEMASK(1))
				pListCtrl->SetItemState(idx,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK);
			else
				pListCtrl->SetItemState(idx,INDEXTOSTATEIMAGEMASK(2),LVIS_STATEIMAGEMASK);

			SetModified();
		}
	}
	UpdateButtons(pListCtrl);
	*pResult = 0;
}

void CPropPage_Groups::OnClickListGroups(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_GROUPS);

	// This little feature requires version 4.71 of the common controls..
	NMLISTVIEW* pNMLISTVIEW = (NMLISTVIEW*) pNMHDR;

	int idx = pNMLISTVIEW->iItem;

	// Only toggle if on an item and on check list box (<= 16 pixels)
	if((pNMLISTVIEW->ptAction.x <= 16) && (idx >= 0))
	{
		if (pListCtrl->GetItemState(idx,LVIS_STATEIMAGEMASK)!=INDEXTOSTATEIMAGEMASK(1))
			pListCtrl->SetItemState(idx,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK);
		else
			pListCtrl->SetItemState(idx,INDEXTOSTATEIMAGEMASK(2),LVIS_STATEIMAGEMASK);
		
		SetModified();
	}

	UpdateButtons(pListCtrl);
 	*pResult = 0;
}

void CPropPage_Groups::OnDblclkListGroups(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_GROUPS);

	// This little feature requires version 4.71 of the common controls..
	NMLISTVIEW* pNMLISTVIEW = (NMLISTVIEW*) pNMHDR;

	int idx = pNMLISTVIEW->iItem;

	if(idx >= 0)
	{
		if (pListCtrl->GetItemState(idx,LVIS_STATEIMAGEMASK)!=INDEXTOSTATEIMAGEMASK(1))
			pListCtrl->SetItemState(idx,INDEXTOSTATEIMAGEMASK(1),LVIS_STATEIMAGEMASK);
		else
			pListCtrl->SetItemState(idx,INDEXTOSTATEIMAGEMASK(2),LVIS_STATEIMAGEMASK);

		SetModified();
	}

	UpdateButtons(pListCtrl);
 	*pResult = 0;
}
*/
void CPropPage_Groups::UpdateButtons(CListCtrl* pListCtrl)
{
	if(pListCtrl->GetSelectedCount() > 0)
		GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(FALSE);
}


void CPropPage_Groups::OnClickListGroups(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_GROUPS);
	UpdateButtons(pListCtrl);
	*pResult = 0;
}

void CPropPage_Groups::OnDblclkListGroups(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_GROUPS);
	UpdateButtons(pListCtrl);
	*pResult = 0;
}

void CPropPage_Groups::OnKeydownListGroups(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_GROUPS);
	UpdateButtons(pListCtrl);	
	*pResult = 0;
}
