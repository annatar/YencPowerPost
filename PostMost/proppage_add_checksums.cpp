// PropPage_Add_Checksums.cpp : implementation file
//

#include "stdafx.h"
#include "postmost.h"
#include "PropPage_Add_Checksums.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_Checksums property page

IMPLEMENT_DYNCREATE(CPropPage_Add_Checksums, CPropertyPage)

CPropPage_Add_Checksums::CPropPage_Add_Checksums() : CPropertyPage(CPropPage_Add_Checksums::IDD)
{
	//{{AFX_DATA_INIT(CPropPage_Add_Checksums)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	//m_pPostFiles = NULL;
}

CPropPage_Add_Checksums::~CPropPage_Add_Checksums()
{
}

void CPropPage_Add_Checksums::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPage_Add_Checksums)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPage_Add_Checksums, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPage_Add_Checksums)
	ON_BN_CLICKED(IDC_CHECK_SFV, OnCheckSfv)
	ON_EN_CHANGE(IDC_EDIT_SFV, OnChangeEditSfv)
	ON_BN_CLICKED(IDC_CHECK_GROUPTHREAD, OnCheckGroupthread)
	ON_BN_CLICKED(IDC_BUTTON_SFV, OnButtonSfv)
	ON_BN_CLICKED(IDC_CHECK_PAR2, OnCheckPar2)
	ON_EN_CHANGE(IDC_EDIT_PAR2, OnChangeEditPar2)
	ON_BN_CLICKED(IDC_BUTTON_PAR2, OnButtonPar2)
	ON_EN_CHANGE(IDC_EDIT_PARTSIZE, OnChangeEditPartsize)
	ON_EN_CHANGE(IDC_EDIT_NZB, OnChangeEditNzb)
	ON_BN_CLICKED(IDC_BUTTON_NZB, OnButtonNzb)
	ON_BN_CLICKED(IDC_CHECK_NZB, OnCheckNzb)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_Checksums message handlers

void CPropPage_Add_Checksums::OnCheckSfv() 
{
	int check=((CButton*)GetDlgItem(IDC_CHECK_SFV))->GetCheck();
	GetDlgItem(IDC_EDIT_SFV)->EnableWindow( check );
	GetDlgItem(IDC_BUTTON_SFV)->EnableWindow( check );
	if (check)
		GetDlgItem(IDC_EDIT_SFV)->GetWindowText(m_sSFV);
	else
		m_sSFV=CString("");
}

void CPropPage_Add_Checksums::OnChangeEditSfv() 
{
	int check=((CButton*)GetDlgItem(IDC_CHECK_SFV))->GetCheck();
	if (check)
		GetDlgItem(IDC_EDIT_SFV)->GetWindowText(m_sSFV);
	else
		m_sSFV=CString("");
}

void CPropPage_Add_Checksums::OnCheckGroupthread() 
{
	m_bGroupThread = ((CButton*) GetDlgItem(IDC_CHECK_GROUPTHREAD))->GetCheck();
}

BOOL CPropPage_Add_Checksums::OnInitDialog() 
{
	CString buf;
	CPropertyPage::OnInitDialog();
	*m_pdwActivation |= PAGE__CHECKSUMS;

	((CButton*) GetDlgItem(IDC_CHECK_SFV))->SetCheck(0);
	GetDlgItem(IDC_EDIT_SFV)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SFV)->EnableWindow(FALSE);

	((CButton*) GetDlgItem(IDC_CHECK_PAR2))->SetCheck(0);
	GetDlgItem(IDC_EDIT_PAR2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PAR2)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_PARTSIZE)->EnableWindow(FALSE);

	((CButton*) GetDlgItem(IDC_CHECK_NZB))->SetCheck(0);
	GetDlgItem(IDC_EDIT_NZB)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_NZB)->EnableWindow(FALSE);

	m_sSFV=CString("");

	par2::PostFiles *m_pPostFiles;
	m_pPostFiles = new par2::PostFiles("",m_pDWA_FileNames->GetSize());

	int n;
	for(n = 0; n < m_pDWA_FileNames->GetSize(); n++)
	{
		m_pPostFiles->initFile(n,(LPCTSTR) m_pDWA_FileNames->GetAt(n),true);
	}
	m_pPostFiles->calcbasename();
	if (m_pPostFiles->basename[0])
	{
		buf.Format("%s.sfv",m_pPostFiles->basename);
		GetDlgItem(IDC_EDIT_SFV)->SetWindowText(buf);
		buf.Format("%s.par2",m_pPostFiles->basename);
		GetDlgItem(IDC_EDIT_PAR2)->SetWindowText(buf);
		buf.Format("%s.nzb",m_pPostFiles->basename);
		GetDlgItem(IDC_EDIT_NZB)->SetWindowText(buf);
	}

	buf.Format("%d",m_iPartSize);
	GetDlgItem(IDC_EDIT_PARTSIZE)->SetWindowText(buf);

	delete m_pPostFiles;

	((CButton*) GetDlgItem(IDC_CHECK_GROUPTHREAD))->SetCheck(m_bGroupThread);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropPage_Add_Checksums::OnButtonSfv() 
{
	CString filename;
	GetDlgItem(IDC_EDIT_SFV)->GetWindowText(filename);
	CFileDialog F_Dlg(FALSE,"sfv",filename,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Checksum Files (*.sfv)|*.sfv|All Files (*.*)|*.*||");
/*
	TCHAR szCurrentDirectory[_MAX_PATH];
	::GetCurrentDirectory(_MAX_PATH, szCurrentDirectory);

	TCHAR szInitialDirectory[_MAX_PATH];
	strcpy(szInitialDirectory, m_szDirectory);

	if(strlen(szInitialDirectory) == 0)
		strcpy(szInitialDirectory, szCurrentDirectory);

	F_Dlg.m_ofn.lpstrInitialDir = &szInitialDirectory[0];

	if(!::CM_IsDirectory(F_Dlg.m_ofn.lpstrInitialDir))
		strcpy(szInitialDirectory, szCurrentDirectory);
*/
	if (F_Dlg.DoModal()==IDOK)
	{
		filename = F_Dlg.GetPathName();
		GetDlgItem(IDC_EDIT_SFV)->SetWindowText(filename);
		OnChangeEditSfv();
	}
/*
	// Reset Current Directory
	::SetCurrentDirectory(szCurrentDirectory);
*/		
}

void CPropPage_Add_Checksums::OnCheckPar2() 
{
	int check=((CButton*)GetDlgItem(IDC_CHECK_PAR2))->GetCheck();
	GetDlgItem(IDC_EDIT_PAR2)->EnableWindow( check );
	GetDlgItem(IDC_BUTTON_PAR2)->EnableWindow( check );
	GetDlgItem(IDC_EDIT_PARTSIZE)->EnableWindow( check );
	if (check)
		GetDlgItem(IDC_EDIT_PAR2)->GetWindowText(m_sPAR2);
	else
		m_sPAR2=CString("");
}

void CPropPage_Add_Checksums::OnChangeEditPar2() 
{
	int check=((CButton*)GetDlgItem(IDC_CHECK_PAR2))->GetCheck();
	if (check)
		GetDlgItem(IDC_EDIT_PAR2)->GetWindowText(m_sPAR2);
	else
		m_sPAR2=CString("");
}

void CPropPage_Add_Checksums::OnButtonPar2() 
{
	CString filename;
	GetDlgItem(IDC_EDIT_PAR2)->GetWindowText(filename);
	CFileDialog F_Dlg(FALSE,"par2",filename,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"Parity Volume 2.0 Files (*.par2)|*.par2|All Files (*.*)|*.*||");
	if (F_Dlg.DoModal()==IDOK)
	{
		filename = F_Dlg.GetPathName();
		GetDlgItem(IDC_EDIT_PAR2)->SetWindowText(filename);
		OnChangeEditPar2();
	}
}

void CPropPage_Add_Checksums::OnChangeEditPartsize() 
{
	m_iPartSize = GetDlgItemInt(IDC_EDIT_PARTSIZE);
}

BOOL CPropPage_Add_Checksums::OnApply() 
{

	if(m_iPartSize % 64 != 0)
	{
		CString strResourceBundleString;
		strResourceBundleString.LoadString(IDS_CHECKSUMS_PART_SIZE_MULTIPLE_64);
		AfxMessageBox(LPCTSTR(strResourceBundleString), MB_ICONSTOP);
		m_iPartSize = ((m_iPartSize+63)/64)*64;
		SetDlgItemInt(IDC_EDIT_PARTSIZE,m_iPartSize);
		return(FALSE);
	}
	
	return CPropertyPage::OnApply();
}

void CPropPage_Add_Checksums::OnChangeEditNzb() 
{
	int check=((CButton*)GetDlgItem(IDC_CHECK_NZB))->GetCheck();
	if (check)
		GetDlgItem(IDC_EDIT_NZB)->GetWindowText(m_sNZB);
	else
		m_sNZB=CString("");
}

void CPropPage_Add_Checksums::OnButtonNzb() 
{
	CString filename;
	GetDlgItem(IDC_EDIT_NZB)->GetWindowText(filename);
	CFileDialog F_Dlg(FALSE,"nzb",filename,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"NZB Files (*.nzb)|*.nzb|All Files (*.*)|*.*||");
	if (F_Dlg.DoModal()==IDOK)
	{
		filename = F_Dlg.GetPathName();
		GetDlgItem(IDC_EDIT_NZB)->SetWindowText(filename);
		OnChangeEditNzb();
	}
}

void CPropPage_Add_Checksums::OnCheckNzb() 
{
	int check=((CButton*)GetDlgItem(IDC_CHECK_NZB))->GetCheck();
	GetDlgItem(IDC_EDIT_NZB)->EnableWindow( check );
	GetDlgItem(IDC_BUTTON_NZB)->EnableWindow( check );
	if (check)
		GetDlgItem(IDC_EDIT_NZB)->GetWindowText(m_sNZB);
	else
		m_sNZB=CString("");
}
