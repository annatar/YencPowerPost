// PropPage_Headers.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "PropPage_Headers.h"

#include "MainFrm.h"
#include "PostMostDoc.h"
#include "PostMostView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Headers property page

IMPLEMENT_DYNCREATE(CPropPage_Headers, CPropertyPage)

CPropPage_Headers::CPropPage_Headers() : CPropertyPage(CPropPage_Headers::IDD)
{
	//{{AFX_DATA_INIT(CPropPage_Headers)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pSettings = (CProgramSettings*) NULL;
	m_pSettings_ApplyTo = (CProgramSettings*) NULL;
}

CPropPage_Headers::~CPropPage_Headers()
{
}

void CPropPage_Headers::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPage_Headers)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPage_Headers, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPage_Headers)
	ON_EN_CHANGE(IDC_EDIT_FROM, OnChangeEditFrom)
	ON_EN_CHANGE(IDC_EDIT_ORG, OnChangeEditOrg)
	ON_EN_CHANGE(IDC_EDIT_REPLYTO, OnChangeEditReplyTo)
	ON_BN_CLICKED(IDC_CHECK_XNOARCHIVE, OnCheckXNoArchive)
	ON_EN_CHANGE(IDC_EDIT_FROM_NICK, OnChangeEditFromNick)
	ON_EN_CHANGE(IDC_EDIT_FOLLOWUPTO, OnChangeEditFollowupTo)
	ON_BN_CLICKED(IDC_CHECK_XSERVERDATE, OnCheckXServerDate)
	ON_EN_CHANGE(IDC_EDIT_DOMAINID, OnChangeEditDomainid)
	ON_BN_CLICKED(IDC_CHECK_FILETHREAD, OnCheckFilethread)
	ON_BN_CLICKED(IDC_CHECK_HEADERPARTIAL, OnCheckPartialheader)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Headers message handlers

BOOL CPropPage_Headers::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Set fields
	GetDlgItem(IDC_EDIT_FROM_NICK)->SetWindowText(m_pSettings->m_szFromNick);
	GetDlgItem(IDC_EDIT_FROM)->SetWindowText(m_pSettings->m_szFrom);
	GetDlgItem(IDC_EDIT_REPLYTO)->SetWindowText(m_pSettings->m_szReplyTo);
	GetDlgItem(IDC_EDIT_ORGANIZATION)->SetWindowText(m_pSettings->m_szOrganization);
	GetDlgItem(IDC_EDIT_FOLLOWUPTO)->SetWindowText(m_pSettings->m_szFollowupTo);
	GetDlgItem(IDC_EDIT_DOMAINID)->SetWindowText(m_pSettings->m_szDomainID);
	
	((CEdit*) GetDlgItem(IDC_EDIT_FROM_NICK))->LimitText(32);
	((CEdit*) GetDlgItem(IDC_EDIT_FROM))->LimitText(80);
	((CEdit*) GetDlgItem(IDC_EDIT_REPLYTO))->LimitText(130);
	((CEdit*) GetDlgItem(IDC_EDIT_ORGANIZATION))->LimitText(130);
	((CEdit*) GetDlgItem(IDC_EDIT_FOLLOWUPTO))->LimitText(130);

	if(m_pSettings->m_bXNoArchiveHeader)
		((CButton*) GetDlgItem(IDC_CHECK_XNOARCHIVE))->SetCheck(1);
	else
		((CButton*) GetDlgItem(IDC_CHECK_XNOARCHIVE))->SetCheck(0);

	if(m_pSettings->m_bXServerDate)
		((CButton*) GetDlgItem(IDC_CHECK_XSERVERDATE))->SetCheck(1);
	else
		((CButton*) GetDlgItem(IDC_CHECK_XSERVERDATE))->SetCheck(0);

	//if(m_pSettings->m_bHeaderPartial)
	//	((CButton*) GetDlgItem(IDC_CHECK_HEADERPARTIAL))->SetCheck(1);
	//else
	//	((CButton*) GetDlgItem(IDC_CHECK_HEADERPARTIAL))->SetCheck(0);

	((CButton*) GetDlgItem(IDC_CHECK_FILETHREAD))->SetCheck((m_pSettings->m_bFileThread?1:0));

	GetDlgItem(IDC_EDIT_DOMAINID)->EnableWindow( TRUE ); //((CButton*)GetDlgItem(IDC_CHECK_FILETHREAD))->GetCheck() );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropPage_Headers::OnChangeEditFrom() 
{
	SetModified(TRUE);
}

void CPropPage_Headers::OnChangeEditOrg() 
{
	SetModified(TRUE);
}

void CPropPage_Headers::OnChangeEditReplyTo() 
{
	SetModified(TRUE);
}

BOOL CPropPage_Headers::OnApply() 
{
	if(ApplyNewSettings())
	{
		if(m_pSettings_ApplyTo != NULL)
			m_pSettings_ApplyTo->Copy(*m_pSettings);
		else
		{
			AfxMessageBox("Application Error - No Object To Apply Settings To", MB_ICONSTOP);
			return FALSE;
		}
		return CPropertyPage::OnApply();
	}
	return FALSE;
}

BOOL CPropPage_Headers::ApplyNewSettings()
{
	CString S;
	BOOL bXNoArchiveHeader;
	BOOL bXServerDate;
	BOOL bFileThread;
	BOOL bHeaderPartial;
	char szNew_FromNick[64];
	char szNew_From[132];
	char szNew_ReplyTo[132];
	char szNew_Organization[132];
	char szNew_FollowupTo[132];
	char szNew_DomainID[132];

	// From Nick / Nickname / Handle
	GetDlgItem(IDC_EDIT_FROM_NICK)->GetWindowText(S);
	strcpy(szNew_FromNick, (LPCTSTR) S);

	// From
	GetDlgItem(IDC_EDIT_FROM)->GetWindowText(S);
	if(S.GetLength() == 0)
	{ 
		AfxMessageBox("Headers\r\n\r\n'(e-mail) From:' Field Is Required!", MB_ICONSTOP); 
		return FALSE;
	}
	strcpy(szNew_From, (LPCTSTR) S);

	// ReplyTo
	GetDlgItem(IDC_EDIT_REPLYTO)->GetWindowText(S);
	strcpy(szNew_ReplyTo, (LPCTSTR) S);

	// Subject Prefix
	GetDlgItem(IDC_EDIT_ORGANIZATION)->GetWindowText(S);
	strcpy(szNew_Organization, (LPCTSTR) S);

	// Followup-To
	GetDlgItem(IDC_EDIT_FOLLOWUPTO)->GetWindowText(S);
	strcpy(szNew_FollowupTo, (LPCTSTR) S);

	if(((CButton*) GetDlgItem(IDC_CHECK_XNOARCHIVE))->GetCheck() == 1)
		bXNoArchiveHeader = TRUE;
	else
		bXNoArchiveHeader = FALSE;
	
	if(((CButton*) GetDlgItem(IDC_CHECK_XSERVERDATE))->GetCheck() == 1)
		bXServerDate = TRUE;
	else
		bXServerDate = FALSE;

	//if(((CButton*) GetDlgItem(IDC_CHECK_HEADERPARTIAL))->GetCheck() == 1)
	//	bHeaderPartial= TRUE;
	//else
		bHeaderPartial = FALSE;
	
	if(((CButton*) GetDlgItem(IDC_CHECK_FILETHREAD))->GetCheck() == 1)
		bFileThread = TRUE;
	else
		bFileThread = FALSE;

	// DomainID
	GetDlgItem(IDC_EDIT_DOMAINID)->GetWindowText(S);
	strncpy(szNew_DomainID, (LPCTSTR) S, 132);
	
	// .:. Everything passed OK.. Apply it to object
	m_pSettings->m_bXNoArchiveHeader = bXNoArchiveHeader;
	m_pSettings->m_bXServerDate = bXServerDate;
	m_pSettings->m_bHeaderPartial = bHeaderPartial;
	m_pSettings->m_bFileThread = bFileThread;
	strcpy(m_pSettings->m_szFromNick, szNew_FromNick);
	strcpy(m_pSettings->m_szFrom, szNew_From);
	strcpy(m_pSettings->m_szReplyTo, szNew_ReplyTo);
	strcpy(m_pSettings->m_szOrganization, szNew_Organization);
	strcpy(m_pSettings->m_szFollowupTo, szNew_FollowupTo);
	strcpy(m_pSettings->m_szDomainID, szNew_DomainID);
	return TRUE;
	
}

BOOL CPropPage_Headers::OnSetActive() 
{
	CMainFrame* pMainFrame = (CMainFrame*) (AfxGetApp()->m_pMainWnd);
	CPostMostView* pView = (CPostMostView*) pMainFrame->GetActiveView();

#ifdef INCLUDESOUND
	if(pView->m_Settings.m_bSound)
		PlaySound((LPCTSTR) IDR_WAVE_CLAP, NULL, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
#endif

	return CPropertyPage::OnSetActive();
}

void CPropPage_Headers::OnCheckXNoArchive() 
{
	SetModified();	
}

void CPropPage_Headers::OnChangeEditFromNick() 
{
	SetModified();
}

void CPropPage_Headers::OnChangeEditFollowupTo() 
{
	SetModified();
}

void CPropPage_Headers::OnCheckXServerDate() 
{
	SetModified();
}

void CPropPage_Headers::OnChangeEditDomainid() 
{
	SetModified();
}

void CPropPage_Headers::OnCheckFilethread() 
{
	SetModified();
	//GetDlgItem(IDC_EDIT_DOMAINID)->EnableWindow( ((CButton*)GetDlgItem(IDC_CHECK_FILETHREAD))->GetCheck() );
}

void CPropPage_Headers::OnCheckPartialheader() 
{
	SetModified();
}
