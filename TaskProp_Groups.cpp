// TaskProp_Groups.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "TaskProp_Groups.h"
#include "AddGroupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskProp_Groups property page

IMPLEMENT_DYNCREATE(CTaskProp_Groups, CPropertyPage)

CTaskProp_Groups::CTaskProp_Groups() : CPropertyPage(CTaskProp_Groups::IDD)
{
	m_pTaskObject = (CTaskObject*) NULL;
	m_pSettings = (CProgramSettings*) NULL;
	m_bSound = TRUE;
}

CTaskProp_Groups::~CTaskProp_Groups()
{
}

void CTaskProp_Groups::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskProp_Groups)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTaskProp_Groups, CPropertyPage)
	//{{AFX_MSG_MAP(CTaskProp_Groups)
	ON_NOTIFY(NM_CLICK, IDC_LIST_TARGET_GROUPS, OnClickListTargetGroups)
	ON_BN_CLICKED(IDC_BUTTON_TASK_ADD_GROUP, OnButtonTaskAddGroup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskProp_Groups message handlers

BOOL CTaskProp_Groups::OnApply() 
{
	// Verify 8 or less groups selected
	int nSelected = GetSelectedGroupCount();
	if(nSelected == 0)
	{
		AfxMessageBox("No Groups Selected!", MB_ICONSTOP);
		return FALSE;
	}
	if(nSelected > 8)
	{
		AfxMessageBox("Too Many Groups Selected!  Maximum of 8 Groups!", MB_ICONSTOP);
		return FALSE;
	}
	
	// Save Group List to m_pTaskObject->m_strGroups
	GetSelectedGroupString(m_pTaskObject->m_strGroups);

	return CPropertyPage::OnApply();
}

BOOL CTaskProp_Groups::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_TARGET_GROUPS);

	if(m_StateImageList.m_hImageList != NULL)
	{
		// Required because we will be re-invoking the dialog 
		m_StateImageList.DeleteImageList();
	}

	m_StateImageList.Create(IDB_STATEICONS,16,1, RGB(255,0,0));
	pListCtrl->SetImageList(&m_StateImageList, LVSIL_STATE);
	pListCtrl->InsertColumn(0, "Target Newsgroups", LVCFMT_LEFT, 192);

	// Set m_CSA_TaskGroupList from m_pTaskObject->m_strGroups
	m_CSA_TaskGroupList.RemoveAll();

	CString strGroupName;
	TCHAR szText[_MAX_PATH];
	int nPosList = 0;
	int nPosText = 0;
	LPCTSTR szGroups = (LPCTSTR) m_pTaskObject->m_strGroups;

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
			m_CSA_TaskGroupList.Add(szText);

			TRACE("m_CSA_TaskGroupList : %s\n", szText);

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
	
	AddGroups(pListCtrl);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTaskProp_Groups::AddGroups(CListCtrl* pListCtrl)
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

		if(::CM_CSA_SEEK(m_CSA_TaskGroupList, (LPCTSTR) m_pSettings->m_GroupList.GetAt(n)) >= 0)
			lvi.state=INDEXTOSTATEIMAGEMASK(2);
		else
			lvi.state=INDEXTOSTATEIMAGEMASK(1);

		pListCtrl->InsertItem(&lvi);
	}
}

int CTaskProp_Groups::GetSelectedGroupCount()
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_TARGET_GROUPS);

	int nSize = pListCtrl->GetItemCount();
	int nSelected = 0;
	int k;

	for(k = 0; k < nSize; k++)
	{
		if (pListCtrl->GetItemState(k, LVIS_STATEIMAGEMASK) == INDEXTOSTATEIMAGEMASK(2))
			++nSelected;
	}

	return(nSelected);
}

void CTaskProp_Groups::GetSelectedGroupString(CString& rString)
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_TARGET_GROUPS);

	int nSize = pListCtrl->GetItemCount();
	int nSelected = 0;
	int k;
	
	TCHAR szGroupString[2048];
	TCHAR szGroupName[_MAX_PATH];

	rString.Empty();

	szGroupString[0] = '\0';

	for(k = 0; k < nSize; k++)
	{
		if (pListCtrl->GetItemState(k, LVIS_STATEIMAGEMASK) == INDEXTOSTATEIMAGEMASK(2))
		{
			// Get Group Name
			pListCtrl->GetItemText(k, 0, szGroupName, _MAX_PATH);

			// Append to szGroupString
			if(strlen(szGroupString) == 0)
				strcpy(szGroupString, szGroupName);
			else
			{
				strcat(szGroupString, ",");
				strcat(szGroupString, szGroupName);
			}
		}
	}

	rString = (LPCTSTR) szGroupString;
	TRACE("CTaskProp_Groups::GetSelectedGroupString:\n");
	TRACE("    '%s'\n", (LPCTSTR) rString);
	TRACE("\n");
}

void CTaskProp_Groups::OnClickListTargetGroups(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_TARGET_GROUPS);

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

BOOL CTaskProp_Groups::OnSetActive() 
{
#ifdef INCLUDESOUND
	if(m_bSound) PlaySound((LPCTSTR) IDR_WAVE_CLAP, NULL, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
#endif
	return CPropertyPage::OnSetActive();
}

void CTaskProp_Groups::OnButtonTaskAddGroup() 
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
			sprintf(szTT, "Newsgroups: %s\r\n\r\nUhh.. Listen..\r\nThat group is already in the list..", szGroupName);
			AfxMessageBox(szTT, MB_ICONINFORMATION);
			return;
		}

		// Ok, Add it...
		CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_TARGET_GROUPS);

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
