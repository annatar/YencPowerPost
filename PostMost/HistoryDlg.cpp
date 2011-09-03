// HistoryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "HistoryDlg.h"

#include "PostMostDoc.h"
#include "PostMostView.h"

#include "DeleteHistoryConfirmDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHistoryDlg dialog


CHistoryDlg::CHistoryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHistoryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHistoryDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nEntries = 0;
	m_nDeleted = 0;
	m_pView = (CPostMostView*) NULL;
}


void CHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHistoryDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHistoryDlg, CDialog)
	//{{AFX_MSG_MAP(CHistoryDlg)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_HIST_COMPACT, OnButtonHistCompact)
	ON_BN_CLICKED(IDC_BUTTON_HIST_DEL_SEL, OnButtonHistDelSel)
	ON_BN_CLICKED(IDC_BUTTON_HIST_QUEUE_SEL, OnButtonHistQueueSel)
	ON_NOTIFY(NM_CLICK, IDC_LIST_HISTORY, OnClickListHistory)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_HISTORY, OnDblclkListHistory)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_HISTORY, OnKeydownListHistory)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHistoryDlg message handlers

void CHistoryDlg::OnDestroy() 
{
	CRect rectWindow;
	GetWindowRect(rectWindow);

	CDialog::OnDestroy();
	
	if(!m_rectAnimateFrom.IsRectEmpty())
		::CM_DrawWireRects(&rectWindow, &m_rectAnimateFrom, 16, 15);	
}

BOOL CHistoryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CWaitCursor wc;

	CenterWindow();
	CRect rectWindow;
	GetWindowRect(rectWindow);

	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_HISTORY);

	CString strResourceBundleString;
	strResourceBundleString.LoadString(IDS_HISTORY_ARTICLE_SUBJECT);
	pListCtrl->InsertColumn(0, LPCTSTR(strResourceBundleString), LVCFMT_LEFT, 320);
	strResourceBundleString.LoadString(IDS_HISTORY_POSTED);
	pListCtrl->InsertColumn(1, LPCTSTR(strResourceBundleString), LVCFMT_LEFT, 64);
	strResourceBundleString.LoadString(IDS_HISTORY_DATE_SAVED);
	pListCtrl->InsertColumn(2, LPCTSTR(strResourceBundleString), LVCFMT_LEFT, 112);

	BuildHistoryFileStats(pListCtrl);

	if(!m_rectAnimateFrom.IsRectEmpty())
		::CM_DrawWireRects(&m_rectAnimateFrom, &rectWindow, 16, 15);

	UpdateButtons();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHistoryDlg::UpdateButtons()
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_HISTORY);

	/*
		IDC_BUTTON_HIST_COMPACT		OnButtonHistCompact
		IDC_BUTTON_HIST_DEL_SEL		OnButtonHistDelSel
		IDC_BUTTON_HIST_QUEUE_SEL	OnButtonHistQueueSel
	*/
	if(pListCtrl->GetSelectedCount() > 0)
	{
		GetDlgItem(IDC_BUTTON_HIST_DEL_SEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_HIST_QUEUE_SEL)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_HIST_DEL_SEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_HIST_QUEUE_SEL)->EnableWindow(FALSE);
	}

	if(m_nDeleted > 0)
		GetDlgItem(IDC_BUTTON_HIST_COMPACT)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_BUTTON_HIST_COMPACT)->EnableWindow(FALSE);

	UpdateHistoryInfoLabel();
}

int CHistoryDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
		
	return 0;
}

void CHistoryDlg::BuildHistoryFileStats(CListCtrl* pListCtrl)
{
	m_nEntries = 0;
	m_nDeleted = 0;

	CFile theHistoryFile;
	CFileException cfe;

	if(theHistoryFile.Open("HISTORY.DAT", CFile::modeRead, &cfe) == FALSE)
	{
		TRACE("CHistoryDlg::BuildHistoryFileStats() - File Not Opened\n");
		return;
	}

	TRACE("CHistoryDlg::BuildHistoryFileStats() - Getting Status ...\n");

	CTaskObject oneTask;

	DWORD dwFilePos = 0;
	
	int nPartsTotal;
	TCHAR szParts[32];

	while(oneTask.ReadFromFile(theHistoryFile) == TRUE)
	{
		++m_nEntries;
		if (oneTask.m_nState == CTaskObject::PM_POSTING)
			oneTask.m_nState = CTaskObject::PM_QUEUED;
		else if (oneTask.m_nState == CTaskObject::PM_ENCODING)
			oneTask.m_nState = CTaskObject::PM_ENC_QUEUED;


		TRACE2("  Task %4d at Offset %u\n", m_nEntries, dwFilePos);
		
		if(oneTask.m_bHistoryDeleted)
			++m_nDeleted;
		else
		{
			if(pListCtrl != NULL)
			{
				nPartsTotal = oneTask.getNbParts();
				sprintf(szParts, "%d/%d", nPartsTotal - oneTask.m_dwaPartsToPost.GetSize(), nPartsTotal);
				pListCtrl->InsertItem(0, oneTask.m_szSubject);
				pListCtrl->SetItemText(0, 1, szParts);
				pListCtrl->SetItemText(0, 2, oneTask.m_szHistoryDateTime);
				pListCtrl->SetItemData(0, dwFilePos);
			}
		}

		dwFilePos = theHistoryFile.GetPosition();
	}

	theHistoryFile.Close();
}

void CHistoryDlg::OnButtonHistCompact() 
{
	// Compact History File
	CWaitCursor wc;

	CFile theHistoryFile;
	CFileException cfe;

	if(theHistoryFile.Open("HISTORY.DAT", CFile::modeReadWrite, &cfe) == FALSE)
	{
		TRACE("CHistoryDlg::BuildHistoryFileStats() - File Not Opened\n");
		return;
	}

	CTaskObject oneTask;

	DWORD dwReadPos = 0;
	DWORD dwWritePos = 0;

	while(oneTask.ReadFromFile(theHistoryFile) == TRUE)
	{
		if(oneTask.m_bHistoryDeleted == TRUE)
			continue;

		dwReadPos = theHistoryFile.GetPosition();

		theHistoryFile.Seek(dwWritePos, CFile::begin);

		oneTask.WriteToFile(theHistoryFile, FALSE);

		dwWritePos = theHistoryFile.GetPosition();

		theHistoryFile.Seek(dwReadPos, CFile::begin);
	}

	theHistoryFile.SetLength(dwWritePos);

	theHistoryFile.Close();

	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_HISTORY);

	pListCtrl->DeleteAllItems();

	BuildHistoryFileStats(pListCtrl);

	UpdateButtons();

}

void CHistoryDlg::OnButtonHistDelSel() 
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_HISTORY);

	int nCount = pListCtrl->GetItemCount();
	int nSelected = pListCtrl->GetSelectedCount();

	TCHAR szTT[64];
	sprintf(szTT, "Delete the %d selected items from the history file?", nSelected);

	CDeleteHistoryConfirmDlg Dlg;
	strcpy(Dlg.m_szText, szTT);

	if(Dlg.DoModal() == IDCANCEL)
		return;

	// Delete items from list; start with highest index and work down..
	int k;
	DWORD dwFilePos;

	CFile theHistoryFile;
	CFileException cfe;

	if(theHistoryFile.Open("HISTORY.DAT", CFile::modeWrite, &cfe) == FALSE)
	{
		TRACE("CHistoryDlg::BuildHistoryFileStats() - File Not Opened\n");
		CString strResourceBundleString;
		strResourceBundleString.LoadString(IDS_HISTORY_ERROR_OPENING_FILE);
		AfxMessageBox(LPCTSTR(strResourceBundleString), MB_ICONSTOP);
		return;
	}

	BOOL bDELETED = TRUE;

	try
	{
		for(k = nCount - 1; k >= 0; k--)
		{
			if(pListCtrl->GetItemState(k, LVIS_SELECTED) == LVIS_SELECTED)
			{
				dwFilePos = pListCtrl->GetItemData(k);
				theHistoryFile.Seek(dwFilePos, CFile::begin);
				theHistoryFile.Write(&bDELETED, sizeof(BOOL));
				pListCtrl->DeleteItem(k);
				++m_nDeleted;
			}
		}
	}
	catch(CFileException* e)
	{
		TCHAR szErrorMessage[1024];
		TCHAR szErrorInfo[768];
		e->GetErrorMessage(szErrorInfo, 760);
		sprintf(szErrorMessage, "Error Processing HISTORY.DAT!\r\n\r\n%s", szErrorInfo);
		e->Delete();
		AfxMessageBox(szErrorMessage, MB_ICONEXCLAMATION);
	}

	theHistoryFile.Close();

	UpdateButtons();
	
}

void CHistoryDlg::OnButtonHistQueueSel()
{
	CListCtrl* pListCtrl = (CListCtrl*) GetDlgItem(IDC_LIST_HISTORY);

	int nCount = pListCtrl->GetItemCount();
	int nSelected = pListCtrl->GetSelectedCount();

	TCHAR szTT[64];
	sprintf(szTT, "Queue the %d selected items?", nSelected);

	if(AfxMessageBox(szTT, MB_ICONQUESTION | MB_OKCANCEL) == IDCANCEL)
		return;

	// Delete items from list; start with highest index and work down..
	int k;
	DWORD dwFilePos;

	CFile theHistoryFile;
	CFileException cfe;

	if(theHistoryFile.Open("HISTORY.DAT", CFile::modeRead, &cfe) == FALSE)
	{
		TRACE("CHistoryDlg::BuildHistoryFileStats() - File Not Opened\n");
		CString strResourceBundleString;
		strResourceBundleString.LoadString(IDS_HISTORY_ERROR_OPENING_FILE);
		AfxMessageBox(LPCTSTR(strResourceBundleString), MB_ICONSTOP);
		return;
	}

	CTaskObject TaskObject;

	try
	{
		for(k = 0; k < nCount; k++)
		{
			if(pListCtrl->GetItemState(k, LVIS_SELECTED) == LVIS_SELECTED)
			{
				dwFilePos = pListCtrl->GetItemData(k);
				theHistoryFile.Seek(dwFilePos, CFile::begin);
				TaskObject.ReadFromFile(theHistoryFile);

				// Set m_nState to QUEUE
				TaskObject.m_nState = CTaskObject::PM_QUEUED;

				
				if(TaskObject.m_strTextPrefix.GetLength() > 0)
					TaskObject.m_bPostPrefixText = TRUE;

				if(TaskObject.m_dwaPartsToPost.GetSize() == 0)
				{
					// Set All Parts To Post
					int nPartsRequired = TaskObject.getNbParts();
					TaskObject.m_dwaPartsToPost.SetSize(0, nPartsRequired);
					for(int k = 1; k <= nPartsRequired; k++)
						TaskObject.m_dwaPartsToPost.Add(k);
				}

				m_pView->AppendTaskToQueue(TaskObject);
			}
		}
	}
	catch(CFileException* e)
	{
		TCHAR szErrorMessage[1024];
		TCHAR szErrorInfo[768];
		e->GetErrorMessage(szErrorInfo, 760);
		sprintf(szErrorMessage, "Error Reading From HISTORY.DAT!\r\n\r\n%s", szErrorInfo);
		e->Delete();
		AfxMessageBox(szErrorMessage, MB_ICONEXCLAMATION);
	}

	theHistoryFile.Close();
}

void CHistoryDlg::OnClickListHistory(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateButtons();
	
	*pResult = 0;
}

void CHistoryDlg::OnDblclkListHistory(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateButtons();
	
	*pResult = 0;
}

void CHistoryDlg::OnKeydownListHistory(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	UpdateButtons();
	*pResult = 0;
}

void CHistoryDlg::UpdateHistoryInfoLabel()
{
	TCHAR szLabel[256];

	CString strResourceBundleString;
	strResourceBundleString.LoadString(IDS_ARTICLE_SUBJECT_LINE);

	sprintf(szLabel, LPCTSTR(strResourceBundleString), m_nEntries - m_nDeleted, m_nDeleted, m_nEntries);
	SetDlgItemText(IDC_LABEL_HISTORY_INFO, szLabel);
}
