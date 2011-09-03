// PropPage_Add_Groups.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "PropPage_Add_Groups.h"
#include "AddGroupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_Groups property page

IMPLEMENT_DYNCREATE(CPropPage_Add_Groups, CPropertyPage)

CPropPage_Add_Groups::CPropPage_Add_Groups() : CPropertyPage(CPropPage_Add_Groups::IDD)
{
	//{{AFX_DATA_INIT(CPropPage_Add_Groups)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pSettings = NULL;
	m_pdwActivation = (DWORD*) NULL;
}

CPropPage_Add_Groups::~CPropPage_Add_Groups()
{
}

void CPropPage_Add_Groups::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPage_Add_Groups)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPage_Add_Groups, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPage_Add_Groups)
	ON_BN_CLICKED(IDC_BUTTON_ADD_GROUP, OnButtonAddGroup)
	ON_NOTIFY(NM_CLICK, IDC_LIST_GROUPS, OnClickListGroups)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GROUPS, OnDblclkListGroups)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_GROUPS, OnKeydownListGroups)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_Groups message handlers

BOOL CPropPage_Add_Groups::OnApply() 
{
	if ((*m_pdwActivation)&PAGE__GROUPS)
	{
		CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_GROUPS);
		m_strGroupList.Empty();

		TCHAR szSelected[2048];
		TCHAR szGroupName[256];

		szSelected[0] = '\0';

		// Save Selected Groups To m_strGroupList (comma seperated)
		int k;
		int nSelected = 0;
		for(k = 0; k < pListCtrl->GetItemCount(); k++)
		{
			pListCtrl->GetItemText(k, 0, szGroupName, 256);
			
			if (pListCtrl->GetItemState(k, LVIS_STATEIMAGEMASK) == INDEXTOSTATEIMAGEMASK(2))
			{
				++nSelected;
				if(nSelected > 8)
				{
					CString strResourceBundleString;
					strResourceBundleString.LoadString(IDS_NEWSGROUPS_TOO_MANY_NEWSGROUPS_SELECTED);
					AfxMessageBox(LPCTSTR(strResourceBundleString), MB_ICONSTOP);
					return(FALSE);
				}

				if(strlen(szSelected) > 0) strcat(szSelected, ",");
				strcat(szSelected, szGroupName);
			}
		}

		m_strGroupList = (LPCTSTR) szSelected;
	}
	if(m_strGroupList.GetLength() == 0)
	{
		CString strResourceBundleString;
		strResourceBundleString.LoadString(IDS_NEWSGROUPS_NO_NEWSGROUPS_SELECTED);
		AfxMessageBox(LPCTSTR(strResourceBundleString), MB_ICONSTOP);
		return(FALSE);
	}

	return CPropertyPage::OnApply();
}

BOOL CPropPage_Add_Groups::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	*m_pdwActivation |= PAGE__GROUPS;
	
	int k = m_pSettings->m_GroupList.GetSize();

	m_StateImageList.Create(IDB_STATEICONS,16,1, RGB(255,0,0));

	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_GROUPS);

	pListCtrl->SetImageList(&m_StateImageList, LVSIL_STATE);
	CString strResourceBundleString;
	strResourceBundleString.LoadString(IDS_NEWSGROUP);
	pListCtrl->InsertColumn(0, LPCTSTR(strResourceBundleString), LVCFMT_LEFT, 320, -1);

	CStringArray CSA_GroupList;
	CString strGroupName;
	TCHAR szText[_MAX_PATH];
	int nPosList = 0;
	int nPosText = 0;
	LPCTSTR szGroups = (LPCTSTR)m_strGroupList;

	while(szGroups[nPosList] != '\0')
	{
		nPosText = 0;

		while((szGroups[nPosList] != ',') && (szGroups[nPosList] != '\0'))
		{
			szText[nPosText] = szGroups[nPosList];
			++nPosList;
			++nPosText;
		}

		szText[nPosText] = '\0';

		if(strlen(szText) > 0)
		{
			CSA_GroupList.Add(szText);

			TRACE("CSA_TaskGroupList : %s\n", szText);

			// Verify Group is in master list
			if(::CM_CSA_SEEK(m_pSettings->m_GroupList, szText) < 0)
			{
				TRACE("  (Not in master list ; adding)\n");
				::CM_CSA_INSERT(m_pSettings->m_GroupList, szText);
			}
		}

		if(szGroups[nPosList] == ',')
			++nPosList;
	}
	
	AddGroups(pListCtrl,CSA_GroupList);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropPage_Add_Groups::AddGroups(CListCtrl* pListCtrl,CStringArray& selgroups)
{
	int n;
	TCHAR szGroupName[256];
	LV_ITEM lvi;

	for(n = 0; n < m_pSettings->m_GroupList.GetSize(); n++)
	{
		::ZeroMemory(&lvi, sizeof(LV_ITEM));
		::ZeroMemory(szGroupName, 256);

		strcpy(szGroupName, (LPCTSTR) m_pSettings->m_GroupList.GetAt(n));

		lvi.mask=LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE | LVIF_STATE;
		lvi.iItem = 0;
		lvi.pszText = (LPSTR) szGroupName;
		lvi.iImage=0;
		lvi.lParam = 0;
		lvi.stateMask=LVIS_STATEIMAGEMASK;

		 if(CM_CSA_SEEK(selgroups, (LPCTSTR) m_pSettings->m_GroupList.GetAt(n)) >= 0)
		 	lvi.state=INDEXTOSTATEIMAGEMASK(2);
		 else
			lvi.state=INDEXTOSTATEIMAGEMASK(1);

		pListCtrl->InsertItem(&lvi);
	}
}

void CPropPage_Add_Groups::OnButtonAddGroup() 
{
	CAddGroupDlg dlg;
	TCHAR szTT[256];
	TCHAR szGroupName[128];
	
	LV_ITEM lvi;

	if(dlg.DoModal() == IDOK)
	{
		dlg.m_GroupName.MakeLower();
		strcpy(szGroupName, (LPCTSTR) dlg.m_GroupName);

		// Filter Newsgroups
		if(!CM_NewsgroupSelectionAllowed(szGroupName)) 
			return;

		if(::CM_CSA_SEEK(m_pSettings->m_GroupList, (LPCTSTR) dlg.m_GroupName) >= 0)
		{
			CString strResourceBundleString;
			strResourceBundleString.LoadString(IDS_NEWSGROUP_ADD_EXISTING_GROUP);
			sprintf(szTT, LPCTSTR(strResourceBundleString), szGroupName);
			AfxMessageBox(szTT, MB_ICONINFORMATION);
			return;
		}

		// Ok, Add it...
		CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_GROUPS);

		::ZeroMemory(&lvi, sizeof(LV_ITEM));

		lvi.mask=LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE | LVIF_STATE;
		lvi.iItem = 0;
		lvi.pszText = (LPSTR) szGroupName;
		lvi.iImage=0;
		lvi.lParam = 0;
		lvi.stateMask=LVIS_STATEIMAGEMASK;
		lvi.state=INDEXTOSTATEIMAGEMASK(1);		// Unchecked initially
		
		pListCtrl->InsertItem(&lvi);

		::CM_CSA_INSERT(m_pSettings->m_GroupList, (LPCTSTR) szGroupName);
	}	
}

void CPropPage_Add_Groups::OnClickListGroups(NMHDR* pNMHDR, LRESULT* pResult) 
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
		
	}

 	*pResult = 0;
}

void CPropPage_Add_Groups::OnDblclkListGroups(NMHDR* pNMHDR, LRESULT* pResult) 
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

	}

 	*pResult = 0;
}

void CPropPage_Add_Groups::OnKeydownListGroups(NMHDR* pNMHDR, LRESULT* pResult) 
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

		}
	}
	*pResult = 0;
}

BOOL CPropPage_Add_Groups::OnSetActive() 
{
#ifdef INCLUDESOUND
	if(m_bSound)
		PlaySound((LPCTSTR) IDR_WAVE_CLAP, NULL, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);	
#endif
	
	return CPropertyPage::OnSetActive();
}
