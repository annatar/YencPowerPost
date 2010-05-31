// PropPage_Add_FileOrder.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "PropPage_Add_FileOrder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_FileOrder property page

IMPLEMENT_DYNCREATE(CPropPage_Add_FileOrder, CPropertyPage)

CPropPage_Add_FileOrder::CPropPage_Add_FileOrder() : CPropertyPage(CPropPage_Add_FileOrder::IDD)
{
	//{{AFX_DATA_INIT(CPropPage_Add_FileOrder)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPropPage_Add_FileOrder::~CPropPage_Add_FileOrder()
{
}

void CPropPage_Add_FileOrder::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPage_Add_FileOrder)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPage_Add_FileOrder, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPage_Add_FileOrder)
	ON_BN_CLICKED(IDC_BUTTON_AFO_UP, OnButtonAfoUp)
	ON_BN_CLICKED(IDC_BUTTON_AFO_DOWN, OnButtonAfoDown)
	ON_NOTIFY(NM_CLICK, IDC_LIST_FILE_ORDER, OnClickListFileOrder)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILE_ORDER, OnDblclkListFileOrder)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_FILE_ORDER, OnKeydownListFileOrder)
	ON_BN_CLICKED(IDC_BUTTON_AFO_SORTPAR, OnButtonAfoSortpar)
	ON_BN_CLICKED(IDC_BUTTON_AFO_DELFILE, OnButtonAfoDelfile)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_FileOrder message handlers

BOOL CPropPage_Add_FileOrder::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	*m_pdwActivation |= PAGE__FILE_ORDER;
	
	CListCtrl* pListCtrl  = (CListCtrl*) GetDlgItem(IDC_LIST_FILE_ORDER);

	CString strResourceBundleString;
	strResourceBundleString.LoadString(IDS_FILENAME);
	pListCtrl->InsertColumn(0, LPCTSTR(strResourceBundleString), LVCFMT_LEFT, 320, -1);
	strResourceBundleString.LoadString(IDS_DIRECTORY);
	pListCtrl->InsertColumn(1, LPCTSTR(strResourceBundleString), LVCFMT_LEFT, 0, -1);

	// Calculate Common Directory Used In Each File
	TCHAR szTemp[_MAX_PATH];

	int n;
	int nOffset = strlen(m_szDirectory) + 1;

	for(n = 0; n < m_pDWA_FileNames->GetSize(); n++)
	{
		_BreakFilename((LPCTSTR) m_pDWA_FileNames->GetAt(n), m_szDirectory, szTemp);
		pListCtrl->InsertItem(n,szTemp,0);
		pListCtrl->SetItemText(n,1,m_szDirectory);
	}
	
	UpdateButtons();
	strcpy(m_szLastDirectory, m_szDirectory);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropPage_Add_FileOrder::_BreakFilename(LPCTSTR FullFilename, char* szDir, char* szFile)
{
	int sl;
	strcpy(szDir, FullFilename);
	sl = strlen(szDir) - 1;
	while((sl > 0) && (szDir[sl] != '\\')) --sl;
	szDir[sl] = '\0';
	strcpy(szFile, &szDir[sl+1]);
}

BOOL CPropPage_Add_FileOrder::OnApply() 
{
	// Re-Populate this->m_pDWA_FileNames
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_FILE_ORDER);
	int n = 0;
	int nCount = pListCtrl->GetItemCount();

	TCHAR szText[_MAX_PATH];
	TCHAR szFilename[_MAX_PATH];

	m_pDWA_FileNames->RemoveAll();

	for(n = 0; n < nCount; n++)
	{
		pListCtrl->GetItemText(n, 0, szText, _MAX_PATH);
		pListCtrl->GetItemText(n, 1, m_szDirectory, _MAX_PATH);
		sprintf(szFilename, "%s\\%s", m_szDirectory, szText);
		m_pDWA_FileNames->Add(szFilename);
	}

	return CPropertyPage::OnApply();
}

BOOL CPropPage_Add_FileOrder::OnSetActive() 
{
#ifdef INCLUDESOUND
	if(m_bSound)
		PlaySound((LPCTSTR) IDR_WAVE_CLAP, NULL, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);	
#endif
		
	return CPropertyPage::OnSetActive();
}

void CPropPage_Add_FileOrder::UpdateButtons()
{
	BOOL bEnabled = TRUE;

	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_FILE_ORDER);

	if(pListCtrl->GetSelectedCount() == 0)
		bEnabled = FALSE;
	
	GetDlgItem(IDC_BUTTON_AFO_UP)->EnableWindow(bEnabled);
	GetDlgItem(IDC_BUTTON_AFO_DOWN)->EnableWindow(bEnabled);
	//GetDlgItem(IDC_BUTTON_AFO_SORTPAR)->EnableWindow(bEnabled);
	GetDlgItem(IDC_BUTTON_AFO_DELFILE)->EnableWindow(bEnabled);

}

void CPropPage_Add_FileOrder::OnButtonAfoUp() 
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_FILE_ORDER);

	int n;
	int nItems = pListCtrl->GetItemCount();

	BOOL bPrevItemSelected = (pListCtrl->GetItemState(0, LVIS_SELECTED) == LVIS_SELECTED) ? TRUE : FALSE;

	TCHAR szItemText1[_MAX_PATH];
	TCHAR szItemText2[_MAX_PATH];
	TCHAR szItemText3[_MAX_PATH];
	TCHAR szItemText4[_MAX_PATH];

	for(n = 1; n < nItems; n++)
	{
		if(pListCtrl->GetItemState(n, LVIS_SELECTED) == LVIS_SELECTED)
		{
			if(!bPrevItemSelected)
			{
				pListCtrl->GetItemText(n-1, 0, szItemText1, _MAX_PATH);
				pListCtrl->GetItemText(n,   0, szItemText2, _MAX_PATH);
				pListCtrl->GetItemText(n-1, 1, szItemText3, _MAX_PATH);
				pListCtrl->GetItemText(n,   1, szItemText4, _MAX_PATH);
				pListCtrl->SetItemText(n-1, 0, szItemText2);
				pListCtrl->SetItemText(n-1, 1, szItemText4);
				pListCtrl->SetItemText(n, 0, szItemText1);
				pListCtrl->SetItemText(n, 1, szItemText3);
				pListCtrl->SetItemState(n-1, LVIS_SELECTED, LVIS_SELECTED);
				pListCtrl->SetItemState(n, 0, LVIS_SELECTED);

			}
		}

		bPrevItemSelected = (pListCtrl->GetItemState(n, LVIS_SELECTED) == LVIS_SELECTED) ? TRUE : FALSE;
	}
}

void CPropPage_Add_FileOrder::OnButtonAfoDown() 
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_FILE_ORDER);

	int n;
	int nItems = pListCtrl->GetItemCount();

	BOOL bPrevItemSelected = (pListCtrl->GetItemState(nItems-1, LVIS_SELECTED) == LVIS_SELECTED) ? TRUE : FALSE;

	TCHAR szItemText1[_MAX_PATH];
	TCHAR szItemText2[_MAX_PATH];
	TCHAR szItemText3[_MAX_PATH];
	TCHAR szItemText4[_MAX_PATH];

	for(n = nItems-2; n >= 0; n--)
	{
		if(pListCtrl->GetItemState(n, LVIS_SELECTED) == LVIS_SELECTED)
		{
			if(!bPrevItemSelected)
			{
				pListCtrl->GetItemText(n+1, 0, szItemText1, _MAX_PATH);
				pListCtrl->GetItemText(n,   0, szItemText2, _MAX_PATH);
				pListCtrl->GetItemText(n+1, 1, szItemText3, _MAX_PATH);
				pListCtrl->GetItemText(n,   1, szItemText4, _MAX_PATH);
				pListCtrl->SetItemText(n+1, 0, szItemText2);
				pListCtrl->SetItemText(n+1, 1, szItemText4);
				pListCtrl->SetItemText(n, 0, szItemText1);
				pListCtrl->SetItemText(n, 1, szItemText3);
				pListCtrl->SetItemState(n+1, LVIS_SELECTED, LVIS_SELECTED);
				pListCtrl->SetItemState(n, 0, LVIS_SELECTED);
			}
		}

		bPrevItemSelected = (pListCtrl->GetItemState(n, LVIS_SELECTED) == LVIS_SELECTED) ? TRUE : FALSE;
	}	
}

void CPropPage_Add_FileOrder::OnClickListFileOrder(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateButtons();
	*pResult = 0;
}

void CPropPage_Add_FileOrder::OnDblclkListFileOrder(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateButtons();
	*pResult = 0;
}

void CPropPage_Add_FileOrder::OnKeydownListFileOrder(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	UpdateButtons();	
	*pResult = 0;
}

static BOOL ispar2volume(const TCHAR* fname)
{
	const TCHAR *e=fname;
	const TCHAR *e2=fname;
	while (*fname)
	{
		if (*fname=='.') { e2=e; e=fname; }
		++fname;
	}
	return (!strnicmp(e2,".vol",4)) && (!stricmp(e,".par2"));
}

void CPropPage_Add_FileOrder::OnButtonAfoSortpar() 
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_FILE_ORDER);

	int n;
	int npar = 0;
	int nItems = pListCtrl->GetItemCount();
	int nLoop = nItems;
	int nnfo = 0;//Alain
	int nsfv = 0;//Alain

	TCHAR szItemText1[_MAX_PATH];
	TCHAR szItemText2[_MAX_PATH];

	for(n = 0; n < nLoop; n++)
	{
		pListCtrl->GetItemText(n, 0, szItemText1, _MAX_PATH);
		pListCtrl->GetItemText(n, 1, szItemText2, _MAX_PATH);
		const TCHAR* e = fileext(szItemText1);
		//Rajout du tri avec NFO et SFV
		if (!stricmp(e,".nfo"))
		{
			pListCtrl->DeleteItem(n);
			pListCtrl->InsertItem(nnfo,szItemText1);
			pListCtrl->SetItemText(nnfo,1,szItemText2);
			++nnfo;
		}
		else if (!stricmp(e,".sfv"))
		{
			pListCtrl->DeleteItem(n);
			pListCtrl->InsertItem(nsfv+nnfo,szItemText1);
			pListCtrl->SetItemText(nsfv+nnfo,1,szItemText2);
			++nnfo;
		}
		//Fin Rajout
		else if ((e[0]=='.' && (e[1]=='p' || e[1]=='P') && isdigit(e[2]) && isdigit(e[3]) && e[4]=='\0') || ispar2volume(szItemText1))
		{
			pListCtrl->DeleteItem(n);
			pListCtrl->InsertItem(nItems,szItemText1);
			pListCtrl->SetItemText(nItems-1,1,szItemText2);
			--n;
			--nLoop;
		}
		else if (!stricmp(e,".par") || !stricmp(e,".par2"))
		{
			pListCtrl->DeleteItem(n);
			pListCtrl->InsertItem(npar+nsfv+nnfo,szItemText1);//Modif pour triage
			pListCtrl->SetItemText(npar+nsfv+nnfo,1,szItemText2);
			++npar;
		}
	}
}


void CPropPage_Add_FileOrder::OnButtonAfoDelfile()
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_FILE_ORDER);

	int n;
	int nItems = pListCtrl->GetItemCount();

	for(n = 0; n < nItems; n++)
	{
		if(pListCtrl->GetItemState(n, LVIS_SELECTED) == LVIS_SELECTED)
		{
				pListCtrl->DeleteItem(n);
				n--;
		}
	}
}

void CPropPage_Add_FileOrder::OnButtonAdd()
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_FILE_ORDER);
	int nItems = pListCtrl->GetItemCount();

	CFileDialog F_Dlg(TRUE);

	F_Dlg.m_ofn.Flags |= OFN_ALLOWMULTISELECT;

	// Increase Buffer For Multi-Select Storage
	TCHAR* pBuffer = new TCHAR[24576];	// 24K
	
	*pBuffer = '\0';

	F_Dlg.m_ofn.lpstrFile = pBuffer;
	F_Dlg.m_ofn.nMaxFile = 24576;

	TCHAR szCurrentDirectory[_MAX_PATH];
	::GetCurrentDirectory(_MAX_PATH, szCurrentDirectory);

	TCHAR szInitialDirectory[_MAX_PATH];
	strcpy(szInitialDirectory, m_szDirectory);

	if(strlen(szInitialDirectory) == 0)
		strcpy(szInitialDirectory, szCurrentDirectory);

	F_Dlg.m_ofn.lpstrInitialDir = &szInitialDirectory[0];

	if(!::CM_IsDirectory(F_Dlg.m_ofn.lpstrInitialDir))
		strcpy(szInitialDirectory, szCurrentDirectory);

	if(F_Dlg.DoModal() == IDOK)
	{
		// Get Filenames
		POSITION pos;

		pos = F_Dlg.GetStartPosition();
		m_pDWA_FileNames->RemoveAll();
		while(pos != (POSITION) NULL)
		{
			m_pDWA_FileNames->Add(F_Dlg.GetNextPathName(pos));
		}

		// Save Current Directory for next time.
		::GetCurrentDirectory(_MAX_PATH,m_szLastDirectory );
		
	}
	else
	{
		m_pDWA_FileNames->RemoveAll();

	}

	// Release Buffer
	delete [] pBuffer;

	// Reset Current Directory
	::SetCurrentDirectory(szCurrentDirectory);


	TCHAR szTemp[_MAX_PATH];

	int n;

	for(n = 0; n < m_pDWA_FileNames->GetSize(); n++)
	{
	    _BreakFilename((LPCTSTR) m_pDWA_FileNames->GetAt(n), m_szDirectory, szTemp);
		pListCtrl->InsertItem((nItems + n),szTemp,0);
		pListCtrl->SetItemText(nItems + n,1,m_szDirectory);
	}
}
