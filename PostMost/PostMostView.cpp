// PostMostView.cpp : implementation of the CPostMostView class
//

#include "stdafx.h"
#include "PostMost.h"

#include "PostMostDoc.h"
#include "PostMostView.h"

#include "MainFrm.h"

#include "PostingProgressDlg.h"
#include "EncodingProgressDlg.h"

#include "PostMostPropertySheet.h"

#include "PropPage_Server.h"
#include "PropPage_Groups.h"
#include "PropPage_Headers.h"
#include "PropPage_ProgramSettings.h"

#include "TaskProp_FileAndParts.h"
#include "TaskProp_Groups.h"
#include "TaskProp_PrefixText.h"
#include "TaskProp_Log.h"

#include "HistoryDlg.h"
#include "AutoRetryDlg.h"

#include "DeleteTaskDlg.h"
#include "DelayedPostingDlg.h"

#include "PropPage_Add_Subject.h"
#include "PropPage_Add_Groups.h"
#include "PropPage_Add_TextPrefix.h"
#include "PropPage_Add_FileOrder.h"
#include "PropPage_Add_Checksums.h"
#include "PropPage_Add_Recovery.h"

#include "EncoderThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPostMostView

IMPLEMENT_DYNCREATE(CPostMostView, CListView)

BEGIN_MESSAGE_MAP(CPostMostView, CListView)
	//{{AFX_MSG_MAP(CPostMostView)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_POST_THREADS, OnUpdatePostThreads)
	ON_COMMAND(ID_POST_THREADS, OnPostThreads)
	ON_UPDATE_COMMAND_UI(ID_POST_START, OnUpdatePostStart)
	ON_UPDATE_COMMAND_UI(ID_POST_STOP, OnUpdatePostStop)
	ON_COMMAND(ID_POST_STOP, OnPostStop)
	ON_COMMAND(ID_POST_START, OnPostStart)
	ON_UPDATE_COMMAND_UI(ID_POST_SETTINGS, OnUpdatePostSettings)
	ON_COMMAND(ID_POST_SETTINGS, OnPostSettings)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLEARDONE, OnUpdateFileClearDoneFiles)
	ON_COMMAND(ID_FILE_CLEARDONE, OnFileClearDoneFiles)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLEARLIST, OnUpdateFileClearList)
	ON_COMMAND(ID_FILE_CLEARLIST, OnFileClearList)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLEAR_ERRORS, OnUpdateFileClearErrors)
	ON_COMMAND(ID_FILE_CLEAR_ERRORS, OnFileClearErrors)
	ON_UPDATE_COMMAND_UI(ID_FILE_QUEUE_ALL, OnUpdateFileQueueAll)
	ON_COMMAND(ID_FILE_QUEUE_ALL, OnFileQueueAll)
	ON_UPDATE_COMMAND_UI(ID_FILE_ADDFILES, OnUpdateFileAddFiles)
	ON_COMMAND(ID_FILE_ADDFILES, OnFileAddFiles)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLEARSELECTED, OnUpdateFileClearSelected)
	ON_COMMAND(ID_FILE_CLEARSELECTED, OnFileClearSelected)
	ON_UPDATE_COMMAND_UI(ID_FILE_TASK_PROPERTIES, OnUpdateFileTaskProperties)
	ON_COMMAND(ID_FILE_TASK_PROPERTIES, OnFileTaskProperties)
	ON_UPDATE_COMMAND_UI(ID_FILE_HISTORY, OnUpdateFileHistory)
	ON_COMMAND(ID_FILE_HISTORY, OnFileHistory)
	ON_UPDATE_COMMAND_UI(ID_FILE_QUEUE_SEL, OnUpdateFileQueueSel)
	ON_COMMAND(ID_FILE_QUEUE_SEL, OnFileQueueSel)
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_FILE_REASSIGN_NEWSGROUPS, OnUpdateFileReassignNewsgroups)
	ON_COMMAND(ID_FILE_REASSIGN_NEWSGROUPS, OnFileReassignNewsgroups)
	ON_UPDATE_COMMAND_UI(ID_POST_START_AT, OnUpdatePostStartAt)
	ON_COMMAND(ID_POST_START_AT, OnPostStartAt)
	ON_WM_TIMER()
	ON_WM_CHAR()
	ON_UPDATE_COMMAND_UI(ID_MOVE_TASK_BOTTOM, OnUpdateMoveTaskBottom)
	ON_COMMAND(ID_MOVE_TASK_BOTTOM, OnMoveTaskBottom)
	ON_UPDATE_COMMAND_UI(ID_MOVE_TASK_DOWN, OnUpdateMoveTaskDown)
	ON_COMMAND(ID_MOVE_TASK_DOWN, OnMoveTaskDown)
	ON_UPDATE_COMMAND_UI(ID_MOVE_TASK_TOP, OnUpdateMoveTaskTop)
	ON_COMMAND(ID_MOVE_TASK_TOP, OnMoveTaskTop)
	ON_UPDATE_COMMAND_UI(ID_MOVE_TASK_UP, OnUpdateMoveTaskUp)
	ON_COMMAND(ID_MOVE_TASK_UP, OnMoveTaskUp)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	ON_UPDATE_COMMAND_UI(ID_FILE_SET_HOLD, OnUpdateFileSetHold)
	ON_COMMAND(ID_FILE_SET_HOLD, OnFileSetHold)
	ON_UPDATE_COMMAND_UI(ID_GENERATE_PAR2_VOL, OnUpdateGeneratePar2Vol)
	ON_COMMAND(ID_GENERATE_PAR2_VOL, OnGeneratePar2Vol)
	ON_UPDATE_COMMAND_UI(ID_ENCODE_START, OnUpdateEncodeStart)
	ON_UPDATE_COMMAND_UI(ID_ENCODE_STOP, OnUpdateEncodeStop)
	ON_UPDATE_COMMAND_UI(ID_POST_ENCODES, OnUpdatePostEncodes)
	ON_COMMAND(ID_POST_ENCODES, OnPostEncodes)
	ON_COMMAND(ID_ENCODE_START, OnEncodeStart)
	ON_COMMAND(ID_ENCODE_STOP, OnEncodeStop)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_FILES, OnUpdateIndicatorFiles)
	ON_MESSAGE(WM_FILES_DROPPED, OnFilesDropped)
	ON_MESSAGE(WM_THREAD_READY,  OnThread_Ready)
	ON_MESSAGE(WM_THREAD_NO_CONNECTION, OnThread_NoConnection)
	ON_MESSAGE(WM_THREAD_SERVER_NOT_READY, OnThread_ServerNotReady)
	ON_MESSAGE(WM_THREAD_UPDATE_PROGRESS, OnThread_UpdateProgress)
	ON_MESSAGE(WM_ENCTHREAD_READY,  OnEncode_Ready)
	ON_MESSAGE(WM_ENCTHREAD_STOPPED,  OnEncode_Stopped)
	ON_MESSAGE(WM_ENCTHREAD_DONE,  OnEncode_Done)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPostMostView construction/destruction
CLIPFORMAT CPostMostView::m_cfObjectDescriptor = NULL;

extern CPostMostApp theApp;

CPostMostView::CPostMostView()
{
	m_bRunning = FALSE;
	m_bWaiting = FALSE;
	m_nWaitTimerID = 0;
	m_bWaiting2 = FALSE;
	//m_StopPost = FALSE;

	m_pProgressDlg = (CPostingProgressDlg*) NULL;
	m_pEncodeDlg = (CEncodingProgressDlg*) NULL;
	m_pEncodingThread = NULL;

	// Initialize Drag & Drop Support
	m_bInDrag = FALSE;
	m_prevDropEffect = DROPEFFECT_NONE;
	// Drag & Drop Support
	m_cfObjectDescriptor = (CLIPFORMAT) ::RegisterClipboardFormat(_T("Object Descriptor"));

	theApp.m_pView=this;
}

CPostMostView::~CPostMostView()
{
}

BOOL CPostMostView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CListView::PreCreateWindow(cs);
}


void CPostMostView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// Load Program Settings
	m_Settings.LoadFromRegistry();

	if (m_Settings.m_Disclaimer != 101)
	{
		AfxMessageBox("DISCLAIMER:\r\n"
		"  This software is FREE software and should be considered BETA.\r\n"
		"No warranties, expressed or implied, is provided. The author assumes\r\n"
		"no liability for damages of ANY kind which may result from the use of\r\n"
		"this software.\r\n"
		"\r\n"
		"  Also, when complaining about problems, keep in mind that I did not\r\n"
		"force you to use it.                                      -Assert.");
		m_Settings.m_Disclaimer = 101;
	}

	if(m_Settings.m_bSysTrayIcon == TRUE)
		((CMainFrame*) AfxGetApp()->m_pMainWnd)->NotifyIcon(NIM_ADD, IDI_ICON_TRAY_READY);

	// Set image lists for list control
	CListCtrl& listCtrl = GetListCtrl();
	listCtrl.SetImageList(&((CPostMostApp*) AfxGetApp())->m_ImageList, LVSIL_SMALL);

	// Get Window Stule from Registry
	listCtrl.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
	listCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS);

	m_Windows_View_Color_BkColor = listCtrl.GetBkColor();
	m_Windows_View_Color_TextColor = listCtrl.GetTextColor();
	m_Windows_View_Color_TextBkColor = listCtrl.GetTextBkColor();

	SetListControlColors();

	// Create Report columns
	CString strResourceBundleString;
	strResourceBundleString.LoadString(IDS_ARTICLE_SUBJECT_LINE);
	listCtrl.InsertColumn(0, LPCTSTR(strResourceBundleString), LVCFMT_LEFT, m_Settings.m_nViewColWid[0], -1);
	strResourceBundleString.LoadString(IDS_PARTS_TO_POST);
	listCtrl.InsertColumn(1, LPCTSTR(strResourceBundleString), LVCFMT_LEFT, m_Settings.m_nViewColWid[1], -1);
	strResourceBundleString.LoadString(IDS_POSTING_RESULT);
	listCtrl.InsertColumn(2, LPCTSTR(strResourceBundleString), LVCFMT_LEFT, m_Settings.m_nViewColWid[2], -1);
	strResourceBundleString.LoadString(IDS_NEWSGROUPS);
	listCtrl.InsertColumn(3, LPCTSTR(strResourceBundleString), LVCFMT_LEFT, m_Settings.m_nViewColWid[3], -1);

	LoadTasksFromQueue();

	StartWaitTimer();

      theApp.ApplySkin(m_Settings.m_szSkin);

}

void CPostMostView::SetListControlColors(COLORREF color, COLORREF bkcolor)
{
	CListCtrl& listCtrl = GetListCtrl();

	if(m_Settings.m_bDefaultListViewColors == TRUE)
	{
		listCtrl.SetBkColor(m_Windows_View_Color_BkColor);
		listCtrl.SetTextColor(m_Windows_View_Color_TextColor);
		listCtrl.SetTextBkColor(m_Windows_View_Color_TextBkColor);
	}
	else
	{
		listCtrl.SetBkColor(bkcolor);
		listCtrl.SetTextColor(color);
		listCtrl.SetTextBkColor(bkcolor);
	}
}

void CPostMostView::SaveColumnWidthsToSettings()
{
	CListCtrl& listCtrl = GetListCtrl();

	for(int k = 0; k < 4; k++)
	{
		m_Settings.m_nViewColWid[k] = listCtrl.GetColumnWidth(k);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPostMostView diagnostics

#ifdef _DEBUG
void CPostMostView::AssertValid() const
{
	CListView::AssertValid();
}

void CPostMostView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CPostMostDoc* CPostMostView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPostMostDoc)));
	return (CPostMostDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPostMostView message handlers

int CPostMostView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	DragAcceptFiles(TRUE);

	if(!m_DropTarget.Register(this))
		AfxMessageBox("Error Enabling Window As A Drag & Drop Target");
			
	//OnEncodeStart();

	return 0;
}

DROPEFFECT CPostMostView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	ASSERT(m_prevDropEffect == DROPEFFECT_NONE);
	TRACE("OnDragEnter()\n");
	
	m_dragSize = CSize(0,0);
	m_dragOffset = CSize(0,0);
	HGLOBAL hObjDesc = pDataObject->GetGlobalData(m_cfObjectDescriptor);
	if(hObjDesc != NULL)
	{
		LPOBJECTDESCRIPTOR pObjDesc = (LPOBJECTDESCRIPTOR) ::GlobalLock(hObjDesc);
		ASSERT(pObjDesc != NULL);
		m_dragSize.cx = (int) pObjDesc->sizel.cx;
		m_dragSize.cy = (int) pObjDesc->sizel.cy;
		m_dragOffset.cx = (int) pObjDesc->pointl.x;
		m_dragOffset.cy = (int) pObjDesc->pointl.y;
		::GlobalUnlock(hObjDesc);
		::GlobalFree(hObjDesc);
	}
	CClientDC dc(NULL);
	dc.HIMETRICtoDP(&m_dragSize);
	dc.HIMETRICtoDP(&m_dragOffset);
	m_dragPoint = point - CSize(1,1);
	return CListView::OnDragOver(pDataObject, dwKeyState, point);

	// return CListView::OnDragEnter(pDataObject, dwKeyState, point);
}

DROPEFFECT CPostMostView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	// TRACE("OnDragOver()\n");
	DROPEFFECT de = DROPEFFECT_NONE;
	point -= m_dragOffset;
	if(point == m_dragPoint)
		return DROPEFFECT_COPY;
	CClientDC dc(this);
	if(m_prevDropEffect != DROPEFFECT_NONE)
		dc.DrawFocusRect(CRect(m_dragPoint, m_dragSize));
	m_prevDropEffect = DROPEFFECT_COPY;
	if(m_prevDropEffect != DROPEFFECT_NONE)
	{
		m_dragPoint = point;
		dc.DrawFocusRect(CRect(m_dragPoint, m_dragSize));
	}
	return DROPEFFECT_COPY;
	// return CListView::OnDragOver(pDataObject, dwKeyState, point);
}

void CPostMostView::OnDragLeave() 
{
	CClientDC dc(this);
	if(m_prevDropEffect != DROPEFFECT_NONE)
	{
		dc.DrawFocusRect(CRect(m_dragPoint, m_dragSize));
		m_prevDropEffect = DROPEFFECT_NONE;
	}
	TRACE("OnDragLeave()\n");
	// CListView::OnDragLeave();
}

BOOL CPostMostView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	ASSERT_VALID(this);
	OnDragLeave();

	TRACE("OnDragDrop()\n");

	STGMEDIUM StgMedium;
	::ZeroMemory(&StgMedium, sizeof(STGMEDIUM));

	UINT kk;
	TCHAR szFilename[_MAX_PATH + 1];

	m_Dropped_Files.RemoveAll();

	if(pDataObject->GetData(CF_HDROP, &StgMedium) == TRUE)
	{
		TRACE1("GetData(CF_HDROP. &STGMEDIUM) Succeeded - tymed = %d\n", StgMedium.tymed);
		if(StgMedium.tymed == TYMED_HGLOBAL)
		{
			HDROP hDrop = (HDROP) ::GlobalLock(StgMedium.hGlobal);
			if(hDrop != (HDROP) NULL)
			{
				UINT nFiles = DragQueryFile(hDrop, (UINT) -1, NULL, 0);
				TRACE1("There were %d file(s) dropped.\n", nFiles);
				for(kk = 0; kk < nFiles; kk++)
				{
					::ZeroMemory(szFilename, _MAX_PATH + 1);
					DragQueryFile(hDrop, kk, (LPTSTR) szFilename, _MAX_PATH + 1);
					if(m_Settings.m_bSortFilesOnAdd)
						::CM_CSA_INSERT(m_Dropped_Files, szFilename);
					else
						m_Dropped_Files.Add((LPCTSTR) szFilename);
					TRACE1("Filename Dropped: %s\n", szFilename);
				}
				::GlobalUnlock(StgMedium.hGlobal);
				::GlobalFree(StgMedium.hGlobal);
			}
		}
		ReleaseStgMedium(&StgMedium);

		PostMessage(WM_FILES_DROPPED);
	}
	else AfxMessageBox("Invalid Format For Drag & Drop Operation.", MB_ICONEXCLAMATION);

	TRACE("CPostMostView::OnDrop() - RETURNING\n");
	return TRUE;
	// return CListView::OnDrop(pDataObject, dropEffect, point);
}



CString XMLize(const char* text)
{
	CString res;
	while (*text)
	{
		char c=*text;
//		     if (c=='&') res+="&amp;";
//		else if (c=='<') res+="&lt;";
//		else if (c=='>') res+="&gt;";
//		else if (c=='"') res+="&quot;";
		if (c=='"' || c=='&' || c=='<' || c=='>' || (unsigned)c>=(unsigned)128)
		{
			static const char hex[]="0123456789ABCDEF";
			res+="&#x";
			res+=hex[(c>>4)&15];
			res+=hex[c&15];
			res+=';';
		}
		else res+=c;
		++text;
	}
	return res;
}

afx_msg LRESULT CPostMostView::OnFilesDropped(WPARAM wParam, LPARAM lParam)
{
	TRACE("CPostMostView::OnFilesDropped() - |m_Dropped_Files| = %d\n", m_Dropped_Files.GetSize());

	CString strSubjectPrefixTemplate;

	BOOL bInsertFilesAtBeginningOfQueue = FALSE;
	BOOL bProcessDir = TRUE;
	BOOL bProcessDirRecursive = FALSE;
	BOOL bCountDirSeperate = FALSE;
	BOOL bAddPrefixText = FALSE;
	BOOL bAddPrefixTextFirstFileOnly = FALSE;
	BOOL bGroupThread = m_Settings.m_bFileThread;

	AfxGetApp()->m_pMainWnd->SetForegroundWindow();

	int kk;
	int nItem;

	CFileStatus fStatus;
	//CListCtrl& listCtrl = GetListCtrl();
	CString sz_theDirectory, sz_theFile;
	CStringArray listFileNames, listDir;
	int nDirectories = 0;

	if(TRUE)
	{
		CWaitCursor wc;
		// Seperate the files from the directories
		for(kk = 0; kk < m_Dropped_Files.GetSize(); kk++)
		{
			if(CFile::GetStatus((LPCTSTR) m_Dropped_Files.GetAt(kk), fStatus))
			{
				if(fStatus.m_attribute & 0x10)
					listDir.Add(m_Dropped_Files.GetAt(kk));
				else
					listFileNames.Add(m_Dropped_Files.GetAt(kk));
			}
		}

		nDirectories = listDir.GetSize();
	}

	// Done with original list - clear it up..
	m_Dropped_Files.RemoveAll();

	CPropPage_Add_Subject		PP_Subject;
	CPropPage_Add_Groups		PP_Groups;
	CPropPage_Add_TextPrefix	PP_TextPrefix;
	CPropPage_Add_FileOrder     PP_FileOrder;
	CPropPage_Add_Checksums      PP_Checksums;

	strcpy(PP_FileOrder.m_szLastDirectory,m_Settings.m_szLastAddDir);

	DWORD dwPageActivation = 0;
	PP_Subject.m_pdwActivation = &dwPageActivation;
	PP_Groups.m_pdwActivation = &dwPageActivation;
	PP_TextPrefix.m_pdwActivation = &dwPageActivation;
	PP_FileOrder.m_pdwActivation = &dwPageActivation;
	PP_Checksums.m_pdwActivation = &dwPageActivation;

	PP_Subject.m_nDir   = nDirectories;
	PP_Subject.m_pDroppedFiles = &listFileNames;
	PP_Subject.m_bInsertFilesAtBeginningOfQueue = bInsertFilesAtBeginningOfQueue;
	PP_Subject.m_bProcessSubdirectories = bProcessDir;
	PP_Subject.m_bRecursive = bProcessDirRecursive;
	PP_Subject.m_bCountDirSeperate = bCountDirSeperate;
	PP_Subject.m_strSubjectPrefixTemplate = (LPCTSTR) m_Settings.m_szLastSubjectTemplate;
	PP_Subject.m_last_strSubjectPrefixTemplate=PP_Subject.m_strSubjectPrefixTemplate;

	PP_Groups.m_pSettings = &m_Settings;

	PP_FileOrder.m_pDWA_FileNames = &listFileNames;

	PP_Checksums.m_pDWA_FileNames = &listFileNames;
	PP_Checksums.m_bGroupThread = bGroupThread;
	PP_Checksums.m_iPartSize = m_Settings.m_nMaxLines*YENCLEN;

	TCHAR szPSTitle[64];
	if(listDir.GetSize() > 0)
	{
		if(listFileNames.GetSize() > 0)
			sprintf(szPSTitle, "Add %d File%s %d Directories", 
			listFileNames.GetSize(), 
			(listFileNames.GetSize() == 1) ? "," : "s,",
			listDir.GetSize());
		else
			sprintf(szPSTitle, "Add %d Director%s", 
			listDir.GetSize(),
			(listDir.GetSize() == 1) ? "y" : "ies" );
	}
	else
	{
		if(listFileNames.GetSize() > 0)
			sprintf(szPSTitle, "Add %d File%s", listFileNames.GetSize(),
			(listFileNames.GetSize() == 1) ? "" : "s");
		else
		{
			AfxMessageBox("No Files To Add!", MB_ICONINFORMATION);
			return(0);
		}
	}

	CPropertySheet PS;
	PS.SetTitle(szPSTitle);
	PS.AddPage(&PP_Subject);
	PS.AddPage(&PP_Groups);
	PS.AddPage(&PP_TextPrefix);
	
	if((listFileNames.GetSize() > 0) && (listDir.GetSize() == 0))
		PS.AddPage(&PP_FileOrder);

	PS.AddPage(&PP_Checksums);

	PS.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	// Setup Example with something real if we can...
	if(listFileNames.GetSize() > 1)
	{
		// strcpy(PP_Subject.m_Ex_szFilename, (LPCTSTR) listFileNames.GetAt(0));
		PP_Subject.m_Ex_nFileCount = listFileNames.GetSize();
	}

	if(PS.DoModal() == IDCANCEL)
	{
		return(0);
	}

	strcpy(m_Settings.m_szLastSubjectTemplate, (LPCTSTR) PP_Subject.m_strSubjectPrefixTemplate);

	strSubjectPrefixTemplate = (LPCTSTR) PP_Subject.m_strSubjectPrefixTemplate;
	bInsertFilesAtBeginningOfQueue = PP_Subject.m_bInsertFilesAtBeginningOfQueue;
	bProcessDir = PP_Subject.m_bProcessSubdirectories;
	bProcessDirRecursive = PP_Subject.m_bRecursive;
	bCountDirSeperate = PP_Subject.m_bCountDirSeperate;

	LPCTSTR lpszTextPrefix = (LPCTSTR) PP_TextPrefix.m_strTextPrefix;
	bAddPrefixText = PP_TextPrefix.m_bSendTextPrefix;
	bAddPrefixTextFirstFileOnly = PP_TextPrefix.m_bSendTextFirstFileOnly;
	LPCTSTR lpszGroupList = (LPCTSTR) PP_Groups.m_strGroupList;
	bGroupThread = PP_Checksums.m_bGroupThread;

	// Process Directories
	int nInsertPoint;
	char szCurFilename[_MAX_PATH+1];
	char szLastDir[_MAX_PATH+1];
	char szCurDir[_MAX_PATH+1];
	char szCurFile[_MAX_PATH+1];

	const char* par2name = NULL;
	int par2pos = 0;
	int kAdded = 0;
	//if(TRUE)
	{
		CWaitCursor wc;

		strcpy(m_Settings.m_szLastAddDir,PP_FileOrder.m_szLastDirectory);

		if(bProcessDir)
		{
			for(kk = 0; kk < listDir.GetSize(); kk++)
			{	
				GetDirectoryFiles(listFileNames, (LPCTSTR) listDir.GetAt(kk), bProcessDirRecursive);
			}
		}

		CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;

		int recover_flags=0;

		if (PP_Checksums.m_sSFV.GetLength()>0) recover_flags|=par2::PostFiles::CALC_CRC;
		if (PP_Checksums.m_sPAR2.GetLength()>0) recover_flags|=par2::PostFiles::CALC_CRC|par2::PostFiles::CALC_HASH|par2::PostFiles::CALC_PART;

		if (recover_flags!=0)
		{ // generating Checksums...
			par2::PostFiles postfiles("",listFileNames.GetSize(),PP_Checksums.m_iPartSize);
			CString progress;
			for(kk = 0; kk < listFileNames.GetSize(); kk++)
			{
				CString fname = listFileNames.GetAt(kk);
				postfiles.initFile(kk,fname,1);
				progress.Format("Computing Checksum for file %s...",fname);
				pMainFrame->SetStatusText(progress);
				postfiles.calcFile(kk,recover_flags);
			}
			if (PP_Checksums.m_sPAR2.GetLength()>0)
			{
				progress.Format("Generating PAR2 file %s...",PP_Checksums.m_sPAR2);
				pMainFrame->SetStatusText(progress);
				par2name = postfiles.generatePAR2(PP_Checksums.m_sPAR2);
				if (par2name!=NULL && par2name[0]!='\0')
				{
					listFileNames.InsertAt(0,par2name);
					progress.Format("PAR2 file %s generated.",par2name);
					par2pos = 0;
				}
				else
				{
					const char* error=strerror(errno);
					progress.Format("Error on file %s:%s",PP_Checksums.m_sPAR2,error);
					AfxMessageBox(progress,MB_OK|MB_ICONERROR);
					par2name = NULL;
				}
				pMainFrame->SetStatusText(progress);
			}
			if (PP_Checksums.m_sSFV.GetLength()>0)
			{
				progress.Format("Generating SFV file %s...",PP_Checksums.m_sSFV);
				pMainFrame->SetStatusText(progress);
				const char* sfvname = postfiles.generateSFV(PP_Checksums.m_sSFV);
				if (sfvname!=NULL && sfvname[0]!='\0')
				{
					listFileNames.InsertAt(0,sfvname); ++par2pos;
					progress.Format("SFV file %s generated.",sfvname);
				}
				else
				{
					const char* error=strerror(errno);
					progress.Format("Error on file %s:%s",PP_Checksums.m_sSFV,error);
					AfxMessageBox(progress,MB_OK|MB_ICONERROR);
				}
				pMainFrame->SetStatusText(progress);
			}
		}

		//if (PP_Checksums.m_sNZB.GetLength()>0) 
		//{
		//		listFileNames.InsertAt(0,PP_Checksums.m_sNZB); ++par2pos;
		//}	


		nItem = listNbTasks(); //listCtrl.GetItemCount();

		CTaskObject* parentObj=NULL;
		CTaskObject* currentObj;

		// Now Process The Files...
		if(!bAddPrefixText) lpszTextPrefix=NULL;

		if(bInsertFilesAtBeginningOfQueue)
			nInsertPoint = 0;
		else
			nInsertPoint = nItem;

		if(bCountDirSeperate == TRUE)
		{
			// Number each file by directory
			CStringArray tempList;

			while(listFileNames.GetSize() > 0)
			{
				// Move first item and set the current directory
				tempList.RemoveAll();
				strcpy(szCurFilename, (LPCTSTR) listFileNames.GetAt(0));
				_BreakFilename((LPCTSTR) listFileNames.GetAt(0), szCurDir, szCurFile);
				strcpy(szLastDir, szCurDir);
				listFileNames.RemoveAt(0);
				tempList.Add(szCurFilename);
				while(listFileNames.GetSize() > 0)
				{
					strcpy(szCurFilename, (LPCTSTR) listFileNames.GetAt(0));
					_BreakFilename((LPCTSTR) listFileNames.GetAt(0), szCurDir, szCurFile);
					if(strcmp(szCurDir, szLastDir) == 0)
					{
						listFileNames.RemoveAt(0);
						tempList.Add(szCurFilename);
					}
					else break;
				}
				// Add Items in tempList
				for(kk = 0; kk < tempList.GetSize(); kk++)
				{	

					currentObj = InsertFileIntoQueue(
						(LPCTSTR) tempList.GetAt(kk),
						(LPCTSTR) strSubjectPrefixTemplate,
						nInsertPoint+kAdded,
						kk+1,
						tempList.GetSize(),
						(((!bAddPrefixTextFirstFileOnly)||(kAdded==0))?lpszTextPrefix:NULL),
						lpszGroupList
						);

					++kAdded;
					if (parentObj!=NULL)
					{
						int parentParts = parentObj->getTotalParts();
						parentObj->GenerateMessageID(m_Settings.m_szDomainID);
						currentObj->setReferences(parentObj->getReferences(TRUE,parentParts));
					}
					else if (bGroupThread)
						parentObj = currentObj;
				}
			}
		}
		else
		{
			// Number them all as one batch
			for(kk = 0; kk < listFileNames.GetSize(); kk++)
			{	
				// Determine point in list to insert new item

				currentObj = InsertFileIntoQueue(
					(LPCTSTR) listFileNames.GetAt(kk),
					(LPCTSTR) strSubjectPrefixTemplate,
					nInsertPoint+kk,
					kk+1,
					listFileNames.GetSize(),
					(((!bAddPrefixTextFirstFileOnly)||(kk==0))?lpszTextPrefix:NULL),
					lpszGroupList
					);
				++kAdded;
				if (parentObj!=NULL)
				{
					int parentParts = parentObj->getTotalParts();
					parentObj->GenerateMessageID(m_Settings.m_szDomainID);
					currentObj->setReferences(parentObj->getReferences(TRUE,parentParts));
				}
				else if (bGroupThread)
					parentObj = currentObj;
			}
		}


		// Do we want to generate NZB
		if (PP_Checksums.m_sNZB.GetLength()>0) 
		{
			FILE* fout = fopen(PP_Checksums.m_sNZB,"w");
			if (fout!=NULL)
			{
				fprintf(fout,
"<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n"
"<!DOCTYPE nzb PUBLIC \"-//newzBin//DTD NZB 0.9//EN\" \"http://www.newzbin.com/DTD/nzb/nzb-0.9.dtd\">\n"
"<nzb xmlns=\"http://www.newzbin.com/DTD/2003/nzb\">\n"
				);

				time_t now = time(NULL);
				TCHAR szArticleFrom[256];

				CString groups = XMLize(lpszGroupList);
				groups.Replace(",","</group>\n      <group>");
				groups = "    <groups>\n      <group>"+groups+"</group>\n    </groups>\n";

				// Build szArticleFrom string from the From / FromNick settings
				if(strlen(m_Settings.m_szFromNick) > 0)
					sprintf(szArticleFrom, "%s (%s)", m_Settings.m_szFrom, m_Settings.m_szFromNick);
				else
					strcpy(szArticleFrom, m_Settings.m_szFrom);
				{
					char *c=szArticleFrom;
					while (*c)
					{
						if (*c=='\"') *c='\'';
						++c;
					}
				}
				char szArticle[1024];

				for (kk=0;kk<kAdded;kk++)
				{
					currentObj = listGetTask(nInsertPoint+kk);
					int nbp = currentObj->getNbParts();
					CM_BuildArticleName(szArticle, currentObj->m_szSubject, currentObj->m_szFilename, 1, nbp);
					CString xmlArticleFrom = XMLize(szArticleFrom);
					CString xmlArticle = XMLize(szArticle);
					fprintf(fout,"  <file poster=\"%s\" date=\"%u\" subject=\"%s\">\n",
						(const char*)xmlArticleFrom,(int)now,(const char*)xmlArticle);
					fprintf(fout,"%s",(const char*)groups);
					fprintf(fout,"    <segments>\n");
					for (int p=0;p<nbp;p++)
					{
						CString mid = currentObj->GenerateMessageID(p,nbp,m_Settings.m_szDomainID);
						mid=mid.Mid(1,mid.GetLength()-2);
						mid = XMLize(mid);
						fprintf(fout,"      <segment bytes=\"%I64u\" number=\"%d\">%s</segment>\n",
							(p==nbp-1?currentObj->m_ullFileSize-p*currentObj->m_nPartSize:currentObj->m_nPartSize),
							p+1,(const char*)mid);
					}
					fprintf(fout,"    </segments>\n");
					fprintf(fout,"  </file>\n");
				}
				fprintf(fout,"</nzb>\n");
				fclose(fout);

				currentObj = InsertFileIntoQueue(
					PP_Checksums.m_sNZB,
					(LPCTSTR) strSubjectPrefixTemplate,
					nInsertPoint,
					0,
					listFileNames.GetSize(),
					NULL,
					lpszGroupList
					);
				++nInsertPoint;
			}
		}
		// Do we want to generate par2 volumes?

		if (par2name!=NULL)
		{
			par2::PostFiles *post = new par2::PostFiles(par2name);
			int nVolFiles = 
				GeneratePar2Vol(post,(LPCSTR)strSubjectPrefixTemplate, lpszGroupList, kAdded, listGetTask(nInsertPoint+par2pos));
			if (nVolFiles>0 && !bCountDirSeperate)
			{ // change the subject lines
				TCHAR sz_theDirectory[_MAX_PATH];
				TCHAR sz_theFile[_MAX_PATH];
				for (kk=0;kk<kAdded;kk++)
				{
					currentObj = listGetTask(nInsertPoint+kk);

					// Get base filename of current file
					_BreakFilename(currentObj->m_szFilename, sz_theDirectory, sz_theFile);

					::CM_BuildSubjectFromTemplate((LPCSTR)strSubjectPrefixTemplate, sz_theDirectory, sz_theFile, currentObj->m_ullFileSize, kk+1, nVolFiles+kAdded, currentObj->m_szSubject);

					UpdateDisplay(nInsertPoint+kk,currentObj,NULL);
				}
			}
		}
	}

	// Done
	listFileNames.RemoveAll();
	listDir.RemoveAll();

	// Compute entire batch size
	m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());

	return((LRESULT) 0);
}

CTaskObject* CPostMostView::InsertFileIntoQueue(LPCTSTR szFilename, LPCTSTR szSubjectPrefixTemplate, int nInsertAt, int nCountValue, int nTotalCount, LPCTSTR szTextPrefix, LPCTSTR szGroupList, int state, ULONGLONG filesize, int partsize)
{
	char szFileSubject[1024];

	TCHAR sz_theDirectory[_MAX_PATH];
	TCHAR sz_theFile[_MAX_PATH];

	// Get base filename of current file
	_BreakFilename(szFilename, sz_theDirectory, sz_theFile);

	if (filesize==0)
	{
		CFileStatus rFileStatus;
		CFile::GetStatus(szFilename, rFileStatus);
		filesize = rFileStatus.m_size;
	}

	TRACE2("Adding '%s' -- Size = %I64u\n", szFilename, filesize);

	::CM_BuildSubjectFromTemplate(szSubjectPrefixTemplate, sz_theDirectory, sz_theFile, filesize, nCountValue, nTotalCount, szFileSubject);

	CTaskObject* pTaskObject = new CTaskObject;

	pTaskObject->Reset();
	
	pTaskObject->m_nState = state;

	pTaskObject->m_strGroups = szGroupList;

	if(szTextPrefix != NULL)
		pTaskObject->m_strTextPrefix = szTextPrefix;

	if(pTaskObject->m_strTextPrefix.GetLength() > 0)
		pTaskObject->m_bPostPrefixText = TRUE;
	else
		pTaskObject->m_bPostPrefixText = FALSE;

	strcpy(pTaskObject->m_szFilename, szFilename);

	strcpy(pTaskObject->m_szSubject, szFileSubject);

	pTaskObject->m_nPartSize = m_Settings.m_nMaxLines*YENCLEN;

	pTaskObject->m_ullFileSize = filesize;
	char* filetype = "";
	CString message;
	if (state == CTaskObject::PM_QUEUED && partsize==0 && m_Settings.m_bDetectPAR2)
	{
		// try to detect par2 volume files
		if (pTaskObject->getNbParts()>1)
		{
			CFile f;
			if (f.Open(szFilename,CFile::modeRead|CFile::shareDenyNone))
			{
				char buf[16];
				f.Read(buf,16);
				if (!memcmp(buf,"PAR2\0PKT",8))
				{ // found a par2 packet!
					int psize = (int)*(unsigned __int64 *)(buf+8);

					f.Seek(32,CFile::current);
					f.Read(buf,16);
					if (!memcmp(buf,"PAR 2.0\0RecvSlic",16))
					{ // found a par2 packet!
						partsize = psize;
						filetype = "par2 ";
					}
				}
				f.Close();
			}
		}
	}
	if (partsize>0)
	{
		int maxdiff = pTaskObject->m_nPartSize/512;
		int newsize = 0;
		if (partsize == pTaskObject->m_nPartSize) newsize=pTaskObject->m_nPartSize;
		if (partsize<pTaskObject->m_nPartSize*3/4)
		{
			int diff = pTaskObject->m_nPartSize % partsize;
			if (diff<=partsize/2) newsize = pTaskObject->m_nPartSize-diff;
			else                newsize = pTaskObject->m_nPartSize+(partsize-diff);
		}
		else
		{
			int nsplit = (partsize+(pTaskObject->m_nPartSize/2))/pTaskObject->m_nPartSize;
			if (partsize%nsplit==0) newsize = partsize/nsplit;
		}
		if (newsize==0 || abs(pTaskObject->m_nPartSize-newsize)>maxdiff)
			message.Format("WARNING: %spacket size %d incompatible with lines/part",filetype,partsize);
		else if (newsize==pTaskObject->m_nPartSize)
			message.Format("%spacket size %d in sync with lines/part :-)",filetype,partsize);
		else
		{
			pTaskObject->m_nPartSize = newsize;
			message.Format("line/part CHANGED to %d to sync with %spacket size %d",pTaskObject->getMaxLines(),filetype,partsize);
		}
	}
	int nParts = pTaskObject->getNbParts();

	pTaskObject->m_dwaPartsToPost.SetSize(0, nParts);

	for(int k = 1; k <= nParts; k++)
	{
		pTaskObject->m_dwaPartsToPost.Add(k);
	}

	TCHAR szPartCount[32];
	sprintf(szPartCount, "%d/%d", pTaskObject->m_dwaPartsToPost.GetSize(), nParts);
	if(pTaskObject->m_bPostPrefixText == TRUE)
		strcat(szPartCount, "+0");

	CListCtrl& listCtrl = GetListCtrl();
	listCtrl.InsertItem(nInsertAt, szFileSubject, TaskState2Image(pTaskObject->m_nState));			// Subject Lines
	listCtrl.SetItemData(nInsertAt, (DWORD) pTaskObject);		// Pointer To Task Object
	listCtrl.SetItemText(nInsertAt, 1, szPartCount);			// Computed Number Of Parts
	listCtrl.SetItemText(nInsertAt, 3, (LPCTSTR) pTaskObject->m_strGroups);

	if (message.GetLength()>0)
	{
		strncpy(pTaskObject->m_szLastResult,message,sizeof(pTaskObject->m_szLastResult));
		listCtrl.SetItemText(nInsertAt, 2, (LPCTSTR) message);			// Result
	}

	m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());

	return pTaskObject;
}

void CPostMostView::GetDirectoryFiles(CStringArray& FileList, LPCTSTR szDirectory, BOOL bRecursive)
{
	char szPath[_MAX_PATH + 1];

	CString szFileFound;
	CFileFind finder;
	CFileStatus fStatus;
	BOOL      bFound;

	sprintf(szPath, szDirectory);
	strcat(szPath, "\\*.*");

	TRACE1("Processing Directory: '%s'\n", szDirectory);

	// (1) Process Files
	bFound = finder.FindFile(szPath);
	while(bFound)
	{
		bFound = finder.FindNextFile();
		szFileFound = finder.GetFilePath();
		try
		{
			if(CFile::GetStatus((LPCTSTR) szFileFound, fStatus))
			{
				if((fStatus.m_attribute & 0x10) == 0x00)
				{
					FileList.Add(szFileFound);
				}
			}
		}
		catch (CException* e)
		{
			e->Delete();
		}
	}

	// (2) Process Subdirectories
	if(bRecursive)
	{
		bFound = finder.FindFile(szPath);
		while(bFound)
		{
			bFound = finder.FindNextFile();
			szFileFound = finder.GetFileName();
			if(szFileFound.Compare(".") == 0) continue;
			if(szFileFound.Compare("..") == 0) continue;
			szFileFound = finder.GetFilePath();
			if(CFile::GetStatus((LPCTSTR) szFileFound, fStatus))
			{
				if(fStatus.m_attribute & 0x10)
					GetDirectoryFiles(FileList, szFileFound, bRecursive);
			}
		}
	}
}


void CPostMostView::_BreakFilename(LPCTSTR FullFilename, CString& Path, CString& File)
{
	int kk;
	int sz = strlen(FullFilename);

	Path.Empty();
	File.Empty();

	for(kk = (sz-1); kk >= 0; kk--)
		if(FullFilename[kk] == '\\') break;
	if(kk < 0)
	{
		File = FullFilename;
		return;
	}
	
	CString FullName = FullFilename;
	if(kk > 0)		Path = FullName.Left(kk+1);
	if(kk < (sz-1)) File = FullName.Right((sz-1) - kk);
	return;
}

void CPostMostView::_BreakFilename(LPCTSTR FullFilename, char* szDir, char* szFile)
{
	int sl;
	strcpy(szDir, FullFilename);
	sl = strlen(szDir) - 1;
	while((sl > 0) && (szDir[sl] != '\\')) --sl;
	szDir[sl] = '\0';
	strcpy(szFile, &szDir[sl+1]);
}

void CPostMostView::OnUpdatePostThreads(CCmdUI* pCmdUI) 
{
	if(m_bWaiting == TRUE)
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(0);
		return;
	}

	pCmdUI->Enable(m_bRunning);
	if(m_bRunning)
	{
		if(m_pProgressDlg != (CPostingProgressDlg*) NULL)
			pCmdUI->SetCheck(1);
		else
			pCmdUI->SetCheck(0);
	}
	else pCmdUI->SetCheck(0);

}

void CPostMostView::OnPostThreads() 
{
	RECT rectProgressButton;
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pMainFrame->GetButtonRect(ID_POST_THREADS,&rectProgressButton);

	if(m_pProgressDlg == (CPostingProgressDlg*) NULL)
	{
		m_pProgressDlg = new CPostingProgressDlg(this);
		m_pProgressDlg->m_rectAnimateFrom = rectProgressButton;
		m_pProgressDlg->m_pView = this;
		if(!m_pProgressDlg->Create(IDD_DIALOG_PROGRESS, this))
		{
			delete m_pProgressDlg;
			AfxMessageBox("Error Creating Progress Window!", MB_ICONSTOP);
			return;
		}
		m_pProgressDlg->SetActiveWindow();
	}
	else
	{
	}
}

void CPostMostView::OnUpdatePostStart(CCmdUI* pCmdUI) 
{
	if(m_bRunning)
		pCmdUI->Enable(FALSE);
	else
	{
		if(listNbTasks()==0)
			pCmdUI->Enable(FALSE);
		else
		{
			pCmdUI->Enable(__FilesInQueue_Q());
		}
	}
	pCmdUI->SetCheck(m_bRunning);
}

void CPostMostView::OnUpdatePostStop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bRunning);	
}

void CPostMostView::OnPostStop() 
{
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if(m_bWaiting == TRUE)
	{
		m_bWaiting = FALSE;
		m_bRunning = FALSE;

		// Update Tray Icon
		((CMainFrame*) AfxGetApp()->m_pMainWnd)->NotifyIcon(NIM_MODIFY, IDI_ICON_TRAY_READY);

		pMainFrame->SetProgressIndicator("");
		return;
	}

	// Update Tray Icon
	pMainFrame->NotifyIcon(NIM_MODIFY, IDI_ICON_TRAY_READY);

	if(m_bRunning == FALSE)
	{
		if(m_bWaiting2 == FALSE)
		if(AfxMessageBox("Posting In Progress!\n\nStop Running?", MB_ICONEXCLAMATION | MB_OKCANCEL) == IDCANCEL)
			return;
	}

	CWaitCursor wc;
	if(m_pProgressDlg != NULL)
	{
		m_pProgressDlg->SendMessage(WM_CLOSE);
		delete m_pProgressDlg;
		m_pProgressDlg = NULL;
	}

	if(!m_ThreadMarshal.Stop_AllThread())
	{
		AfxMessageBox("Error Stopping Poster Thread!", MB_ICONSTOP);
		return;
	}

	// Turn any 'P' images into 'Q' images..
	int kk;
	int nItems = listNbTasks();
	CTaskObject* pTaskObject;

	// If we just posted one, set the image/state back to QUEUE
	for(kk = 0; kk < nItems; kk++)
	{		
		pTaskObject=listGetTask(kk);

		if(pTaskObject->m_nState==CTaskObject::PM_POSTING)
		{
			pTaskObject->m_nState = CTaskObject::PM_QUEUED;
			UpdateDisplay(kk,pTaskObject,"");
			//break;
		}
	}
	m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());

	pMainFrame->SetProgressIndicator("");

	m_bRunning = FALSE;
	
	SaveTasksToQueue();
}

void CPostMostView::OnPostStart()
{
	CWaitCursor wc;

	//CListCtrl& list = GetListCtrl();

	if(listNbTasks() == 0)
	{
		AfxMessageBox("No Files Queued For Posting!", MB_ICONSTOP);
		return;
	}

	m_ThreadMarshal.SetProgramSettings(m_Settings);

	// Compute entire batch size
	m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());

	if(!m_ThreadMarshal.Start_AllThread(m_hWnd))
	{
		AfxMessageBox("Error Starting Poster Thread!", MB_ICONSTOP);
		return;
	}

	m_bRunning = TRUE;
	
	// Update Tray Icon
	((CMainFrame*) AfxGetApp()->m_pMainWnd)->NotifyIcon(NIM_MODIFY, IDI_ICON_TRAY_POSTING);

	m_Time_Started = CTime::GetCurrentTime();

	if(m_Settings.m_bShowProgressDialogOnStart && (m_pProgressDlg == NULL))
		OnPostThreads();

}

void CPostMostView::OnUpdatePostSettings(CCmdUI* pCmdUI)
{
	if(m_bRunning)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CPostMostView::OnPostSettings() 
{
	RECT rectButton;
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pMainFrame->GetButtonRect(ID_POST_SETTINGS,&rectButton);

	CProgramSettings CurrentProgramSettings;
	CurrentProgramSettings.Copy(m_Settings);

	CPostMostPropertySheet PS("Properties for " TITLE);
	PS.m_rectAnimateFrom = rectButton;

	CPropPage_Server  PP_Server;	
		PP_Server.m_pSettings = &CurrentProgramSettings;
		PP_Server.m_pSettings_ApplyTo = &m_Settings;

	CPropPage_Headers PP_Headers;	
		PP_Headers.m_pSettings = &CurrentProgramSettings;
		PP_Headers.m_pSettings_ApplyTo = &m_Settings;

	CPropPage_Groups  PP_Groups;	
		PP_Groups.m_pSettings = &CurrentProgramSettings;
		PP_Groups.m_pSettings_ApplyTo = &m_Settings;

	CPropPage_ProgramSettings PP_Program;
		PP_Program.m_pSettings = &CurrentProgramSettings;
		PP_Program.m_pSettings_ApplyTo = &m_Settings;

	PS.AddPage(&PP_Server);
	PS.AddPage(&PP_Headers);
	PS.AddPage(&PP_Groups);
	PS.AddPage(&PP_Program);

	PS.DoModal();

	m_Settings.SaveToRegistry();

}

LRESULT CPostMostView::OnThread_UpdateProgress(WPARAM wParam, LPARAM lParam)
{
	int iThread = wParam/2; wParam&=1;
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;

	TCHAR szArticle[320];
	int nPart, nPartOf;

	m_ThreadMarshal.GetCurrentArticleStatus(iThread, szArticle, &nPart, &nPartOf);

	TCHAR szText[64];
	
	CM_BuildArticleName(szText, "Posting Part", "", nPart, nPartOf);

	if(m_bWaiting2==FALSE)
	pMainFrame->SetProgressIndicator(szText);

	if(nPart==1 && nPartOf != 1)
	SaveTasksToQueue();

	return(0);
}

LRESULT CPostMostView::OnThread_NoConnection(WPARAM wParam, LPARAM lParam)
{
	int iThread = wParam/2; wParam&=1;
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CTaskObject* pTaskObject = (CTaskObject*) lParam;
	pMainFrame->SetProgressIndicator("");

	BOOL bResult_OK = FALSE;
	char szResult[1024];

	sprintf(szResult, "Error Connecting To Server");
	m_ThreadMarshal.GetResult(iThread, bResult_OK, szResult);

	if(m_Settings.m_bAutoRetry)
	{
		pMainFrame->NotifyIcon(NIM_MODIFY, IDI_ICON_TRAY_ERROR);
		CAutoRetryDlg D;
		strcpy(D.m_szMessage, szResult);
		int result = D.DoModal();
		if(result == IDCANCEL)
		{
			PostStopThread(iThread,(CTaskObject*)lParam,wParam);
		}
		else
		{
			pMainFrame->NotifyIcon(NIM_MODIFY, IDI_ICON_TRAY_POSTING);
			PostEnded(iThread,pTaskObject,wParam);
			if (pTaskObject!=NULL && result != IDSKIP)
			{
				pTaskObject->m_nState=CTaskObject::PM_QUEUED;
				UpdateDisplay(pTaskObject,NULL);
			}
			m_ThreadMarshal.Reconnect(iThread);
		}
	}
	else
	{
		PostStopThread(iThread,(CTaskObject*)lParam,wParam);
		/////AfxMessageBox("Error Connecting To Server", MB_ICONSTOP);
	}
	m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());

	return(0);
}

LRESULT CPostMostView::OnThread_ServerNotReady(WPARAM wParam, LPARAM lParam)
{
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pMainFrame->SetProgressIndicator("");
	int iThread = wParam/2; wParam&=1;

	TCHAR szMessage[512];
	sprintf(szMessage, "Server Not Ready!\r\n\r\n%s", m_ThreadMarshal.GetServerConnectString(iThread));
	// AfxMessageBox(szMessage, MB_ICONSTOP);

	if(m_Settings.m_bAutoRetry)
	{
		CAutoRetryDlg RetryDlg;
		strcpy(RetryDlg.m_szMessage, szMessage);

		if(RetryDlg.DoModal() == IDCANCEL)
		{
			PostStopThread(iThread,(CTaskObject*)lParam,wParam);
		}
		else
		{
			m_ThreadMarshal.Reconnect(iThread);
		}
	}
	else
	{
		PostStopThread(iThread,(CTaskObject*)lParam,wParam);
	}

	return(0);
}

void CPostMostView::PostEnded(int iThread, CTaskObject* pTaskObject, BOOL bSTOP)
{
	TRACE("CPostMostView::PostEnded()\n");
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;

	pMainFrame->SetProgressIndicator("");

	if (pTaskObject == NULL) return;

	BOOL bResult_OK = FALSE;
	char szResult[1024];

	m_ThreadMarshal.GetResult(iThread, bResult_OK, szResult);

	if(pTaskObject->m_nState==CTaskObject::PM_POSTING)
	{
		if (bResult_OK)
		{
			pTaskObject->m_nState = CTaskObject::PM_POSTED;
		}
		else
		{
			pTaskObject->m_nState = CTaskObject::PM_ERROR;
		}

		//CheckStillPosting();
	}
	UpdateDisplay(pTaskObject, szResult);
	m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());

}

void CPostMostView::PostStopThread(int iThread, CTaskObject* pTaskObject, BOOL bSTOP)
{
	TRACE("CPostMostView::PostStopThread()\n");
	PostEnded(iThread, pTaskObject, bSTOP);
	CheckStillPosting();
}

LRESULT CPostMostView::OnThread_Ready(WPARAM wParam, LPARAM lParam)
{
	TRACE("CPostMostView::OnThread_Ready()\n");
	int iThread = wParam/2; wParam&=1;
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CTaskObject* pTaskObject = (CTaskObject*) lParam;

	pMainFrame->SetProgressIndicator("");

	PostEnded(iThread, pTaskObject, wParam);

	// If task was m_STOP'd, then we're done with it..
	if(wParam != 0)
		return(0);

	if(!m_bRunning) return(0);

	// Assign Next Task
	//CListCtrl& theList = GetListCtrl();
	int nItems = listNbTasks();
	int kk;

	// Get New Item To Assign
	//int nposting = 0;
	for(kk = 0; kk < nItems; kk++)
	{
		pTaskObject = listGetTask(kk);

		if(pTaskObject->m_nState == CTaskObject::PM_QUEUED)
		{
			pTaskObject->m_nState=CTaskObject::PM_POSTING;
			char buff[128];
			sprintf(buff,"Upload in thread %d",iThread+1);
			UpdateDisplay(kk,pTaskObject,buff);

			m_ThreadMarshal.Assign_Task(iThread, pTaskObject);

			if(m_Settings.m_bHighlightCurrentFile)
				GetListCtrl().EnsureVisible(kk, FALSE);
			return(0);
		}
		//else if (pTaskObject->m_nState == PM_POSTING)
		  //++nposting;
	}
	m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());

	// Nothing Left To Assign
	m_ThreadMarshal.Assign_Task(iThread, NULL);

	CheckStillPosting();
	/*
	if (nposting==0)
	{ // no more thread posting
	  m_bPostStopNoSound = TRUE; 
	  OnPostStop();
	  m_bPostStopNoSound = FALSE;
	  ::Sleep(960);
	  if(m_Settings.m_bSound) PlaySound((LPCTSTR) IDR_WAVE_XFER_COMPLETE, NULL, SND_ASYNC | SND_RESOURCE);
	}
	*/
	return(0);
}

void CPostMostView::CheckStillPosting()
{
	int nItems = listNbTasks();
	for (int i=0;i<nItems;i++)
	{
		if (listGetTask(i)->m_nState==CTaskObject::PM_POSTING)
			return; // still posting
	}
	// no more thread posting
	OnPostStop();

}

void CPostMostView::OnUpdateFileClearDoneFiles(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(__FilesInQueue_OK());
}

void CPostMostView::OnFileClearDoneFiles() 
{
	CDeleteTaskDlg Dlg;

	strcpy(Dlg.m_szMessage, "Remove All Posted Files From The List?");
	
	if(Dlg.DoModal() == IDOK)
	{
		CWaitCursor wc;

		CListCtrl& theList = GetListCtrl();
		int nItems = theList.GetItemCount();

		int kk;

		CPtrArray TasksToClear;
		CTaskObject* pTaskObject;

		for(kk = (nItems-1); kk >= 0; kk--)
		{
			pTaskObject = (CTaskObject*) theList.GetItemData(kk);
			if (pTaskObject->m_nState == CTaskObject::PM_POSTED)
			{
				TasksToClear.Add(pTaskObject);
				theList.DeleteItem(kk);
			}
		}

		if(Dlg.m_bCheckValue == TRUE)
			SaveTasksToHistory(TasksToClear);

		for(kk = 0; kk < TasksToClear.GetSize(); kk++)
		{
			pTaskObject = (CTaskObject*) TasksToClear.GetAt(kk);
			delete pTaskObject;
		}

		TasksToClear.RemoveAll();

		m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());
	}
}

void CPostMostView::OnUpdateFileClearList(CCmdUI* pCmdUI)
{
	if(m_bRunning)
		pCmdUI->Enable(FALSE);
	else
	{
		if(listNbTasks() > 0)
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}

void CPostMostView::OnFileClearList() 
{
	CDeleteTaskDlg Dlg;

	strcpy(Dlg.m_szMessage, "Remove All Files From The List?");
	
	if(Dlg.DoModal() == IDOK)
	{
		RemoveAllTasks(Dlg.m_bCheckValue);
	}
}

void CPostMostView::OnUpdateFileClearErrors(CCmdUI* pCmdUI) 
{
	//if(m_bRunning)
		//pCmdUI->Enable(FALSE);
	//else
	{
		pCmdUI->Enable(__FilesInQueue_ERR());
	}
}

void CPostMostView::OnFileClearErrors() 
{
	if(AfxMessageBox("Resubmit Files In Error Into Queue?", MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;

	// Change all IMAGE_ERROR into IMAGE_QUEUE
	for (int i=0;i<listNbTasks();i++)
	{
		CTaskObject* o = listGetTask(i);
		if (o->m_nState == CTaskObject::PM_ERROR)
		{
			o->m_nState = CTaskObject::PM_QUEUED;
			UpdateDisplay(i,o,"");
		}
	}
	m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());
}

void CPostMostView::OnUpdateFileQueueAll(CCmdUI* pCmdUI) 
{
	//if(m_bRunning)
		//pCmdUI->Enable(FALSE);
	//else
	{
		if(listNbTasks() == 0)
			pCmdUI->Enable(FALSE);
		else
		{
			if(__FilesInQueue_ERR() || __FilesInQueue_OK())
				pCmdUI->Enable(TRUE);
			else
				pCmdUI->Enable(FALSE);
		}
	}
}

void CPostMostView::OnFileQueueAll() 
{
	if(AfxMessageBox("Resubmit All Files Into Queue?", MB_YESNO | MB_ICONQUESTION) == IDNO)
		return;

	int nItems = listNbTasks();
	int kk;
	CTaskObject* pTaskObject;

	// Change all into PM_QUEUED
	for(kk = 0; kk < nItems; kk++)
	{
		pTaskObject = listGetTask(kk);
		if(pTaskObject->m_nState != CTaskObject::PM_QUEUED && pTaskObject->m_nState != CTaskObject::PM_POSTING
		&& pTaskObject->m_nState != CTaskObject::PM_ENC_QUEUED && pTaskObject->m_nState != CTaskObject::PM_ENCODING
		  )
		{
			pTaskObject->m_nState = CTaskObject::PM_QUEUED;
			UpdateDisplay(kk,pTaskObject,"");

		}
	}
	m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());
}


void CPostMostView::OnUpdateFileAddFiles(CCmdUI* pCmdUI) 
{
	//if(m_bRunning)
		//pCmdUI->Enable(FALSE);
	//else
		pCmdUI->Enable(TRUE);
}

void CPostMostView::OnFileAddFiles() 
{
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
	strcpy(szInitialDirectory, m_Settings.m_szLastAddDir);

	if(strlen(szInitialDirectory) == 0)
		strcpy(szInitialDirectory, szCurrentDirectory);

	F_Dlg.m_ofn.lpstrInitialDir = &szInitialDirectory[0];

	if(!::CM_IsDirectory(F_Dlg.m_ofn.lpstrInitialDir))
		strcpy(szInitialDirectory, szCurrentDirectory);
	
	if(F_Dlg.DoModal() == IDOK)
	{
		// Get Filenames
		POSITION pos;
		m_Dropped_Files.RemoveAll();
		pos = F_Dlg.GetStartPosition();

		while(pos != (POSITION) NULL)
		{
			if(m_Settings.m_bSortFilesOnAdd)
				::CM_CSA_INSERT(m_Dropped_Files, (LPCTSTR) F_Dlg.GetNextPathName(pos));
			else
				m_Dropped_Files.Add( (LPCTSTR) F_Dlg.GetNextPathName(pos) );
		}
		
		if(m_Dropped_Files.GetSize() > 0)
		{
			PostMessage(WM_FILES_DROPPED);
		}
		else
		{
		}

	// Save Current Directory for next time..
	::GetCurrentDirectory(_MAX_PATH, m_Settings.m_szLastAddDir);


	}
	else
	{
	}

	// Release Buffer
	delete [] pBuffer;

	// Reset Current Directory
	::SetCurrentDirectory(szCurrentDirectory);

}

void CPostMostView::OnUpdateIndicatorFiles(CCmdUI* pCmdUI)
{
	char szTT[64];
	int nFileCount = 0;
	int nCount = listNbTasks();

	CTaskObject* pTaskObject;
	ULONGLONG ullTotalSizeBytes = 0;

	if(GetListCtrl().GetSelectedCount() > 0)
	{
		for(int kk = 0; kk < nCount; kk++)
		{
			if(GetListCtrl().GetItemState(kk, LVIS_SELECTED) == LVIS_SELECTED)
			{
				pTaskObject = (CTaskObject*) GetListCtrl().GetItemData(kk);
				ullTotalSizeBytes += pTaskObject->m_ullFileSize;
				++nFileCount;
			}
		}
	}
	else
	{
		for(int kk = 0; kk < nCount; kk++)
		{
			pTaskObject = (CTaskObject*) GetListCtrl().GetItemData(kk);
			ullTotalSizeBytes += pTaskObject->m_ullFileSize;
			++nFileCount;
		}
	}

	ULONGLONG ullTotalSizeK = ullTotalSizeBytes >> 10;

	if(ullTotalSizeK < 1024)
	{
		if(nFileCount == nCount)
			sprintf(szTT, "%d Files (%I64u K)", nCount, ullTotalSizeK);
		else
			sprintf(szTT, "= %d Files (%I64u K)", nFileCount, ullTotalSizeK);
	}
	else
	{
		ULONGLONG ullFileSizeMB = (ULONGLONG) ullTotalSizeK;
		ullFileSizeMB = ullFileSizeMB / 1024.0;
		if (ullFileSizeMB < 51200)
		{
			if(nFileCount == nCount)
				sprintf(szTT, "%d Files (%0.1I64u MB)", nCount, ullFileSizeMB);
			else
				sprintf(szTT, "= %d Files (%0.1I64u MB)", nFileCount, ullFileSizeMB);
		}
		else
		{
			ULONGLONG ullFileSizeGB = (ULONGLONG) ullFileSizeMB;
			ullFileSizeGB = ullFileSizeGB / 1024.0;
			if(nFileCount == nCount)
				sprintf(szTT, "%d Files (%0.1I64u GB)", nCount, ullFileSizeGB);
			else
				sprintf(szTT, "= %d Files (%0.1I64u GB)", nFileCount, ullFileSizeGB);
		}
	}
	
	pCmdUI->Enable(TRUE);
	pCmdUI->SetText(szTT);
}

void CPostMostView::OnUpdateFileClearSelected(CCmdUI* pCmdUI) 
{
	//if(m_bRunning)
	//{
	//	pCmdUI->Enable(FALSE);
	//	return;
	//}

	if(__FilesSelected_Posting())
		pCmdUI->Enable(FALSE);
	else if(GetListCtrl().GetSelectedCount() > 0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
	
}

void CPostMostView::OnFileClearSelected() 
{
	// Remove Selected Items From List
	CDeleteTaskDlg Dlg;

	sprintf(Dlg.m_szMessage, "Remove %d Selected Files From List?", GetListCtrl().GetSelectedCount());

	if(Dlg.DoModal() == IDOK)
	{
		CWaitCursor wc;

		int kk, nCount;
		CListCtrl& theList = GetListCtrl();
		CPtrArray TasksToClear;
		CTaskObject* pTaskObject;
		nCount = GetListCtrl().GetItemCount();

		for(kk = nCount - 1; kk >= 0; kk--)
		{
			if(theList.GetItemState(kk, LVIS_SELECTED) == LVIS_SELECTED)
			{
				pTaskObject = (CTaskObject*) theList.GetItemData(kk);
				TasksToClear.Add(pTaskObject);
				theList.DeleteItem(kk);
			}
		}

		if(Dlg.m_bCheckValue == TRUE)
			SaveTasksToHistory(TasksToClear);

		for(kk = 0; kk < TasksToClear.GetSize(); kk++)
		{
			pTaskObject = (CTaskObject*) TasksToClear.GetAt(kk);
			delete pTaskObject;
		}

		TasksToClear.RemoveAll();

		m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());

	}

}

void CPostMostView::RemoveAllTasks(BOOL bWriteToHistory)
{
	CListCtrl& theList = GetListCtrl();

	theList.LockWindowUpdate();

	int nItems = theList.GetItemCount();

	int kk;

	CPtrArray TasksToClear;
	CTaskObject* pTaskObject;

	for(kk = (nItems-1); kk >= 0; kk--)
	{
		pTaskObject = (CTaskObject*) theList.GetItemData(kk);
		TasksToClear.Add(pTaskObject);
		theList.DeleteItem(kk);
	}
	
	if(bWriteToHistory)
		SaveTasksToHistory(TasksToClear);

	for(kk = 0; kk < TasksToClear.GetSize(); kk++)
	{
		pTaskObject = (CTaskObject*) TasksToClear.GetAt(kk);
		delete pTaskObject;
	}

	TasksToClear.RemoveAll();

	GetListCtrl().UnlockWindowUpdate();

}

void CPostMostView::OnUpdateFileTaskProperties(CCmdUI* pCmdUI) 
{
	if(__FilesSelected_Posting())
		pCmdUI->Enable(FALSE);
	else
	{
		if(GetListCtrl().GetSelectedCount() > 0)
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
}

void CPostMostView::OnFileTaskProperties() 
{
	CTaskObject* pTaskObject;

	int nSelected = GetListCtrl().GetSelectedCount();
	int nCurrentItem = 0;

	int nItem;
	//TCHAR szPartCount[32];
	TCHAR szTitle[64];
	POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
	CRect rectItem;

	while(pos != NULL)
	{
		++nCurrentItem;
		nItem = GetListCtrl().GetNextSelectedItem(pos);
		pTaskObject = (CTaskObject*) GetListCtrl().GetItemData(nItem);
		
		GetListCtrl().EnsureVisible(nItem, FALSE);

		if(pTaskObject != NULL)
		{
			GetListCtrl().GetItemRect(nItem, &rectItem, LVIR_LABEL);
			GetListCtrl().ClientToScreen(&rectItem);

			sprintf(szTitle, "Task Item (%d of %d)", nCurrentItem, nSelected);

			if(ViewTaskProperties(pTaskObject, szTitle, &rectItem) == IDCANCEL)
				break;
			UpdateDisplay(nItem,pTaskObject,NULL);
/*
			// Update list view with new szSubject and number of parts
			GetListCtrl().SetItemText(nItem, 0, pTaskObject->m_szSubject);
			sprintf(szPartCount, "%d/%d", pTaskObject->m_dwaPartsToPost.GetSize(), pTaskObject->getNbParts());
			if(pTaskObject->m_bPostPrefixText == TRUE)
				strcat(szPartCount, "+0");
			GetListCtrl().SetItemText(nItem, 1, szPartCount);
			GetListCtrl().SetItemText(nItem, 3, (LPCTSTR) pTaskObject->m_strGroups);
*/
		}
	}
	m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());
}

int CPostMostView::ViewTaskProperties(CTaskObject* pTaskObject, LPCTSTR szTitle, LPRECT lprectItem)
{
	CTaskProp_FileAndParts PP_FileAndParts;
	CTaskProp_Groups       PP_Groups;
	CTaskProp_PrefixText   PP_PrefixText;
	CTaskProp_Log          PP_Log;

	CPostMostPropertySheet PS(szTitle);

	PS.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	if(lprectItem != NULL)
		PS.m_rectAnimateFrom = *lprectItem;

	PS.AddPage(&PP_FileAndParts);
	PS.AddPage(&PP_Groups);
	PS.AddPage(&PP_PrefixText);
	PS.AddPage(&PP_Log);

	CTaskObject proxyTaskObject;

	proxyTaskObject.Copy(pTaskObject);

	PP_FileAndParts.m_pTaskObject = &proxyTaskObject;
	PP_Groups.m_pTaskObject = &proxyTaskObject;
	PP_Groups.m_pSettings = &m_Settings;
	PP_PrefixText.m_pTaskObject = &proxyTaskObject;
	PP_Log.m_pTaskObject = &proxyTaskObject;


	if(PS.DoModal() == IDCANCEL)
		return(IDCANCEL);

	// Okay, save object information
	pTaskObject->Copy(&proxyTaskObject);

	return(IDOK);
}

void CPostMostView::OnUpdateFileHistory(CCmdUI* pCmdUI) 
{
	if(m_bRunning)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

void CPostMostView::OnFileHistory() 
{
	CHistoryDlg Dlg;

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pMainFrame->GetButtonRect(ID_FILE_HISTORY,&Dlg.m_rectAnimateFrom);

	Dlg.m_pView = this;
	Dlg.DoModal();
}

void CPostMostView::SaveTasksToHistory(CPtrArray& theTasks)
{
	if(theTasks.GetSize() == 0) return;

	CFile theHistoryFile;
	CFileException cfe;

	if(theHistoryFile.Open("HISTORY.DAT", CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate, &cfe) == FALSE)
	{
		TCHAR szError[600];
		TCHAR szErrorText[512];
		cfe.GetErrorMessage(szErrorText, 512);
		sprintf(szError, "Error Saving To History!\r\n\r\n%s", szErrorText);
		AfxMessageBox(szError, MB_ICONEXCLAMATION);
		return;
	}

	theHistoryFile.SeekToEnd();

	int nCount = theTasks.GetSize();
	CTaskObject* pTaskObject;

	for(int k = 0; k < nCount; k++)
	{
		pTaskObject = (CTaskObject*) theTasks.GetAt(k);
		if(pTaskObject != NULL)
		{
			if(strlen(pTaskObject->m_szHistoryDateTime) == 0)
				pTaskObject->WriteToFile(theHistoryFile, TRUE);
		}
	}

	theHistoryFile.Close();

	m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());
}

void CPostMostView::SaveTasksToQueue()
{
	int nCount = GetListCtrl().GetItemCount();

	if(nCount == 0) 
	{
		::DeleteFile("QUEUE.DAT"); // Ensure it's deleted..
		return;
	}

	CFile theQueueFile;
	CFileException cfe;

	if(theQueueFile.Open("QUEUE.DAT", CFile::modeWrite | CFile::modeCreate, &cfe) == FALSE)
	{
		TCHAR szError[600];
		TCHAR szErrorText[512];
		cfe.GetErrorMessage(szErrorText, 512);
		sprintf(szError, "Error Saving To Queue File!\r\n\r\n%s", szErrorText);
		AfxMessageBox(szError, MB_ICONEXCLAMATION);
		return;
	}

	CTaskObject* pTaskObject;

	for(int k = 0; k < nCount; k++)
	{
		pTaskObject = (CTaskObject*) GetListCtrl().GetItemData(k);
		if(pTaskObject != NULL)
		{
			pTaskObject->WriteToFile(theQueueFile, FALSE);
		}
	}

	theQueueFile.Close();
}

void CPostMostView::LoadTasksFromQueue()
{
	CFile theQueueFile;
	CFileException cfe;

	if(theQueueFile.Open("QUEUE.DAT", CFile::modeRead, &cfe) == FALSE)
		return;

	CTaskObject OneTask;

	while(OneTask.ReadFromFile(theQueueFile))
	{
		if (OneTask.m_nState == CTaskObject::PM_POSTING)
			OneTask.m_nState = CTaskObject::PM_QUEUED;
		else if (OneTask.m_nState == CTaskObject::PM_ENCODING)
			OneTask.m_nState = CTaskObject::PM_ENC_QUEUED;
		AppendTaskToQueue(OneTask);
	}

	theQueueFile.Close();
}

void CPostMostView::OnUpdateFileQueueSel(CCmdUI* pCmdUI) 
{
	if(GetListCtrl().GetSelectedCount() == 0)
		pCmdUI->Enable(FALSE);
	//else if (__FilesSelected_Posting())
		//pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(__FilesSelected_Not_Q());
}

void CPostMostView::OnFileQueueSel() 
{
	int nCount = GetListCtrl().GetSelectedCount();

	if(nCount == 0) return;

	TCHAR szText[64];

	sprintf(szText, "Re-Queue Selected %d Files?", nCount);

	if(AfxMessageBox(szText, MB_OKCANCEL) == IDCANCEL)
		return;

	// Re-Queue Selected Files
	CTaskObject* pTaskObject;
	int k, nItems;
	nItems = listNbTasks();

	for(k = 0; k < nItems; k++)
	{
		if(GetListCtrl().GetItemState(k, LVIS_SELECTED) == LVIS_SELECTED)
		{
			pTaskObject = listGetTask(k);
			if (pTaskObject->m_nState != CTaskObject::PM_QUEUED
			 && pTaskObject->m_nState != CTaskObject::PM_ENC_QUEUED
			 && pTaskObject->m_nState!=CTaskObject::PM_POSTING
			 && pTaskObject->m_nState!=CTaskObject::PM_ENCODING)
			{
				pTaskObject->m_nState = CTaskObject::PM_QUEUED;
				UpdateDisplay(k,pTaskObject,"");
			}
		}
	}
	m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());
}

int TaskState2Image(int State)
{
	int nImage;
	switch(State)
	{
	case CTaskObject::PM_ERROR:
		nImage = IMAGE_ERROR;
		break;
	case CTaskObject::PM_POSTED:
		nImage = IMAGE_DONE;
		break;
	case CTaskObject::PM_HOLDING:
	case CTaskObject::PM_ENC_HOLDING:
		nImage = IMAGE_HOLDING;
		break;
	case CTaskObject::PM_POSTING:
		nImage = IMAGE_POSTING;
		break;
	case CTaskObject::PM_QUEUED:
		nImage = IMAGE_QUEUE;
		break;
	case CTaskObject::PM_ENC_QUEUED:
		nImage = IMAGE_ENC_QUEUE;
		break;
	case CTaskObject::PM_ENCODING:
		nImage = IMAGE_ENCODING;
		break;
	case CTaskObject::PM_UNDEFINED:
	default:
		nImage = IMAGE_ERROR;
		break;
	}
	return nImage;
}

void CPostMostView::AppendTaskToQueue(CTaskObject& theTask)
{
	int nAppendPos = GetListCtrl().GetItemCount();

	CTaskObject* pNewTask;

	int nImage;
	int nParts;
	TCHAR szPartCount[32];

	pNewTask = new CTaskObject;
	pNewTask->Copy(&theTask);

	nImage = TaskState2Image(pNewTask->m_nState);

	nParts = pNewTask->getNbParts();

	sprintf(szPartCount, "%d", nParts);
	sprintf(szPartCount, "%d/%d", theTask.m_dwaPartsToPost.GetSize(), nParts);
	if(theTask.m_bPostPrefixText == TRUE) strcat(szPartCount, "+0");

	GetListCtrl().InsertItem(nAppendPos, pNewTask->m_szSubject, nImage);
	GetListCtrl().SetItemText(nAppendPos, 1, szPartCount);
	GetListCtrl().SetItemText(nAppendPos, 2, pNewTask->m_szLastResult);
	GetListCtrl().SetItemText(nAppendPos, 3, (LPCTSTR) pNewTask->m_strGroups);
	GetListCtrl().SetItemData(nAppendPos, (DWORD) pNewTask);
}

BOOL CPostMostView::__FilesInQueue_ERR()
{
	// Check to see if any items are marked in ERR
	int nItems = listNbTasks();
	int kk;
	for(kk = 0; kk < nItems; kk++)
	{
		if (listGetTask(kk)->m_nState==CTaskObject::PM_ERROR)
			return TRUE;
	}

	return(FALSE);
}

BOOL CPostMostView::__FilesInQueue_OK()
{
	// Check to see if any items are marked as OK
	int nItems = listNbTasks();
	for(int kk = 0; kk < nItems; kk++)
	{
		if (listGetTask(kk)->m_nState==CTaskObject::PM_POSTED)
			return TRUE;
	}

	return(FALSE);
}

BOOL CPostMostView::__FilesSelected_Not_Q()
{
	// Check to see if any items are marked for QUEUE
	int nItems = listNbTasks();
	int kk;
	for(kk = 0; kk < nItems; kk++)
	{
		if(GetListCtrl().GetItemState(kk, LVIS_SELECTED) == LVIS_SELECTED)
		{
			if (listGetTask(kk)->m_nState!=CTaskObject::PM_QUEUED)
				return TRUE;
		}
	}

	return(FALSE);
}

BOOL CPostMostView::__FilesSelected_Posting()
{
	// Check to see if any items are marked for POSTING
	int nItems = listNbTasks();
	int kk;
	for(kk = 0; kk < nItems; kk++)
	{
		if(GetListCtrl().GetItemState(kk, LVIS_SELECTED) == LVIS_SELECTED)
		{
			if (listGetTask(kk)->m_nState==CTaskObject::PM_POSTING
			 || listGetTask(kk)->m_nState==CTaskObject::PM_ENCODING)
				return TRUE;
		}
	}

	return(FALSE);
}

BOOL CPostMostView::__FilesInQueue_Q()
{
	// Check to see if any items are marked for QUEUE
	int nItems = listNbTasks();
	int kk;
	for(kk = 0; kk < nItems; kk++)
	{
		if (listGetTask(kk)->m_nState==CTaskObject::PM_QUEUED)
			return TRUE;
	}

	return(FALSE);
}

// int __ComputeBatchSize()
ULONGLONG CPostMostView::__ComputeBatchSize()
{
	// Compute batch size in lines
	CTaskObject* pTaskObject;

	int kk;
	int nItems = listNbTasks();

	ULONGLONG nSizeBatch = 0;
	
	int n, nParts, nPTPSize, nLines;

	for(kk = 0; kk < nItems; kk++)
	{
		pTaskObject = listGetTask(kk);
		if( pTaskObject->m_nState == CTaskObject::PM_QUEUED
		 || pTaskObject->m_nState == CTaskObject::PM_ENC_QUEUED
		 || pTaskObject->m_nState == CTaskObject::PM_ENCODING
		 || pTaskObject->m_nState == CTaskObject::PM_POSTING
		  )
		{
			nParts = pTaskObject->getNbParts();
			nPTPSize = pTaskObject->m_dwaPartsToPost.GetSize();
			if(nPTPSize > 0)
			{
				for(n = 0; n < nPTPSize; n++)
				{
					if(pTaskObject->m_dwaPartsToPost.GetAt(n) != (UINT) nParts)
					{
						// Full part to post..
						nSizeBatch += pTaskObject->getMaxLines();
					}
					else
					{
						nLines = ((pTaskObject->m_ullFileSize % pTaskObject->m_nPartSize) / YENCLEN);
						nSizeBatch += nLines;
					}
				}
			}
		}
	}

	TRACE("__ComputeBatchSize() = %I64u lines\n", nSizeBatch);
	return(nSizeBatch);
}

void CPostMostView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// Adjust point for keyboard activation
	if((point.x == -1) && (point.y == -1))
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		point = rcClient.TopLeft();
		point.Offset(8,8);
		ClientToScreen(&point);
	}

	// Load Context Menu
	CMenu menuContext;
	menuContext.LoadMenu(IDR_MENU_CONTEXT);

	// Get Context Menu from First Sub-Menu
	CMenu* pMenu = menuContext.GetSubMenu(0);
	ASSERT_VALID(pMenu);

	// Display it..
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, AfxGetMainWnd(), NULL);
	
}

void CPostMostView::OnUpdateFileReassignNewsgroups(CCmdUI* pCmdUI) 
{
	if( (GetListCtrl().GetSelectedCount() > 0) && !__FilesSelected_Posting())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CPostMostView::OnFileReassignNewsgroups() 
{
	DWORD dwPageActivation = 0;

	CPropertySheet			PS;
	CPropPage_Add_Groups	PP_Groups;

	PP_Groups.m_pdwActivation = &dwPageActivation;
	PP_Groups.m_pSettings = &m_Settings;

	PS.SetTitle("Re-Assign Newsgroups");
	PS.AddPage(&PP_Groups);
	PS.m_psh.dwFlags |= PSH_NOAPPLYNOW;

	if(PS.DoModal() == IDCANCEL)
	{
		return;
	}
	
	LPCTSTR lpszGroupList = (LPCTSTR) PP_Groups.m_strGroupList;

	CTaskObject* pTaskObject;
	int nItems = GetListCtrl().GetItemCount();
	int kk;

	for(kk = 0; kk < nItems; kk++)
	{
		if(GetListCtrl().GetItemState(kk, LVIS_SELECTED) == LVIS_SELECTED)
		{
			pTaskObject = (CTaskObject*) GetListCtrl().GetItemData(kk);
			if(pTaskObject != NULL)
			{
				pTaskObject->m_strGroups = lpszGroupList;
				GetListCtrl().SetItemText(kk, 3, lpszGroupList);
			}
		}
	}
}

void CPostMostView::UpdateItemDisplay(int nIndex)
{
	CTaskObject* pTaskObject;
	pTaskObject = (CTaskObject*) GetListCtrl().GetItemData(nIndex);

	int nParts;
	TCHAR szPartCount[32];

	nParts = pTaskObject->getNbParts();

	sprintf(szPartCount, "%d/%d", pTaskObject->m_dwaPartsToPost.GetSize(), nParts);
	if(pTaskObject->postPrefixText()) strcat(szPartCount, "+0");

	// Set Item Image
	LVITEM theItem;
	::ZeroMemory(&theItem, sizeof(LVITEM));
	theItem.mask = LVIF_IMAGE;
	theItem.iItem = nIndex;
	theItem.iImage = TaskState2Image(pTaskObject->m_nState);

	GetListCtrl().SetItem(&theItem);

	// Set Item Column Text
	GetListCtrl().SetItemText(nIndex, 0, pTaskObject->m_szSubject);
	GetListCtrl().SetItemText(nIndex, 1, szPartCount);
	GetListCtrl().SetItemText(nIndex, 2, pTaskObject->m_szLastResult);
	GetListCtrl().SetItemText(nIndex, 3, (LPCTSTR) pTaskObject->m_strGroups);
}

void CPostMostView::UpdateDisplay(CTaskObject* pTaskObject, const TCHAR* message)
{
	int nItems = listNbTasks();
	int nIndex;
	for (nIndex = 0;nIndex<nItems;nIndex++)
		if (listGetTask(nIndex)==pTaskObject) break;
	if (nIndex<nItems)
		UpdateDisplay(nIndex,pTaskObject,message);
}

void CPostMostView::UpdateDisplay(int nIndex, CTaskObject* pTaskObject, const TCHAR* message)
{
	int nParts = pTaskObject->getNbParts();

	TCHAR szPartCount[32];

	sprintf(szPartCount, "%d/%d", pTaskObject->m_dwaPartsToPost.GetSize(), nParts);

	if(pTaskObject->postPrefixText())
		strcat(szPartCount, "+0");

	CListCtrl& listCtrl = GetListCtrl();

	listCtrl.SetItemText(nIndex, 0, pTaskObject->m_szSubject);

	listCtrl.SetItemText(nIndex, 1, szPartCount);			// Computed Number Of Parts

	LVITEM theItem;
	::ZeroMemory(&theItem, sizeof(LVITEM));
	theItem.mask = LVIF_IMAGE;
	theItem.iItem = nIndex;

	theItem.iImage = TaskState2Image(pTaskObject->m_nState);

	listCtrl.SetItem(&theItem);

	if (message!=NULL)
		listCtrl.SetItemText(nIndex, 2, message);

	listCtrl.SetItemText(nIndex, 3, (LPCTSTR) pTaskObject->m_strGroups);

}

void CPostMostView::OnUpdatePostStartAt(CCmdUI* pCmdUI) 
{
	if(m_bRunning)
		pCmdUI->Enable(FALSE);
	else
	{
		if(GetListCtrl().GetItemCount() == 0)
			pCmdUI->Enable(FALSE);
		else
		{
			pCmdUI->Enable(__FilesInQueue_Q());
		}
	}	
}

void CPostMostView::OnPostStartAt() 
{
	CDelayedPostingDlg D;

	if(m_bRepeat == TRUE)
	{
		D.CheckRepeat=TRUE;
	}

	D.m_nInit_Hour = m_Settings.m_nStartTimeSel[0];
	D.m_nInit_Min  = m_Settings.m_nStartTimeSel[1];
	D.m_nInit_AMPM = m_Settings.m_nStartTimeSel[2];

	D.m_nInit_Hour_End = m_Settings.m_nStopTimeSel[0];
	D.m_nInit_Min_End  = m_Settings.m_nStopTimeSel[1];
	D.m_nInit_AMPM_End = m_Settings.m_nStopTimeSel[2];

	D.CheckStop = m_Settings.m_nCheckStop;

//	((CButton*) GetDlgItem(IDC_CHECK_STOP))->SetCheck(m_Settings.m_nCheckStop); 

	if(D.DoModal() == IDOK)
	{
		m_Settings.m_nStartTimeSel[0] = D.m_nInit_Hour;
		m_Settings.m_nStartTimeSel[1] = D.m_nInit_Min;
		m_Settings.m_nStartTimeSel[2] = D.m_nInit_AMPM;

		m_Settings.m_nStopTimeSel[0] = D.m_nInit_Hour_End;
		m_Settings.m_nStopTimeSel[1] = D.m_nInit_Min_End;
		m_Settings.m_nStopTimeSel[2] = D.m_nInit_AMPM_End;

		m_Settings.m_nCheckStop = D.CheckStop;

		m_TimeToStart = D.m_StartTime;
		m_TimeToStartEnd = D.m_StartTimeEnd;

		// Set State Variables
		m_bWaiting  = TRUE;
		m_bRunning  = TRUE;
		m_bWaiting2 = D.m_StopValue;
		m_bRepeat   = D.CheckRepeat;
			
		// Update Tray Icon
		((CMainFrame*) AfxGetApp()->m_pMainWnd)->NotifyIcon(NIM_MODIFY, IDI_ICON_TRAY_WAITING);

		UpdateWaitTime();
	}
}

void CPostMostView::StartWaitTimer()
{
	m_nWaitTimerID = SetTimer(4926, 1000, NULL);	
	ASSERT(m_nWaitTimerID);

}

void CPostMostView::StopWaitTimer()
{
	KillTimer(m_nWaitTimerID);
}

void CPostMostView::UpdateWaitTime()
{
	//Declaration for STOP ONLY IF FILE FINISHED UPPED
	TCHAR szArticle[320];
	int nPart, nPartOf;
	CDelayedPostingDlg D;

	m_ThreadMarshal.GetCurrentArticleStatus(0, szArticle, &nPart, &nPartOf);
	//End of these declaration

	CTime currentTime = CTime::GetCurrentTime();

	CTimeSpan timeToGo = m_TimeToStart - currentTime;
	CTimeSpan timeToLeft = m_TimeToStartEnd - currentTime;

	TCHAR szTT[128];
	if(m_bWaiting==TRUE)
	sprintf(szTT, "Waiting %02d:%02d:%02d", timeToGo.GetHours(), timeToGo.GetMinutes(), timeToGo.GetSeconds());//PB
	else
	sprintf(szTT, "Stop in %02d:%02d:%02d", timeToLeft.GetHours(), timeToLeft.GetMinutes(), timeToLeft.GetSeconds());//PB

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pMainFrame->SetProgressIndicator(szTT);

	if( (timeToGo.GetHours() <= 0) &&
		(timeToGo.GetMinutes() <= 0) &&
		(timeToGo.GetSeconds() <= 0) &&
		m_bWaiting==TRUE) 
	{
		m_bWaiting  = FALSE;
		m_bRunning  = FALSE;

		((CMainFrame*) AfxGetApp()->m_pMainWnd)->NotifyIcon(NIM_MODIFY, IDI_ICON_TRAY_READY);
		OnPostStart();
	}


	if( (timeToLeft.GetHours() <= 0) &&
		(timeToLeft.GetMinutes() <= 0) &&
		(timeToLeft.GetSeconds() <= 0) &&
		m_bWaiting2==TRUE &&
		m_bWaiting==FALSE &&
		nPart ==1)//Modification STOP ONLY IF FILE COMPLETED
	{
		OnPostStop();
		m_bWaiting2 = FALSE;
		if(m_bRepeat == TRUE) OnPostStartAt();
	}

}

void CPostMostView::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 4926)
	{
		TRACE("CPostMostView::OnTimer(%d)\n", nIDEvent);

		if(m_bRunning == FALSE)	return;
		if(m_bWaiting2 == TRUE && m_bWaiting == FALSE)
		{
			UpdateWaitTime();
			return;
		}

		if(m_bWaiting == TRUE)
		{
			UpdateWaitTime();
			return;
		}

		// Posting .. See if we have the Stall Timeout Set
		if(m_Settings.m_bStallTimeoutEnabled == FALSE)
			return;

		// Get Progress and Check for Stall
		int n;
		int nSecondsToScan = m_Settings.m_nMinuteStallTimeout * 60;
		BOOL bFlatlined = TRUE;
		int nTransmissionHistory[300];

		if(m_ThreadMarshal.GetTransmissionHistoryTotal(nTransmissionHistory) == FALSE)
			return;

		CTime theTimeNow = CTime::GetCurrentTime();
		CTimeSpan theTimeSpan = theTimeNow - m_Time_Started;

		n = theTimeSpan.GetTotalSeconds();
		if(n < nSecondsToScan) return;

		for(n = 0; n < nSecondsToScan; n++)
		{
			if(nTransmissionHistory[n] != 0)
			{
				bFlatlined = FALSE;
				TRACE("Not Flatlined: nTransmissionHistory[%d] = %d\n", n, nTransmissionHistory[n]);
				break;
			}
		}

		if(bFlatlined)
		{
			// TODO: Stop and Restart
			TRACE("**** Stall Timer Exceeded - Start/Restart!\n");
			OnPostStop();
			OnPostStart();

		}
		return;

	}

	CListView::OnTimer(nIDEvent);
}

void CPostMostView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// if((m_bRunning == FALSE) && (nFlags & 0x20000000))  // ALT Key pressed
	if(m_bRunning == FALSE)
	{
		TRACE("nFlags = 0x%08X\n", nFlags);

		switch(nChar)
		{
			// CTRL-A selects all items
		case 1:
			List_SelectAllItems();
			break;
			// CTRL-U moves selected items up list
		case 21:
			List_MoveSelected_Up();
			break;

			// CTRL-D moves selected items up list
		case 4:
			List_MoveSelected_Down();
			break;

			// CTRL-T moves selected items to top
		case 20:
			List_MoveSelected_Top();
			break;

			// CTRL-B moves selected items to bottom
		case 2:
			List_MoveSelected_Bottom();
			break;

		default:
			break;
		}
	}

	TRACE("CPostMostView::OnChar:: nChar = %d, nFlags = %08X\n", nChar, nFlags);	
	CListView::OnChar(nChar, nRepCnt, nFlags);
}

void CPostMostView::List_SelectAllItems()
{
	int nItems = GetListCtrl().GetItemCount();
	for(int k = 0; k < nItems; k++)
		GetListCtrl().SetItemState(k, LVIS_SELECTED, LVIS_SELECTED);
}

void CPostMostView::List_MoveSelected_Up()
{
	CListCtrl& listCtrl = GetListCtrl();

	int nItems = listCtrl.GetItemCount();
	int nSelected = listCtrl.GetSelectedCount();

	if((nSelected == 0) || (nItems == 1))
		return;
	
	int k;
	BOOL bAnyShifted = FALSE;
	DWORD dwItem, dwItemPrev;
	CDWordArray dwa_AllItems;
	CDWordArray dwa_SelectedItems;
	dwa_SelectedItems.SetSize(0, nSelected);
	dwa_SelectedItems.SetSize(0, nItems);

	// Collect Item Information
	for(k = 0; k < nItems; k++)
	{
		dwa_AllItems.Add((DWORD) listCtrl.GetItemData(k));
		if(listCtrl.GetItemState(k, LVIS_SELECTED) == LVIS_SELECTED)
			::CM_DWA_INSERT(dwa_SelectedItems, (int) listCtrl.GetItemData(k));
	}

	// Shift Items in dwa varaibles
	for(k = 1; k < nItems; k++)
	{
		dwItem = dwa_AllItems.GetAt(k);
		dwItemPrev = dwa_AllItems.GetAt(k-1);

		// Skip if not selected or item previous is selected
		if(::CM_DWA_SEEK(dwa_SelectedItems, dwItem) < 0) continue;
		if(::CM_DWA_SEEK(dwa_SelectedItems, (int) dwItemPrev) >= 0) continue;

		dwa_AllItems.SetAt(k, dwItemPrev);
		dwa_AllItems.SetAt(k-1, dwItem);

		bAnyShifted = TRUE;
	}

	// Bail now if nothing done
	if(bAnyShifted == FALSE)
	{
		dwa_AllItems.RemoveAll();
		dwa_SelectedItems.RemoveAll();
		return;
	}

	// Update List Control
	for(k = 0; k < nItems; k++)
	{
		dwItem = dwa_AllItems.GetAt(k);
		if(dwItem == listCtrl.GetItemData(k))
			continue;

		listCtrl.SetItemData(k, dwItem);
		UpdateItemDisplay(k);
		if(::CM_DWA_SEEK(dwa_SelectedItems, (int) dwItem) >= 0)
			listCtrl.SetItemState(k, LVIS_SELECTED, LVIS_SELECTED);
		else
			listCtrl.SetItemState(k, 0, LVIS_SELECTED);
	}


	// Finished
	dwa_AllItems.RemoveAll();
	dwa_SelectedItems.RemoveAll();

}

void CPostMostView::List_MoveSelected_Down()
{
	CListCtrl& listCtrl = GetListCtrl();

	int nItems = listCtrl.GetItemCount();
	int nSelected = listCtrl.GetSelectedCount();

	if((nSelected == 0) || (nItems == 1))
		return;

	int k;
	BOOL bAnyShifted = FALSE;
	DWORD dwItem, dwItemNext;
	CDWordArray dwa_AllItems;
	CDWordArray dwa_SelectedItems;
	dwa_SelectedItems.SetSize(0, nSelected);
	dwa_SelectedItems.SetSize(0, nItems);

	// Collect Item Information
	for(k = 0; k < nItems; k++)
	{
		dwa_AllItems.Add((DWORD) listCtrl.GetItemData(k));
		if(listCtrl.GetItemState(k, LVIS_SELECTED) == LVIS_SELECTED)
			::CM_DWA_INSERT(dwa_SelectedItems, (int) listCtrl.GetItemData(k));
	}

	// Shift Items in dwa varaibles
	for(k = (nItems-2); k >= 0; k--)
	{
		dwItem = dwa_AllItems.GetAt(k);
		dwItemNext = dwa_AllItems.GetAt(k+1);

		// Skip if not selected
		if(::CM_DWA_SEEK(dwa_SelectedItems, dwItem) < 0)
			continue;

		// Skip if item previous is selected
		if(::CM_DWA_SEEK(dwa_SelectedItems, (int) dwItemNext) >= 0)
			continue;

		dwa_AllItems.SetAt(k, dwItemNext);
		dwa_AllItems.SetAt(k+1, dwItem);

		bAnyShifted = TRUE;
	}

	// Bail now if nothing done
	if(bAnyShifted == FALSE)
	{
		dwa_AllItems.RemoveAll();
		dwa_SelectedItems.RemoveAll();
		return;
	}

	// Update List Control
	for(k = 0; k < nItems; k++)
	{
		dwItem = dwa_AllItems.GetAt(k);
		if(dwItem == listCtrl.GetItemData(k))
			continue;

		listCtrl.SetItemData(k, dwItem);
		UpdateItemDisplay(k);
		if(::CM_DWA_SEEK(dwa_SelectedItems, (int) dwItem) >= 0)
			listCtrl.SetItemState(k, LVIS_SELECTED, LVIS_SELECTED);
		else
			listCtrl.SetItemState(k, 0, LVIS_SELECTED);
	}


	// Finished
	dwa_AllItems.RemoveAll();
	dwa_SelectedItems.RemoveAll();
}

void CPostMostView::List_MoveSelected_Top()
{
	CListCtrl& listCtrl = GetListCtrl();

	int nItems = listCtrl.GetItemCount();
	int nSelected = listCtrl.GetSelectedCount();

	if((nSelected == 0) || (nItems == 1))
		return;
	
	int k;
	BOOL bAnyShifted = FALSE;
	DWORD dwItem, dwItemPrev;
	CDWordArray dwa_AllItems;
	CDWordArray dwa_SelectedItems;
	dwa_SelectedItems.SetSize(0, nSelected);
	dwa_SelectedItems.SetSize(0, nItems);

	// Collect Item Information
	for(k = 0; k < nItems; k++)
	{
		dwa_AllItems.Add((DWORD) listCtrl.GetItemData(k));
		if(listCtrl.GetItemState(k, LVIS_SELECTED) == LVIS_SELECTED)
			::CM_DWA_INSERT(dwa_SelectedItems, (int) listCtrl.GetItemData(k));
	}

	// Keep Shifting Items in dwa varaibles up until none are shifted
	int nShifts = 0;
	do
	{
		bAnyShifted = FALSE;

		for(k = 1; k < nItems; k++)
		{
			dwItem = dwa_AllItems.GetAt(k);
			dwItemPrev = dwa_AllItems.GetAt(k-1);

			// Skip if not selected or item previous is selected
			if(::CM_DWA_SEEK(dwa_SelectedItems, dwItem) < 0) continue;
			if(::CM_DWA_SEEK(dwa_SelectedItems, (int) dwItemPrev) >= 0) continue;

			dwa_AllItems.SetAt(k, dwItemPrev);
			dwa_AllItems.SetAt(k-1, dwItem);

			bAnyShifted = TRUE;
			++nShifts;
		}

	} while(bAnyShifted == TRUE);

	// Bail now if nothing done
	if(nShifts == 0)
	{
		dwa_AllItems.RemoveAll();
		dwa_SelectedItems.RemoveAll();
		return;
	}

	// Update List Control
	for(k = 0; k < nItems; k++)
	{
		dwItem = dwa_AllItems.GetAt(k);
		if(dwItem == listCtrl.GetItemData(k))
			continue;

		listCtrl.SetItemData(k, dwItem);
		UpdateItemDisplay(k);
		if(::CM_DWA_SEEK(dwa_SelectedItems, (int) dwItem) >= 0)
			listCtrl.SetItemState(k, LVIS_SELECTED, LVIS_SELECTED);
		else
			listCtrl.SetItemState(k, 0, LVIS_SELECTED);
	}


	// Finished
	dwa_AllItems.RemoveAll();
	dwa_SelectedItems.RemoveAll();
}

void CPostMostView::List_MoveSelected_Bottom()
{
	CListCtrl& listCtrl = GetListCtrl();

	int nItems = listCtrl.GetItemCount();
	int nSelected = listCtrl.GetSelectedCount();

	if((nSelected == 0) || (nItems == 1))
		return;

	int k;
	BOOL bAnyShifted = FALSE;
	DWORD dwItem, dwItemNext;
	CDWordArray dwa_AllItems;
	CDWordArray dwa_SelectedItems;
	dwa_SelectedItems.SetSize(0, nSelected);
	dwa_SelectedItems.SetSize(0, nItems);

	// Collect Item Information
	for(k = 0; k < nItems; k++)
	{
		dwa_AllItems.Add((DWORD) listCtrl.GetItemData(k));
		if(listCtrl.GetItemState(k, LVIS_SELECTED) == LVIS_SELECTED)
			::CM_DWA_INSERT(dwa_SelectedItems, (int) listCtrl.GetItemData(k));
	}

	// Keep Shifting Items in dwa varaibles down until no shifts are made
	int nShifts = 0;
	do
	{
		bAnyShifted = FALSE;
		for(k = (nItems-2); k >= 0; k--)
		{
			dwItem = dwa_AllItems.GetAt(k);
			dwItemNext = dwa_AllItems.GetAt(k+1);

			// Skip if not selected
			if(::CM_DWA_SEEK(dwa_SelectedItems, dwItem) < 0)
				continue;

			// Skip if item previous is selected
			if(::CM_DWA_SEEK(dwa_SelectedItems, (int) dwItemNext) >= 0)
				continue;

			dwa_AllItems.SetAt(k, dwItemNext);
			dwa_AllItems.SetAt(k+1, dwItem);

			bAnyShifted = TRUE;
			++nShifts;
		}

	} while(bAnyShifted == TRUE);

	// Bail now if nothing done
	if(nShifts == 0)
	{
		dwa_AllItems.RemoveAll();
		dwa_SelectedItems.RemoveAll();
		return;
	}

	// Update List Control
	for(k = 0; k < nItems; k++)
	{
		dwItem = dwa_AllItems.GetAt(k);
		if(dwItem == listCtrl.GetItemData(k))
			continue;

		listCtrl.SetItemData(k, dwItem);
		UpdateItemDisplay(k);
		if(::CM_DWA_SEEK(dwa_SelectedItems, (int) dwItem) >= 0)
			listCtrl.SetItemState(k, LVIS_SELECTED, LVIS_SELECTED);
		else
			listCtrl.SetItemState(k, 0, LVIS_SELECTED);
	}


	// Finished
	dwa_AllItems.RemoveAll();
	dwa_SelectedItems.RemoveAll();
}


void CPostMostView::OnUpdateMoveTaskBottom(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((GetListCtrl().GetSelectedCount() > 0));
}

void CPostMostView::OnMoveTaskBottom() 
{
	List_MoveSelected_Bottom();
}

void CPostMostView::OnUpdateMoveTaskDown(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((GetListCtrl().GetSelectedCount() > 0));
}

void CPostMostView::OnMoveTaskDown() 
{
	List_MoveSelected_Down();
}

void CPostMostView::OnUpdateMoveTaskTop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((GetListCtrl().GetSelectedCount() > 0));
}

void CPostMostView::OnMoveTaskTop() 
{
	List_MoveSelected_Top();
}

void CPostMostView::OnUpdateMoveTaskUp(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((GetListCtrl().GetSelectedCount() > 0));
}

void CPostMostView::OnMoveTaskUp() 
{
	List_MoveSelected_Up();
}

void CPostMostView::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	
	if(pLVKeyDow->wVKey == VK_DELETE)
	{
		TRACE("CPostMostView::OnKeydown() - DELETE\n");
		int nItemsTotal = GetListCtrl().GetItemCount();
		int nItemsSelected = GetListCtrl().GetSelectedCount();

		if(nItemsTotal > 0)
		{
			if(nItemsTotal == nItemsSelected)
			{
				PostMessage(WM_COMMAND, ID_FILE_CLEARLIST);
				*pResult = 0;
				return;
			}

			if(nItemsSelected > 0)
				PostMessage(WM_COMMAND, ID_FILE_CLEARSELECTED);
			else
				PostMessage(WM_COMMAND, ID_FILE_CLEARLIST);

			*pResult = 0;
			return;
		}
	}
	
	*pResult = 0;
}

void CPostMostView::OnUpdateFileSetHold(CCmdUI* pCmdUI) 
{
	if((__FilesSelected_Posting()) || (GetListCtrl().GetSelectedCount() == 0))
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	// Check to see if any items are NOT marked for PM_HOLDING
	CListCtrl& theList = GetListCtrl();
	int nItems = theList.GetItemCount();
	int kk;
	CTaskObject* pTaskObject;

	for(kk = 0; kk < nItems; kk++)
	{
		if(theList.GetItemState(kk, LVIS_SELECTED) == LVIS_SELECTED)
		{
			pTaskObject = (CTaskObject*) theList.GetItemData(kk);

			if(pTaskObject->m_nState != CTaskObject::PM_HOLDING
			&& pTaskObject->m_nState != CTaskObject::PM_ENC_HOLDING)
			{
				pCmdUI->Enable(TRUE);
				return;
			}
		}
	}

	pCmdUI->Enable(FALSE);

}

void CPostMostView::OnFileSetHold() 
{
	// Check to see if any items are marked for QUEUE
	CListCtrl& theList = GetListCtrl();
	int nItems = theList.GetItemCount();
	int kk;

	CTaskObject* pTaskObject;

	for(kk = 0; kk < nItems; kk++)
	{
		if(theList.GetItemState(kk, LVIS_SELECTED) == LVIS_SELECTED)
		{
			pTaskObject = (CTaskObject*) theList.GetItemData(kk);

			if (pTaskObject->m_nState != CTaskObject::PM_HOLDING
			 && pTaskObject->m_nState != CTaskObject::PM_POSTING
			 && pTaskObject->m_nState != CTaskObject::PM_ENC_HOLDING
			 && pTaskObject->m_nState != CTaskObject::PM_ENCODING)
			{
				if (pTaskObject->m_nState == CTaskObject::PM_ENC_QUEUED)
					pTaskObject->m_nState = CTaskObject::PM_ENC_HOLDING;
				else
					pTaskObject->m_nState = CTaskObject::PM_HOLDING;
				UpdateDisplay(kk,pTaskObject,NULL);
			}
		}
	}
	m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());
}

void CPostMostView::OnUpdateGeneratePar2Vol(CCmdUI* pCmdUI) 
{
	BOOL enable = FALSE;
	CListCtrl &listCtrl = GetListCtrl();
	//pCmdUI->Enable(listCtrl.GetSelectedCount() == 1);	
	if (listCtrl.GetSelectedCount() == 1)
	{
		POSITION pos = listCtrl.GetFirstSelectedItemPosition();
		int index = listCtrl.GetNextSelectedItem(pos);
		CTaskObject* pTaskObject = listGetTask(index);
		int namelen = strlen(pTaskObject->m_szFilename);
		if (namelen>=5 && !stricmp(pTaskObject->m_szFilename+namelen-5,".par2"))
		{ // the selected file is a .par2 :-)
			enable = TRUE;
		}
	}
	pCmdUI->Enable(enable);
}

void CPostMostView::OnGeneratePar2Vol() 
{
	TRACE("CPostMostView::OnGeneratePar2Vol()\n");

	BOOL enable = FALSE;
	CListCtrl &listCtrl = GetListCtrl();
	if (listCtrl.GetSelectedCount() != 1) return;

	POSITION pos = listCtrl.GetFirstSelectedItemPosition();
	int index = listCtrl.GetNextSelectedItem(pos);
	CTaskObject* pTaskObject = listGetTask(index);
	int namelen = strlen(pTaskObject->m_szFilename);
	if (!(namelen>=5 && !stricmp(pTaskObject->m_szFilename+namelen-5,".par2"))) return;

	// the selected file is a .par2 :-)

	par2::PostFiles *post = new par2::PostFiles(pTaskObject->m_szFilename);

	TCHAR sz_theDirectory[_MAX_PATH];
	TCHAR sz_theFile[_MAX_PATH];
	_BreakFilename(pTaskObject->m_szFilename,sz_theDirectory,sz_theFile);

	CString subject (pTaskObject->m_szSubject);
	subject.Replace(sz_theFile,"$F");

	GeneratePar2Vol(post,(LPCSTR)subject, pTaskObject->m_strGroups, 0, pTaskObject);
}

// returns the number of created files
int CPostMostView::GeneratePar2Vol(par2::PostFiles *post, LPCSTR subject, const CString& groups, int nFile0, CTaskObject* parentObj)
{
	TRACE("CPostMostView::GeneratePar2Vol()\n");

	if (post->nbfiles<=0 || post->partsize<=0)
	{ // bad file
		AfxMessageBox("Bad .par2 file :-(");
		delete post;
		return 0;
	}

	CPropPage_Add_Subject		PP_Subject;
	CPropPage_Add_Groups		PP_Groups;
	CPropPage_Add_TextPrefix	PP_TextPrefix;
	CPropPage_Add_Recovery      PP_Recovery;

	DWORD dwPageActivation = 0;
	PP_Subject.m_pdwActivation = &dwPageActivation;
	PP_Subject.m_bGroupsOK = TRUE;
	PP_Groups.m_pdwActivation = &dwPageActivation;
	PP_TextPrefix.m_pdwActivation = &dwPageActivation;
	PP_Recovery.m_pdwActivation = &dwPageActivation;

	CStringArray listFileNames;

	//listFileNames.Add(CString(pTaskObject->m_szFilename));

	PP_Subject.m_nDir   = 0;
	PP_Subject.m_pDroppedFiles = &listFileNames;
	PP_Subject.m_bInsertFilesAtBeginningOfQueue = FALSE;
	PP_Subject.m_bProcessSubdirectories = FALSE;
	PP_Subject.m_bRecursive = FALSE;
	PP_Subject.m_bCountDirSeperate = FALSE;

	PP_Subject.m_strSubjectPrefixTemplate = (LPCTSTR) subject;

	PP_Groups.m_pSettings = &m_Settings;
	PP_Groups.m_strGroupList = groups;

	//PP_Recovery.m_pDWA_FileNames = &listFileNames;
	PP_Recovery.m_sPAR2=post->basename;
	PP_Recovery.m_bGroupThread = m_Settings.m_bFileThread;
	PP_Recovery.m_iPartSize = post->partsize;
	PP_Recovery.m_iSourceChunks=0;
	PP_Recovery.m_iSourceFiles=0;
	PP_Recovery.m_iSourceSize=0;
	PP_Recovery.m_iMaxPartsPerFile=0;
	for (int f=0;f<post->nbfiles;f++)
	{
		if (post->files[f].flags&par2::PostFiles::SELECTED)
		{
			PP_Recovery.m_iSourceFiles++;
			PP_Recovery.m_iSourceChunks+=post->files[f].nbparts;
			PP_Recovery.m_iSourceSize+=(int)post->files[f].size;
			if (post->files[f].nbparts>PP_Recovery.m_iMaxPartsPerFile)
				PP_Recovery.m_iMaxPartsPerFile=post->files[f].nbparts;
		}
	}
	PP_Recovery.m_iFirstChunks=post->nbrecvpart;
	PP_Recovery.m_iNbChunks=PP_Recovery.m_iSourceChunks/10;
	PP_Recovery.m_bSameSize=FALSE;

	CPropertySheet PS;
	PS.SetTitle("Generating .par2 volumes");
	PS.AddPage(&PP_Recovery);
	PS.AddPage(&PP_Subject);
	PS.AddPage(&PP_Groups);
	PS.AddPage(&PP_TextPrefix);
	//if((listFileNames.GetSize() > 0) && (listDir.GetSize() == 0))
	//	PS.AddPage(&PP_FileOrder);

	PS.m_psh.dwFlags |= PSH_NOAPPLYNOW;
/*
	// Setup Example with something real if we can...
	if(listFileNames.GetSize() > 1)
	{
		// strcpy(PP_Subject.m_Ex_szFilename, (LPCTSTR) listFileNames.GetAt(0));
		PP_Subject.m_Ex_nFileCount = listFileNames.GetSize();
	}
*/
	PP_Subject.m_Ex_nFileCount = (nFile0==0)?post->nbfiles+1:nFile0;

	if(PS.DoModal() == IDCANCEL)
	{
		delete post;
		return 0;
	}

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CString progress;

	progress.Format("Creating PAR2 Volume files...");
	pMainFrame->SetStatusText(progress);

		// Calculate the size of each file
		int nbvfiles=0;
		int nbc = PP_Recovery.m_iNbChunks;
		int maxc = (PP_Recovery.m_iMaxPartsPerFile>0?PP_Recovery.m_iMaxPartsPerFile:(nbc>0)?nbc:1);
		int *file_size = new int[nbc]; // at least 1 chunck/file
		if (!PP_Recovery.m_bSameSize)
		{
			int nc = 1;
			while (nbc>=nc && nc<=maxc)
			{
				file_size[nbvfiles]=nc;
				++nbvfiles; nbc-=nc;
				nc*=2;
			}
		}
		while (nbc>maxc)
		{
			file_size[nbvfiles]=maxc;
			++nbvfiles; nbc-=maxc;
		}
		if (nbc>0)
		{
			file_size[nbvfiles]=nbc;
			++nbvfiles;
		}

		par2::PostPar2Volumes* worker;
		worker = new par2::PostPar2Volumes(post,PP_Recovery.m_iFirstChunks,nbvfiles,file_size);

		delete[] file_size;

	if (!worker->Start())
	{
		progress.Format("Creating PAR2 Volume files FAILED at Start :-(");
		pMainFrame->SetStatusText(progress);
		delete worker;
		delete post;
		return 0;
	}
/*
	for (int i=0;i<post->nbfiles;i++)
	{
		progress.Format("Processing file %s...",post->files[i].name);
		pMainFrame->SetStatusText(progress);
		if (!worker->ProcessFile(i))
		{
			
			progress.Format("Error while processing file %s",post->files[i].name);
			pMainFrame->SetStatusText(progress);
			AfxMessageBox(progress);

			if(m_Settings.m_bSound) PlaySound((LPCTSTR) IDR_WAVE_VAULT, NULL, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
			delete worker;
			delete post;
			return;
		}
	}

	progress.Format("Finishing PAR2 Volume files creation...");
	pMainFrame->SetStatusText(progress);

	if (!worker->Finish())
	{
		progress.Format("Creating PAR2 Volume files FAILED at Finish :-(");
		pMainFrame->SetStatusText(progress);
		if(m_Settings.m_bSound) PlaySound((LPCTSTR) IDR_WAVE_VAULT, NULL, SND_ASYNC | SND_RESOURCE | SND_NOSTOP);
		delete worker;
		delete post;
		return;
	}
*/

	int partsize = worker->GetPacketLength();

	CString strSubjectPrefixTemplate = (LPCTSTR) PP_Subject.m_strSubjectPrefixTemplate;
	BOOL bInsertFilesAtBeginningOfQueue = PP_Subject.m_bInsertFilesAtBeginningOfQueue;

	LPCTSTR lpszTextPrefix = (LPCTSTR) PP_TextPrefix.m_strTextPrefix;
	BOOL bAddPrefixText = PP_TextPrefix.m_bSendTextPrefix;
	BOOL bAddPrefixTextFirstFileOnly = PP_TextPrefix.m_bSendTextFirstFileOnly;
	LPCTSTR lpszGroupList = (LPCTSTR) PP_Groups.m_strGroupList;
	BOOL bGroupThread = PP_Recovery.m_bGroupThread;


	//progress.Format("PAR2 Volume files creation DONE!");
	//pMainFrame->SetStatusText(progress);
	pMainFrame->SetStatusText("");

	int taskstate = CTaskObject::PM_ENC_QUEUED;

	if (m_pEncodingThread!=NULL && m_pEncodingThread->isReady())
	{
		taskstate = CTaskObject::PM_ENCODING;
		m_pEncodingThread->sendTask(worker);
	}

	// Process File list
	int nInsertPoint;

	if(!bAddPrefixText) lpszTextPrefix=NULL;

	int kk;
	int nItem;
	int nVolFiles = worker->nb_vol_files;
	{
		CWaitCursor wc;
		nItem = listNbTasks(); //listCtrl.GetItemCount();
		//CTaskObject* parentObj=pTaskObject;
		CTaskObject* currentObj;
		// Now Process The Files...
		int kAdded = 0;

		// Determine point in list to insert new item
		if(bInsertFilesAtBeginningOfQueue)
			nInsertPoint = 0; // TODO: nFile0 ?
		else
			nInsertPoint = nItem;

		// Number them all as one batch
		for(kk = 0; kk < worker->nb_vol_files; kk++)
		{	

			currentObj = InsertFileIntoQueue(
				(LPCTSTR) worker->vol_files_name[kk],
				(LPCTSTR) strSubjectPrefixTemplate,
				nInsertPoint+kk,
				nFile0+kk+1,
				nFile0+nVolFiles,
				(((!bAddPrefixTextFirstFileOnly)||(kk==0))?lpszTextPrefix:NULL),
				lpszGroupList,
				taskstate,
				worker->vol_files_size[kk]*worker->GetPacketLength()+worker->sources->par2common_size,
				partsize
				);
			worker->vol_files_priv[kk]=currentObj;

			if (parentObj!=NULL)
			{
				int parentParts = parentObj->getTotalParts();
				parentObj->GenerateMessageID(m_Settings.m_szDomainID);
				currentObj->setReferences(parentObj->getReferences(TRUE,parentParts));
			}
			else if (bGroupThread)
				parentObj = currentObj;
		}
	}

	if (taskstate == CTaskObject::PM_ENC_QUEUED)
	{
		delete worker;
		delete post;
	}

	// Compute entire batch size
	m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());

	return nVolFiles;
}

void CPostMostView::OnUpdateEncodeStart(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pEncodingThread==NULL);
	pCmdUI->SetCheck(m_pEncodingThread!=NULL);
}

void CPostMostView::OnUpdateEncodeStop(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pEncodingThread!=NULL);
}

void CPostMostView::OnUpdatePostEncodes(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);	
	pCmdUI->SetCheck(m_pEncodeDlg!=NULL);
}

void CPostMostView::OnPostEncodes() 
{
	if( m_pEncodeDlg != NULL)
	{
		if(m_pEncodeDlg != NULL)
		{
			m_pEncodeDlg->SendMessage(WM_CLOSE);
			delete m_pEncodeDlg;
			m_pEncodeDlg = NULL;
		}
	}
	else
	{
		RECT rectEncodingButton;
		CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
		pMainFrame->GetButtonRect(ID_POST_ENCODES,&rectEncodingButton);

		if(m_pEncodeDlg == (CEncodingProgressDlg*) NULL)
		{
			m_pEncodeDlg = new CEncodingProgressDlg(this);
			m_pEncodeDlg->m_rectAnimateFrom = rectEncodingButton;
			m_pEncodeDlg->m_pView = this;
			if(!m_pEncodeDlg->Create(IDD_DIALOG_ENCODING, this))
			{
				delete m_pEncodeDlg;
				AfxMessageBox("Error Creating Encoding Progress Window!", MB_ICONSTOP);
				return;
			}
			m_pEncodeDlg->SetActiveWindow();
		}
		else
		{
		}
	}
}

void CPostMostView::OnEncodeStart() 
{
	if (m_pEncodingThread!=NULL) return; // already started

	m_pEncodingThread = CEncoderThread::Create(this,&m_Settings);

}

void CPostMostView::OnEncodeStop() 
{
	if (m_pEncodingThread!=NULL)
		m_pEncodingThread->STOP();
}

LRESULT CPostMostView::OnEncode_Ready(WPARAM wParam, LPARAM lParam)
{
	TRACE("CPostMostView::OnEncode_Ready()\n");
	//if (wParam!=(WPARAM)m_pEncodingThread) return (0);
	if (m_pEncodingThread == NULL || !m_pEncodingThread->isReady()) return 0;

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	//CTaskObject* pTaskObject = (CTaskObject*) lParam;

	//pMainFrame->SetProgressIndicator("");

	// get the first enc_queued task

	CTaskObject* firstTask = NULL;

	par2::PostFiles* postfiles = NULL;

	CTaskObject* pTaskObject;

	CPtrArray taskArray;
	CStringArray filenameArray;

	int nItems = listNbTasks();
	int kk;
	for(kk = 0; kk < nItems; kk++)
	{
		pTaskObject = listGetTask(kk);

		if(pTaskObject->m_nState == CTaskObject::PM_ENC_QUEUED)
		{
			if (postfiles==NULL)
			{
				firstTask = pTaskObject;
				postfiles = new par2::PostFiles(CString(firstTask->m_szFilename)+CString(".temp"));
				taskArray.Add(pTaskObject);
				filenameArray.Add(pTaskObject->m_szFilename);
			}
			else
			{
				par2::hash16 setid;
				if (par2::PostFiles::GetSetID(CString(pTaskObject->m_szFilename)+CString(".temp"),setid))
				{
					if (par2::hash_compare(&setid,&(postfiles->par2_setid))==0)
					{
						taskArray.Add(pTaskObject);
						filenameArray.Add(pTaskObject->m_szFilename);
					}
				}
			}
		}
	}

	if (postfiles == NULL) return 0;

	void** privarray = new void*[taskArray.GetSize()];
	for (kk=0;kk<taskArray.GetSize();kk++)
		privarray[kk]=taskArray.GetAt(kk);

	par2::PostPar2Volumes * par2vols = new par2::PostPar2Volumes(postfiles,&filenameArray,privarray);
	delete[] privarray;

	for (kk=0;kk<taskArray.GetSize();kk++)
	{
		pTaskObject = (CTaskObject*) taskArray.GetAt(kk);
		if (!strcmp(pTaskObject->m_szFilename,filenameArray.GetAt(kk)))
		{ // file ok
			pTaskObject->m_nState = CTaskObject::PM_ENCODING;
		}
		else
		{
			pTaskObject->m_Log.Add(filenameArray.GetAt(kk));
			strncpy(pTaskObject->m_szLastResult,filenameArray.GetAt(kk),sizeof(pTaskObject->m_szLastResult));
			pTaskObject->m_nState = CTaskObject::PM_ERROR;
		}
		UpdateDisplay(pTaskObject,filenameArray.GetAt(kk));
	}
/*
	if (m_pEncodingThread == NULL || !m_pEncodingThread->isReady())
	{
		On_EncodeStop();
	}
	else*/
	{
		m_pEncodingThread->sendTask(par2vols);
	}

	m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());
	return(0);
}

LRESULT CPostMostView::OnEncode_Stopped(WPARAM wParam, LPARAM lParam)
{
	TRACE("CPostMostView::OnEncode_Stopped()\n");
	CEncoderThread* encoder = (CEncoderThread*)wParam;
	par2::PostPar2Volumes* task = encoder->getTask();
	encoder->clearTask();
	if (task!=NULL)
	{
		for (int f=0;f<task->nb_vol_files;f++)
		{
			CTaskObject* fobj = (CTaskObject*)task->vol_files_priv[f];
			if (task->isok())
			{
				strncpy(fobj->m_szLastResult,"Encoding OK.",1024);
				fobj->m_Log.Add(fobj->m_szLastResult);
				fobj->m_nState = CTaskObject::PM_QUEUED;
			}
			else
			{
				strncpy(fobj->m_szLastResult,task->errorMsg,1024);
				fobj->m_Log.Add(task->errorMsg);
				fobj->m_nState = CTaskObject::PM_ENC_QUEUED;
			}
			UpdateDisplay(fobj,fobj->m_szLastResult);
		}
		delete task->sources;
		delete task;
	}
	if (m_pEncodingThread == encoder)
		m_pEncodingThread = NULL;
	encoder->Delete();
	m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());
	return 0;	
}

LRESULT CPostMostView::OnEncode_Done(WPARAM wParam, LPARAM lParam)
{
	TRACE("CPostMostView::OnEncode_Done()\n");
	CEncoderThread* encoder = (CEncoderThread*)wParam;
	par2::PostPar2Volumes* task = (par2::PostPar2Volumes*)lParam;
	if (task!=NULL)
	{

		for (int f=0;f<task->nb_vol_files;f++)
		{
			CTaskObject* fobj = (CTaskObject*)task->vol_files_priv[f];
			strncpy(fobj->m_szLastResult,task->errorMsg,1024);
			fobj->m_Log.Add(task->errorMsg);
			if (task->isok())
			{
				fobj->m_nState = CTaskObject::PM_QUEUED;
			}
			else
			{
				fobj->m_nState = CTaskObject::PM_ENC_QUEUED;
			}
			UpdateDisplay(fobj,fobj->m_szLastResult);
		}

		delete task->sources;
		delete task;
	}
	m_ThreadMarshal.SetBatchSize(__ComputeBatchSize());
	return 0;	
}
