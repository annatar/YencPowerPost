// PropPage_Add_TextPrefix.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "PropPage_Add_TextPrefix.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_TextPrefix property page

IMPLEMENT_DYNCREATE(CPropPage_Add_TextPrefix, CPropertyPage)

CPropPage_Add_TextPrefix::CPropPage_Add_TextPrefix() : CPropertyPage(CPropPage_Add_TextPrefix::IDD)
{
	//{{AFX_DATA_INIT(CPropPage_Add_TextPrefix)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pdwActivation = (DWORD*) NULL;
	m_bSound = FALSE;
	m_bSendTextPrefix = FALSE;
	m_bSendTextFirstFileOnly = FALSE;
	m_bFirstChange = TRUE;
}

CPropPage_Add_TextPrefix::~CPropPage_Add_TextPrefix()
{
}

void CPropPage_Add_TextPrefix::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPage_Add_TextPrefix)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPage_Add_TextPrefix, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPage_Add_TextPrefix)
	ON_EN_CHANGE(IDC_EDIT_PREFIX_TEXT, OnChangeEditPrefixText)
	ON_BN_CLICKED(IDC_CHECK_SEND_TEXT_PREFIX, OnCheckSendTextPrefix)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_TextPrefix message handlers

BOOL CPropPage_Add_TextPrefix::OnApply() 
{
	GetDlgItem(IDC_EDIT_PREFIX_TEXT)->GetWindowText(m_strTextPrefix);
	if(((CButton*) GetDlgItem(IDC_CHECK_SEND_TEXT_PREFIX))->GetCheck() == 1)
	{
		m_bSendTextPrefix = TRUE;
		if(((CButton*) GetDlgItem(IDC_RADIO_PREFIX_ALL))->GetCheck() == 1)
			m_bSendTextFirstFileOnly = FALSE;
		else
			m_bSendTextFirstFileOnly = TRUE;
	}
	else
	{
		m_bSendTextPrefix = FALSE;
	}

	return CPropertyPage::OnApply();
}

void CPropPage_Add_TextPrefix::OnChangeEditPrefixText() 
{
	if(GetDlgItem(IDC_EDIT_PREFIX_TEXT)->GetWindowTextLength() == 0)
	{
		((CButton*) GetDlgItem(IDC_CHECK_SEND_TEXT_PREFIX))->SetCheck(0);
		GetDlgItem(IDC_RADIO_PREFIX_ALL)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_PREFIX_FIRST_ONLY)->EnableWindow(FALSE);
		m_bFirstChange = TRUE;
	}
	else
	{
		if(m_bFirstChange)
		{
			((CButton*) GetDlgItem(IDC_CHECK_SEND_TEXT_PREFIX))->SetCheck(1);
			GetDlgItem(IDC_RADIO_PREFIX_ALL)->EnableWindow(TRUE);
			GetDlgItem(IDC_RADIO_PREFIX_FIRST_ONLY)->EnableWindow(TRUE);
			m_bFirstChange = FALSE;
		}
	}
}

void CPropPage_Add_TextPrefix::OnCheckSendTextPrefix() 
{
	int nCheckValue = ((CButton*) GetDlgItem(IDC_CHECK_SEND_TEXT_PREFIX))->GetCheck();

	GetDlgItem(IDC_RADIO_PREFIX_ALL)->EnableWindow(nCheckValue == 1);
	GetDlgItem(IDC_RADIO_PREFIX_FIRST_ONLY)->EnableWindow(nCheckValue == 1);
}

BOOL CPropPage_Add_TextPrefix::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	*m_pdwActivation |= PAGE__TEXT_PREFIX;

	((CButton*) GetDlgItem(IDC_CHECK_SEND_TEXT_PREFIX))->SetCheck(0);
	((CButton*) GetDlgItem(IDC_RADIO_PREFIX_ALL))->SetCheck(0);
	((CButton*) GetDlgItem(IDC_RADIO_PREFIX_FIRST_ONLY))->SetCheck(1);
	GetDlgItem(IDC_RADIO_PREFIX_ALL)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_PREFIX_FIRST_ONLY)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPropPage_Add_TextPrefix::OnSetActive() 
{
#ifdef INCLUDESOUND
	if(m_bSound)
		PlaySound((LPCTSTR) IDR_WAVE_CLAP, NULL, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);	
#endif
	
	return CPropertyPage::OnSetActive();
}
