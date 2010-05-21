// PropPage_ProgramSettings.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "PropPage_ProgramSettings.h"

#include "MainFrm.h"
#include "PostMostDoc.h"
#include "PostMostView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropPage_ProgramSettings property page

IMPLEMENT_DYNCREATE(CPropPage_ProgramSettings, CPropertyPage)

CPropPage_ProgramSettings::CPropPage_ProgramSettings() : CPropertyPage(CPropPage_ProgramSettings::IDD)
{
	//{{AFX_DATA_INIT(CPropPage_ProgramSettings)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pSettings = (CProgramSettings*) NULL;
	m_pSettings_ApplyTo = (CProgramSettings*) NULL;
}

CPropPage_ProgramSettings::~CPropPage_ProgramSettings()
{
}

void CPropPage_ProgramSettings::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPage_ProgramSettings)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPage_ProgramSettings, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPage_ProgramSettings)
	ON_BN_CLICKED(IDC_CHECK_HL_LAST, OnCheckHLLast)
	ON_BN_CLICKED(IDC_CHECK_SHOWPROGRESSONSTART, OnCheckShowProgressOnStart)
	ON_BN_CLICKED(IDC_CHECK_SOUND, OnCheckSound)
	ON_EN_CHANGE(IDC_EDIT_MAXLINES, OnChangeEditMaxLines)
	ON_BN_CLICKED(IDC_CHECK_SORTFILES, OnCheckSortfiles)
	ON_BN_CLICKED(IDC_CHECK_AUTORETRY, OnCheckAutoRetry)
	ON_BN_CLICKED(IDC_CHECK_SYSTRAY, OnCheckSystray)
	ON_BN_CLICKED(IDC_CHECK_DETECTPAR2, OnCheckDetectpar2)
	ON_CBN_EDITCHANGE(IDC_COMBO_SKIN, OnEditchangeComboSkin)
	ON_CBN_SELCHANGE(IDC_COMBO_SKIN, OnSelchangeComboSkin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

extern CPostMostApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPropPage_ProgramSettings message handlers

BOOL CPropPage_ProgramSettings::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// Search for skins
	((CComboBox*) GetDlgItem(IDC_COMBO_SKIN))->AddString("Default");
	HANDLE search;
	WIN32_FIND_DATA data;
	CHAR buffer[3*MAX_PATH];
	strcpy(buffer,theApp.m_szAppDir);
	strcat(buffer,"\\*.*");
	search = FindFirstFile(buffer, &data);
	if (search != INVALID_HANDLE_VALUE)
	do
	{
	  if (data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY && strcmp(data.cFileName,".") && strcmp(data.cFileName,"..") && stricmp(data.cFileName,"Default"))
	  {
  	    strcpy(buffer,theApp.m_szAppDir);
	    strcat(buffer,"\\");
	    strcat(buffer,data.cFileName);
	    strcat(buffer,"\\skin.txt");
	    FILE* t = fopen(buffer,"r");
	    if (t != NULL)
	    {
	      ((CComboBox*) GetDlgItem(IDC_COMBO_SKIN))->AddString(data.cFileName);
	      fclose(t);
	    }
	  }
	}
	while(FindNextFile(search,&data)!=0);

	((CComboBox*) GetDlgItem(IDC_COMBO_SKIN))->SelectString(-1,m_pSettings->m_szSkin);
	
	// IDC_EDIT_MAXLINES
	CString S;
	S.Format("%d", m_pSettings->m_nMaxLines);
	GetDlgItem(IDC_EDIT_MAXLINES)->SetWindowText((LPCTSTR) S);

	// IDC_CHECK_HL_LAST
	if(m_pSettings->m_bHighlightCurrentFile)
		((CButton*) GetDlgItem(IDC_CHECK_HL_LAST))->SetCheck(1);
	else
		((CButton*) GetDlgItem(IDC_CHECK_HL_LAST))->SetCheck(0);

	// IDC_CHECK_SHOWPROGRESSONSTART
	if(m_pSettings->m_bShowProgressDialogOnStart)
		((CButton*) GetDlgItem(IDC_CHECK_SHOWPROGRESSONSTART))->SetCheck(1);
	else
		((CButton*) GetDlgItem(IDC_CHECK_SHOWPROGRESSONSTART))->SetCheck(0);

	// IDC_CHECK_SORTFILES
	if(m_pSettings->m_bSortFilesOnAdd)
		((CButton*) GetDlgItem(IDC_CHECK_SORTFILES))->SetCheck(1);
	else
		((CButton*) GetDlgItem(IDC_CHECK_SORTFILES))->SetCheck(0);

	// IDC_CHECK_AUTORETRY
	if(m_pSettings->m_bAutoRetry)
		((CButton*) GetDlgItem(IDC_CHECK_AUTORETRY))->SetCheck(1);
	else
		((CButton*) GetDlgItem(IDC_CHECK_AUTORETRY))->SetCheck(0);

	// IDC_CHECK_SYSTRAY
	if(m_pSettings->m_bSysTrayIcon)
		((CButton*) GetDlgItem(IDC_CHECK_SYSTRAY))->SetCheck(1);
	else
		((CButton*) GetDlgItem(IDC_CHECK_SYSTRAY))->SetCheck(0);

	// IDC_CHECK_SOUND
#ifdef INCLUDESOUND
	if(m_pSettings->m_bSound)
		((CButton*) GetDlgItem(IDC_CHECK_SOUND))->SetCheck(1);
	else
		((CButton*) GetDlgItem(IDC_CHECK_SOUND))->SetCheck(0);
#else
		((CButton*) GetDlgItem(IDC_CHECK_SOUND))->SetCheck(0);
		GetDlgItem(IDC_CHECK_SOUND)->EnableWindow(0);
#endif

	((CButton*) GetDlgItem(IDC_CHECK_DETECTPAR2))->SetCheck(m_pSettings->m_bDetectPAR2?1:0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPropPage_ProgramSettings::ApplyNewSettings()
{
	int k;
	CString S;
	int   nNew_MaxLines;
	BOOL  bHighlightCurrentFile;
	BOOL  bShowProgressDialogOnStart;
	BOOL  bSortFiles;
	BOOL  bAutoRetry;
	BOOL  bSysTray;
	BOOL  bSound;

	// Max Lines
	GetDlgItem(IDC_EDIT_MAXLINES)->GetWindowText(S);
	if((S.GetLength() == 0) || (S.GetLength() > 5))
		{ AfxMessageBox("Program\r\n\r\nInvalid Max-Lines Number Specified\r\n(Range is 500 to 32000)", MB_ICONSTOP); return FALSE; }
	k = atoi((LPCTSTR) S);
	if((k < 500) || (k > 32000))
		{ AfxMessageBox("Program\r\n\r\nInvalid Max-Lines Number Specified\r\n(Range is 500 to 32000)", MB_ICONSTOP); return FALSE; }
	nNew_MaxLines = k;

	// Highlight Current File being Posted
	if(((CButton*) GetDlgItem(IDC_CHECK_HL_LAST))->GetCheck() == 1)
		bHighlightCurrentFile = TRUE;
	else
		bHighlightCurrentFile = FALSE;

	if(((CButton*) GetDlgItem(IDC_CHECK_SHOWPROGRESSONSTART))->GetCheck() == 1)
		bShowProgressDialogOnStart = TRUE;
	else
		bShowProgressDialogOnStart = FALSE;
	
	if(((CButton*) GetDlgItem(IDC_CHECK_SORTFILES))->GetCheck() == 1)
		bSortFiles = TRUE;
	else
		bSortFiles = FALSE;

	if(((CButton*) GetDlgItem(IDC_CHECK_AUTORETRY))->GetCheck() == 1)
		bAutoRetry = TRUE;
	else
		bAutoRetry = FALSE;

	if(((CButton*) GetDlgItem(IDC_CHECK_SYSTRAY))->GetCheck() == 1)
		bSysTray = TRUE;
	else
		bSysTray = FALSE;

	if(((CButton*) GetDlgItem(IDC_CHECK_SOUND))->GetCheck() == 1)
		bSound = TRUE;
	else
		bSound = FALSE;

	// .:. Everything passed OK.. Apply it to object
	m_pSettings->m_nMaxLines = nNew_MaxLines;
	m_pSettings->m_bHighlightCurrentFile = bHighlightCurrentFile;
	m_pSettings->m_bShowProgressDialogOnStart = bShowProgressDialogOnStart;
	m_pSettings->m_bSortFilesOnAdd = bSortFiles;
	m_pSettings->m_bAutoRetry = bAutoRetry;
	m_pSettings->m_bSysTrayIcon = bSysTray;
	m_pSettings->m_bSound = bSound;
	m_pSettings->m_bDetectPAR2=(((CButton*) GetDlgItem(IDC_CHECK_DETECTPAR2))->GetCheck() == 1)?TRUE:FALSE;

	GetDlgItem(IDC_COMBO_SKIN)->GetWindowText(S);
	if((S.GetLength() < MAX_PATH) && (S.GetLength() > 0))
	  strcpy(m_pSettings->m_szSkin, (LPCTSTR) S);

	// Apply SysTray Now
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pMainFrame->SysTrayEnable(bSysTray);
	theApp.ApplySkin(m_pSettings->m_szSkin);
	return TRUE;
}

BOOL CPropPage_ProgramSettings::OnApply() 
{
	if(ApplyNewSettings())
	{
		if(m_pSettings_ApplyTo != NULL)
			m_pSettings_ApplyTo->Copy(*m_pSettings);
		else
		{
			AfxMessageBox("Dev Error - No Object To Apply Settings To", MB_ICONSTOP);
			return FALSE;
		}
		return CPropertyPage::OnApply();
	}
	return FALSE;
}

BOOL CPropPage_ProgramSettings::OnSetActive() 
{
	CMainFrame* pMainFrame = (CMainFrame*) (AfxGetApp()->m_pMainWnd);
	CPostMostView* pView = (CPostMostView*) pMainFrame->GetActiveView();

#ifdef INCLUDESOUND
	if(pView->m_Settings.m_bSound)
		PlaySound((LPCTSTR) IDR_WAVE_CLAP, NULL, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);	
#endif

	return CPropertyPage::OnSetActive();
}

void CPropPage_ProgramSettings::OnCheckHLLast() 
{
	SetModified();
}

void CPropPage_ProgramSettings::OnCheckShowProgressOnStart() 
{
	SetModified();
}

void CPropPage_ProgramSettings::OnCheckSound() 
{
	SetModified();
#ifdef INCLUDESOUND
	if(((CButton*) GetDlgItem(IDC_CHECK_SOUND))->GetCheck() == 1)
		PlaySound((LPCTSTR) IDR_WAVE_CHEESYPOOFS, NULL, SND_ASYNC | SND_RESOURCE);
#endif
}

void CPropPage_ProgramSettings::OnChangeEditMaxLines() 
{
	SetModified();
}

void CPropPage_ProgramSettings::OnCheckSortfiles() 
{
	SetModified();
}

void CPropPage_ProgramSettings::OnCheckAutoRetry() 
{
	SetModified();
}

void CPropPage_ProgramSettings::OnCheckSystray() 
{
	SetModified();
}

void CPropPage_ProgramSettings::OnCheckDetectpar2() 
{
	SetModified();
}

void CPropPage_ProgramSettings::OnEditchangeComboSkin() 
{
	SetModified();
	CString S;
	GetDlgItem(IDC_COMBO_SKIN)->GetWindowText(S);
	if((S.GetLength() < MAX_PATH) && (S.GetLength() > 0))
	theApp.ApplySkin(S);
}

void CPropPage_ProgramSettings::OnSelchangeComboSkin() 
{
	SetModified();
	CString S;
	GetDlgItem(IDC_COMBO_SKIN)->GetWindowText(S);
	if((S.GetLength() < MAX_PATH) && (S.GetLength() > 0))
	theApp.ApplySkin(S);
}
