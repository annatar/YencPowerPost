// PropPage_Add_Recovery.cpp : implementation file
//

#include "stdafx.h"
#include "postmost.h"
#include "PropPage_Add_Recovery.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_Recovery property page

char* format_size(unsigned long size)
{
	static char buffer[256];
	if (size>=(1<<20))
	{
		size>>=10;
		sprintf(buffer,"%ld.%02ld MB",size>>10,((size&((1<<10)-1))*100)>>10);
	}
	else if (size>=(1<<10))
	{
		sprintf(buffer,"%ld.%02ld KB",size>>10,((size&((1<<10)-1))*100)>>10);
	}
	else
		sprintf(buffer,"%ld B",size);
	return buffer;	
}


IMPLEMENT_DYNCREATE(CPropPage_Add_Recovery, CPropertyPage)

CPropPage_Add_Recovery::CPropPage_Add_Recovery() : CPropertyPage(CPropPage_Add_Recovery::IDD)
{
	m_bLockUpdates = true;
	//{{AFX_DATA_INIT(CPropPage_Add_Recovery)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	//m_pPostFiles = NULL;
}

CPropPage_Add_Recovery::~CPropPage_Add_Recovery()
{
}

void CPropPage_Add_Recovery::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropPage_Add_Recovery)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropPage_Add_Recovery, CPropertyPage)
	//{{AFX_MSG_MAP(CPropPage_Add_Recovery)
	ON_BN_CLICKED(IDC_CHECK_GROUPTHREAD, OnCheckGroupthread)
	ON_EN_CHANGE(IDC_EDIT_PAR2, OnChangeEditPar2)
	ON_EN_CHANGE(IDC_EDIT_FIRST, OnChangeEditFirst)
	ON_EN_CHANGE(IDC_EDIT_BLOCKS, OnChangeEditBlocks)
	ON_EN_CHANGE(IDC_EDIT_FILES, OnChangeEditFiles)
	ON_BN_CLICKED(IDC_CHECK_SAMESIZE, OnCheckSamesize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_Recovery message handlers

void CPropPage_Add_Recovery::OnCheckGroupthread() 
{
	if (m_bLockUpdates) return;
	m_bGroupThread = ((CButton*) GetDlgItem(IDC_CHECK_GROUPTHREAD))->GetCheck();
}

BOOL CPropPage_Add_Recovery::OnInitDialog() 
{
	CString buf;
	m_bLockUpdates = true;
	CPropertyPage::OnInitDialog();
	*m_pdwActivation |= PAGE__RECOVERY;

	buf.Format("Block size: %d\r\n"
						 "Number of Source Files: %d \r\n"
						 "Number of Source Blocks: %d \r\n"
						 "Total Size of Source Data: %s \r\n",
						 m_iPartSize,
						 m_iSourceFiles,
						 m_iSourceChunks,
						 format_size(m_iSourceSize));
	SetDlgItemText(IDC_TEXT_PAR2_INFO,buf);

	SetDlgItemText(IDC_EDIT_PAR2,m_sPAR2);

	//buf.Format("%d",m_iFirstChunks);
	//SetDlgItemText(IDC_EDIT_FIRST,buf);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_FIRST))->SetRange(0,32767);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_FIRST))->SetPos(m_iFirstChunks);

	//buf.Format("%d",m_iNbChunks);
	//SetDlgItemText(IDC_EDIT_BLOCKS,buf);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_BLOCKS))->SetRange(0,m_iSourceChunks);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_BLOCKS))->SetPos(m_iNbChunks);

	//buf.Format("%d",m_iMaxPartsPerFile);
	//SetDlgItemText(IDC_EDIT_FILES,buf);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_FILES))->SetRange(0,32767);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_FILES))->SetPos(m_iMaxPartsPerFile);

	((CButton*) GetDlgItem(IDC_CHECK_SAMESIZE))->SetCheck(m_bSameSize);
	((CButton*) GetDlgItem(IDC_CHECK_GROUPTHREAD))->SetCheck(m_bGroupThread);

	m_bLockUpdates = false;

	UpdateStats();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPropPage_Add_Recovery::OnChangeEditPar2() 
{
	GetDlgItemText(IDC_EDIT_PAR2,m_sPAR2);
}

BOOL CPropPage_Add_Recovery::OnApply() 
{
/*
	if(m_iPartSize % 64 != 0)
	{
		AfxMessageBox("Recovery\r\n\r\nPart Size must be a multiple of 64!", MB_ICONSTOP);
		return(FALSE);
	}
*/
	return CPropertyPage::OnApply();
}

void CPropPage_Add_Recovery::UpdateStats()
{
	int nbfiles=0,size=0;
	int nbc = m_iNbChunks;
	int maxc = (m_iMaxPartsPerFile>0?m_iMaxPartsPerFile:(nbc>0)?nbc:1);
	if (!m_bSameSize)
	{
		int nc = 1;
		while (nbc>=nc && nc<=maxc)
		{
			++nbfiles; nbc-=nc;
			nc*=2;
		}
	}
	nbfiles += (nbc+maxc-1)/maxc;

	size = (m_iPartSize)*m_iNbChunks;

	//char buf[16];
	//GetDlgItem(IDC_TEXT_NBFILES)->SetWindowText(itoa(nbfiles,buf,10));
	//GetDlgItem(IDC_TEXT_RECVSIZE)->SetWindowText(itoa(size,buf,10));

	CString sbuf;
	sbuf.Format("%d",nbfiles); SetDlgItemText(IDC_TEXT_NBFILES,sbuf);
	sbuf.Format("%s",format_size(size)); SetDlgItemText(IDC_TEXT_RECVSIZE,sbuf);


}

void CPropPage_Add_Recovery::OnChangeEditFirst() 
{
	if (m_bLockUpdates) return;
	int i = GetDlgItemInt(IDC_EDIT_FIRST);
	if (i>=0 && i<32767) m_iFirstChunks = i;
}

void CPropPage_Add_Recovery::OnChangeEditBlocks() 
{
	if (m_bLockUpdates) return;
	int i = GetDlgItemInt(IDC_EDIT_BLOCKS);
	if (i>=0 && i<32767)
	{
		m_iNbChunks = i;
		UpdateStats();
	}
}

void CPropPage_Add_Recovery::OnChangeEditFiles() 
{
	if (m_bLockUpdates) return;
	int i = GetDlgItemInt(IDC_EDIT_FILES);
	if (i>=0 && i<32767)
	{
		m_iMaxPartsPerFile = i;
		UpdateStats();
	}
}

void CPropPage_Add_Recovery::OnCheckSamesize() 
{
	if (m_bLockUpdates) return;
	m_bSameSize = ((CButton*) GetDlgItem(IDC_CHECK_SAMESIZE))->GetCheck();
	UpdateStats();	
}
