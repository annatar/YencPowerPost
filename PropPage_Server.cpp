// PropPage_Server.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "PropPage_Server.h"

#include "MainFrm.h"
#include "PostMostDoc.h"
#include "PostMostView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Server property page

IMPLEMENT_DYNCREATE(CPropPage_Server, CPropertyPage)

CPropPage_Server::CPropPage_Server() : CPropertyPage(CPropPage_Server::IDD)
{
	//{{AFX_DATA_INIT(CPropPage_Server)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pSettings = (CProgramSettings*) NULL;
	m_pSettings_ApplyTo = (CProgramSettings*) NULL;
}

CPropPage_Server::~CPropPage_Server()
{
}

void CPropPage_Server::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPage_Server)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPage_Server, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPage_Server)
	ON_BN_CLICKED(IDC_CHECK_LOGIN, OnCheckLogin)
	ON_EN_CHANGE(IDC_EDIT_PASSWORD, OnChangeEditPassword)
	ON_EN_CHANGE(IDC_EDIT_CONFIRM, OnChangeEditConfirm)
	ON_EN_CHANGE(IDC_EDIT_MAXLINES, OnChangeEditMaxLines)
	ON_EN_CHANGE(IDC_EDIT_PORT, OnChangeEditPort)
	ON_EN_CHANGE(IDC_EDIT_SERVER, OnChangeEditServer)
	ON_EN_CHANGE(IDC_EDIT_USERNAME, OnChangeEditUsername)
	ON_EN_CHANGE(IDC_EDIT_BWCAP, OnChangeEditBwcap)
	ON_CBN_EDITCHANGE(IDC_COMBO_SERVER, OnEditchangeComboServer)
	ON_CBN_SELCHANGE(IDC_COMBO_SERVER, OnSelchangeComboServer)
	ON_CBN_SELENDOK(IDC_COMBO_SERVER, OnSelendokComboServer)
	ON_EN_CHANGE(IDC_EDIT_CONNECTIONS, OnChangeEditConnections)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Server message handlers

BOOL CPropPage_Server::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	CString S;

	// Set Server Name

	for (int i=0;i<CProgramSettings::NSERVERS;i++)
	{
		if (m_pSettings->servers[i].m_szServerName[0])
			((CComboBox*) GetDlgItem(IDC_COMBO_SERVER))->AddString(m_pSettings->servers[i].m_szServerName);
	}


	((CComboBox*) GetDlgItem(IDC_COMBO_SERVER))->SetWindowText(m_pSettings->m_szServerName);

	// Set Port
	S.Format("%d", m_pSettings->m_nServerPort);
	((CEdit*) GetDlgItem(IDC_EDIT_PORT))->SetWindowText((LPCTSTR) S);

	// Set Connections
	((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_CONNECTIONS))->SetRange(1,NPOSTTHREAD);
	((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_CONNECTIONS))->SetPos(m_pSettings->m_nConnections);

	// Set Login Checkbox
	if(m_pSettings->m_bRequireLogin)
		((CButton*) GetDlgItem(IDC_CHECK_LOGIN))->SetCheck(1);
	else
		((CButton*) GetDlgItem(IDC_CHECK_LOGIN))->SetCheck(0);

	((CEdit*) GetDlgItem(IDC_EDIT_USERNAME))->SetWindowText(m_pSettings->m_szUsername);
	((CEdit*) GetDlgItem(IDC_EDIT_PASSWORD))->SetWindowText(m_pSettings->m_szPassword);
	((CEdit*) GetDlgItem(IDC_EDIT_CONFIRM))->SetWindowText(m_pSettings->m_szPassword);

	if(m_pSettings->m_bRequireLogin == FALSE)
	{
		GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_CONFIRM)->EnableWindow(FALSE);
	}

	// Set Bandwidth
	S.Format("%d", m_pSettings->m_nBandwidthCap);
	((CEdit*) GetDlgItem(IDC_EDIT_BWCAP))->SetWindowText((LPCTSTR) S);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropPage_Server::OnCheckLogin() 
{
	SetModified(TRUE);

	int bCheck = ((CButton*) GetDlgItem(IDC_CHECK_LOGIN))->GetCheck();

	GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow((bCheck == 1) ? TRUE : FALSE);
	GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow((bCheck == 1) ? TRUE : FALSE);
	GetDlgItem(IDC_EDIT_CONFIRM)->EnableWindow((bCheck == 1) ? TRUE : FALSE);
}

void CPropPage_Server::OnChangeEditPassword() 
{
	SetModified(TRUE);
	((CEdit*) GetDlgItem(IDC_EDIT_CONFIRM))->SetWindowText("");	
}

void CPropPage_Server::OnChangeEditConfirm() 
{
	SetModified(TRUE);
}

void CPropPage_Server::OnChangeEditMaxLines() 
{
	SetModified(TRUE);
}

void CPropPage_Server::OnChangeEditPort() 
{
	SetModified(TRUE);
}

void CPropPage_Server::OnChangeEditServer() 
{
	SetModified(TRUE);
}

void CPropPage_Server::OnChangeEditUsername() 
{
	SetModified(TRUE);
}

BOOL CPropPage_Server::OnApply() 
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

BOOL CPropPage_Server::ApplyNewSettings()
{
	int k;
	CString S;
	char szNew_ServerName[2*132];
	int   nNew_ServerPort;
	int   nNew_Connections;
	BOOL  bNew_LoginRequired;
	char szNew_Username[132];
	char szNew_Password[132];
	char szNew_Confirm[132];
	int  nNew_nBandwidthCap;

	// Server Name
	GetDlgItem(IDC_COMBO_SERVER)->GetWindowText(S);
	if((S.GetLength() < 2*132) && (S.GetLength() > 0))
		strcpy(szNew_ServerName, (LPCTSTR) S);
	else { AfxMessageBox("News Server\r\n\r\nInvalid Server Name Specified (Specify, 263 Chars Max)", MB_ICONSTOP); return FALSE; }
	
	// Port
	GetDlgItem(IDC_EDIT_PORT)->GetWindowText(S);
	if((S.GetLength() == 0) || (S.GetLength() > 5))
		{ AfxMessageBox("News Server\r\n\r\nInvalid Port Number Specified", MB_ICONSTOP); return FALSE; }
	k = atoi((LPCTSTR) S);
	if((k < 1) || (k > 65535))
		{ AfxMessageBox("News Server\r\n\r\nInvalid Port Number Specified", MB_ICONSTOP); return FALSE; }
	nNew_ServerPort = k;

	// Connections
	nNew_Connections = GetDlgItemInt(IDC_EDIT_CONNECTIONS); //((CSliderCtrl*) GetDlgItem(IDC_CONNECTIONS))->GetPos();

	// BW
	GetDlgItem(IDC_EDIT_BWCAP)->GetWindowText(S);
	k = atoi((LPCTSTR) S);
	if(k < 0) k=0;
	nNew_nBandwidthCap = k;
	
	// Login Flag
	k = ((CButton*) GetDlgItem(IDC_CHECK_LOGIN))->GetCheck();
	if(k == 0)
		bNew_LoginRequired = FALSE;
	else
		bNew_LoginRequired = TRUE;

	// Username
	GetDlgItem(IDC_EDIT_USERNAME)->GetWindowText(S);
	if((S.GetLength() < 132) && (S.GetLength() > 0))
		strcpy(szNew_Username, (LPCTSTR) S);
	else if(bNew_LoginRequired)
	{
		AfxMessageBox("News Server\r\n\r\nInvalid Username Specified (Specify, 132 Chars Max)", MB_ICONSTOP); 
		return FALSE; 
	} else strcpy(szNew_Username, (LPCTSTR) S);

	// Password
	GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowText(S);
	if((S.GetLength() < 132) && (S.GetLength() > 0))
		strcpy(szNew_Password, (LPCTSTR) S);
	else if(bNew_LoginRequired)
	{
		AfxMessageBox("News Server\r\n\r\nInvalid Password Specified (Specify, 132 Chars Max)", MB_ICONSTOP); 
		return FALSE; 
	} else strcpy(szNew_Password, (LPCTSTR) S);
	
	// Confirm Password
	GetDlgItem(IDC_EDIT_CONFIRM)->GetWindowText(S);
	if((S.GetLength() < 132) && (S.GetLength() > 0))
		strcpy(szNew_Confirm, (LPCTSTR) S);
	else if(bNew_LoginRequired)
	{
		AfxMessageBox("News Server\r\n\r\nInvalid Confirm Password Specified (Specify, 132 Chars Max)", MB_ICONSTOP); 
		return FALSE; 
	} else strcpy(szNew_Confirm, (LPCTSTR) S);

	// Verify Password == Confirm Password
	if((bNew_LoginRequired == TRUE) && (strcmp(szNew_Password, szNew_Confirm) != 0))
	{
		AfxMessageBox("News Server\r\n\r\nPassword And Confirm Password Do Not Match!", MB_ICONSTOP); 
		return FALSE; 
	}

	// .:. Everything passed OK.. Apply it to object
	m_pSettings->m_nServerPort = nNew_ServerPort;
	m_pSettings->m_bRequireLogin = bNew_LoginRequired;
	m_pSettings->m_nConnections = nNew_Connections;
	strcpy(m_pSettings->m_szServerName, szNew_ServerName);
	strcpy(m_pSettings->m_szUsername, szNew_Username);
	strcpy(m_pSettings->m_szPassword, szNew_Password);
	m_pSettings->m_nBandwidthCap = nNew_nBandwidthCap;

	m_pSettings->PushServerSettings();

	return TRUE;
}

BOOL CPropPage_Server::OnSetActive() 
{
	CMainFrame* pMainFrame = (CMainFrame*) (AfxGetApp()->m_pMainWnd);
	CPostMostView* pView = (CPostMostView*) pMainFrame->GetActiveView();

#ifdef INCLUDESOUND
	if(pView->m_Settings.m_bSound)
		PlaySound((LPCTSTR) IDR_WAVE_CLAP, NULL, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
#endif

	return CPropertyPage::OnSetActive();
}

void CPropPage_Server::OnChangeEditBwcap() 
{
	SetModified(TRUE);
}

void CPropPage_Server::OnEditchangeComboServer() 
{
	SetModified(TRUE);
}

void CPropPage_Server::OnSelchangeComboServer() 
{
	/*
	SetModified(TRUE);
	CString S;
	GetDlgItem(IDC_COMBO_SERVER)->GetWindowText(S);

	for (int i=0;i<CProgramSettings::NSERVERS;i++)
	{
		if (!strcmp(S,m_pSettings->servers[i].m_szServerName)) {

			// Set Port
			S.Format("%d", m_pSettings->servers[i].m_nServerPort);
			((CEdit*) GetDlgItem(IDC_EDIT_PORT))->SetWindowText((LPCTSTR) S);

			// Set Login Checkbox
			if(m_pSettings->servers[i].m_bRequireLogin)
				((CButton*) GetDlgItem(IDC_CHECK_LOGIN))->SetCheck(1);
			else
				((CButton*) GetDlgItem(IDC_CHECK_LOGIN))->SetCheck(0);

			((CEdit*) GetDlgItem(IDC_EDIT_USERNAME))->SetWindowText(m_pSettings->servers[i].m_szUsername);
			((CEdit*) GetDlgItem(IDC_EDIT_PASSWORD))->SetWindowText(m_pSettings->servers[i].m_szPassword);
			((CEdit*) GetDlgItem(IDC_EDIT_CONFIRM))->SetWindowText(m_pSettings->servers[i].m_szPassword);

			if(m_pSettings->servers[i].m_bRequireLogin == FALSE)
			{
				GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(FALSE);
				GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(FALSE);
				GetDlgItem(IDC_EDIT_CONFIRM)->EnableWindow(FALSE);
			}
			return;
		}
	}
	*/
}

void CPropPage_Server::OnSelendokComboServer() 
{
	SetModified(TRUE);
	CString S;
	//GetDlgItem(IDC_COMBO_SERVER)->GetWindowText(S);
	CComboBox* cb = (CComboBox*)GetDlgItem(IDC_COMBO_SERVER);
	int ind = cb->GetCurSel();
	if (ind==CB_ERR) return;
	cb->GetLBText(ind,S);

	for (int i=0;i<CProgramSettings::NSERVERS;i++)
	{
		if (!strcmp(S,m_pSettings->servers[i].m_szServerName)) {

			// Set Port
			S.Format("%d", m_pSettings->servers[i].m_nServerPort);
			((CEdit*) GetDlgItem(IDC_EDIT_PORT))->SetWindowText((LPCTSTR) S);

			// Set Login Checkbox
			if(m_pSettings->servers[i].m_bRequireLogin)
				((CButton*) GetDlgItem(IDC_CHECK_LOGIN))->SetCheck(1);
			else
				((CButton*) GetDlgItem(IDC_CHECK_LOGIN))->SetCheck(0);

			((CEdit*) GetDlgItem(IDC_EDIT_USERNAME))->SetWindowText(m_pSettings->servers[i].m_szUsername);
			((CEdit*) GetDlgItem(IDC_EDIT_PASSWORD))->SetWindowText(m_pSettings->servers[i].m_szPassword);
			((CEdit*) GetDlgItem(IDC_EDIT_CONFIRM))->SetWindowText(m_pSettings->servers[i].m_szPassword);
			//((CSliderCtrl*) GetDlgItem(IDC_CONNECTIONS))->SetPos(m_pSettings->servers[i].m_nConnections);
			((CSpinButtonCtrl*) GetDlgItem(IDC_SPIN_CONNECTIONS))->SetPos(m_pSettings->m_nConnections);

			//if(m_pSettings->servers[i].m_bRequireLogin == FALSE)
			{
				GetDlgItem(IDC_EDIT_USERNAME)->EnableWindow(m_pSettings->servers[i].m_bRequireLogin);
				GetDlgItem(IDC_EDIT_PASSWORD)->EnableWindow(m_pSettings->servers[i].m_bRequireLogin);
				GetDlgItem(IDC_EDIT_CONFIRM)->EnableWindow(m_pSettings->servers[i].m_bRequireLogin);
			}
			return;
		}
	}
}

void CPropPage_Server::OnChangeEditConnections() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}
