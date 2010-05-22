// PropPage_Add_Subject.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "PropPage_Add_Subject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_Subject property page

IMPLEMENT_DYNCREATE(CPropPage_Add_Subject, CPropertyPage)

CPropPage_Add_Subject::CPropPage_Add_Subject() : CPropertyPage(CPropPage_Add_Subject::IDD)
{
	//{{AFX_DATA_INIT(CPropPage_Add_Subject)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bSound = FALSE;
	m_nDir = 0;
	m_bInsertFilesAtBeginningOfQueue = FALSE;
	m_bProcessSubdirectories = FALSE;
	m_bRecursive = FALSE;
	m_bCountDirSeperate = FALSE;
	m_pDroppedFiles = (CStringArray*) NULL;
	m_pdwActivation = (DWORD*) NULL;

	sprintf(m_Ex_szFilename, "FILE.ZYX");
	sprintf(m_Ex_szDirectory, "DIRECTORY");
	m_Ex_Filesize = 1048576;
	m_Ex_nFileCount = 10;

	m_bFirstSound = TRUE;	// To keep first OnSetActive() sound from happening
	m_bGroupsOK = FALSE;
}

CPropPage_Add_Subject::~CPropPage_Add_Subject()
{
}

void CPropPage_Add_Subject::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPage_Add_Subject)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPage_Add_Subject, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPage_Add_Subject)
	ON_EN_CHANGE(IDC_EDIT_SUBJECT_TEMPLATE, OnChangeEditSubjectTemplate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_Subject message handlers

BOOL CPropPage_Add_Subject::OnApply() 
{
	if(((*m_pdwActivation) & PAGE__GROUPS) == 0 && !m_bGroupsOK)
	{
		AfxMessageBox("No Groups Selected", MB_ICONSTOP);
		return(FALSE);
	}

	TCHAR szTemplate[128];
	GetDlgItem(IDC_EDIT_SUBJECT_TEMPLATE)->GetWindowText(szTemplate, 127);

	if (CString(szTemplate) == m_last_strSubjectPrefixTemplate)
	{
		if (AfxMessageBox("You did not change the subject line.\nIs it ok?", MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2)==IDNO)
			return (FALSE);
	}

	TCHAR szSubject[512];

	if(::CM_BuildSubjectFromTemplate(szTemplate, m_Ex_szDirectory, m_Ex_szFilename, m_Ex_Filesize, 1, m_Ex_nFileCount, szSubject) == FALSE)
	{
		AfxMessageBox("Subject template must include $F", MB_ICONSTOP);
		return FALSE;
	}
	
	
	m_strSubjectPrefixTemplate = (LPCTSTR) szTemplate;

	// AB begin
	for (int i=0;i<m_strSubjectPrefixTemplate.GetLength();i++) szTemplate[i]=tolower(szTemplate[i]);
	/*
	if (!strstr(szTemplate,"yenc")) {
		AfxMessageBox("Subject template must include yEnc", MB_ICONSTOP);
		return FALSE;
	}
	*/
	// AB end

	// Save Checkbox Options

	// Insert Files at beginning of queue..
	if(((CButton*) GetDlgItem(IDC_CHECK_INSERT_HEAD))->GetCheck() == 1)
		m_bInsertFilesAtBeginningOfQueue = TRUE;
	else
		m_bInsertFilesAtBeginningOfQueue = FALSE;

	// Process Directories
	if(((CButton*) GetDlgItem(IDC_CHECK_PROCESS_DIRECTORIES))->GetCheck() == 1)
		m_bProcessSubdirectories = TRUE;
	else
		m_bProcessSubdirectories = FALSE;

	// Enumerate by Directory
	if(((CButton*) GetDlgItem(IDC_CHECK_ENUMBYDIR))->GetCheck() == 1)
		m_bCountDirSeperate = TRUE;
	else
		m_bCountDirSeperate = FALSE;
	
	// Recursive
	if(((CButton*) GetDlgItem(IDC_CHECK_RECURSIVE))->GetCheck() == 1)
		m_bRecursive = TRUE;
	else
		m_bRecursive = FALSE;
	

	return CPropertyPage::OnApply();
}

BOOL CPropPage_Add_Subject::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	*m_pdwActivation |= PAGE__SUBJECT;

	//m_last_strSubjectPrefixTemplate = m_strSubjectPrefixTemplate;
	
	CEdit* pEdit = (CEdit*) GetDlgItem(IDC_EDIT_SUBJECT_TEMPLATE);
	pEdit->LimitText(112);
	pEdit->SetWindowText(m_strSubjectPrefixTemplate);

	TCHAR szExampleInfo[512];
	sprintf(szExampleInfo, "(Example is %s, 1 of %d, ", m_Ex_szFilename, m_Ex_nFileCount);
	::CM_strcat_FileSizeKiloByte(szExampleInfo, m_Ex_Filesize);
	strcat(szExampleInfo, "k)");
	SetDlgItemText(IDC_STATIC_EXAMPLE_INFO, szExampleInfo);

	// If no directories, disable the process directories and enum by dir options
	if(m_nDir == 0)
	{
		GetDlgItem(IDC_CHECK_PROCESS_DIRECTORIES)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_RECURSIVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_ENUMBYDIR)->EnableWindow(FALSE);
	}
	else
	{
		if(m_pDroppedFiles->GetSize() == 0)
		{
			// Only makes sense to process directories; check it and disable window
			GetDlgItem(IDC_CHECK_PROCESS_DIRECTORIES)->EnableWindow(FALSE);
			((CButton*) GetDlgItem(IDC_CHECK_PROCESS_DIRECTORIES))->SetCheck(1);
		}
	}

	OnChangeEditSubjectTemplate();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropPage_Add_Subject::OnChangeEditSubjectTemplate() 
{
	TCHAR szTemplate[128];
	TCHAR szSubject[512];

	GetDlgItem(IDC_EDIT_SUBJECT_TEMPLATE)->GetWindowText(szTemplate, 127);

	::CM_BuildSubjectFromTemplate(szTemplate, m_Ex_szDirectory, m_Ex_szFilename, m_Ex_Filesize, 1, m_Ex_nFileCount, szSubject);

	SetDlgItemText(IDC_STATIC_EX_OUTPUT, szSubject);

}

BOOL CPropPage_Add_Subject::OnSetActive() 
{
	if(m_bSound)
	{
#ifdef INCLUDESOUND
		if(m_bFirstSound == FALSE)
			PlaySound((LPCTSTR) IDR_WAVE_CLAP, NULL, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);	
#endif
		m_bFirstSound = FALSE;
	}

	return CPropertyPage::OnSetActive();
}
