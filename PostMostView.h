// PostMostView.h : interface of the CPostMostView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_POSTMOSTVIEW_H__1E48643D_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
#define AFX_POSTMOSTVIEW_H__1E48643D_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WM_FILES_DROPPED	(WM_USER + 1000)

#define IMAGE_QUEUE			0
#define IMAGE_POSTING		1
#define IMAGE_DONE			2
#define IMAGE_ERROR         3
#define IMAGE_HOLDING       4
#define IMAGE_ENC_QUEUE     5
#define IMAGE_ENCODING      6

int TaskState2Image(int State);

class CPostingProgressDlg;
class CEncodingProgressDlg;
class CEncoderThread;

namespace par2
{
	class PostFiles;
};

class CPostMostView : public CListView
{
protected: // create from serialization only
	CPostMostView();
	DECLARE_DYNCREATE(CPostMostView)

// Attributes
public:
	CPostMostDoc* GetDocument();

	CProgramSettings	 m_Settings;
	BOOL				 m_bRunning;
	BOOL                 m_bWaiting;
	UINT                 m_nWaitTimerID;
	CTime                m_Time_Started;
	CTime                m_TimeToStart;
	CTime                m_TimeToStartEnd;
	CThreadMarshal       m_ThreadMarshal;
	CPostingProgressDlg* m_pProgressDlg;
	CEncodingProgressDlg* m_pEncodeDlg;
	BOOL                 m_bWaiting2;
	BOOL				         m_bRepeat;

	CEncoderThread*      m_pEncodingThread;
	
	// Drag & Drop Support
	CStringArray	m_Dropped_Files;
	static CLIPFORMAT m_cfObjectDescriptor;

	BOOL			m_bInDrag;				// With heels!
	COleDropTarget  m_DropTarget;
	DROPEFFECT		m_prevDropEffect;
	CRect			m_DragRect;
	CPoint			m_dragPoint;
	CSize			m_dragSize;
	CSize			m_dragOffset;

// Operations
public:
	void _BreakFilename(LPCTSTR FullFilename, CString& Path, CString& File);
	void _BreakFilename(LPCTSTR FullFilename, char* szDir, char* szFile);
	CTaskObject* InsertFileIntoQueue(LPCTSTR m_Filename, LPCTSTR szSubjectPrefix, int nInsertAt, int nCountValue, int nTotalCount, LPCTSTR szTextPrefix, LPCTSTR szGroupList, int state=CTaskObject::PM_QUEUED, int filesize=0, int partsize=0);
	void GetDirectoryFiles(CStringArray& FileList, LPCTSTR szDirectory, BOOL bRecursive);
	// int GetBatchProgress();

	void SetListControlColors(COLORREF color=RGB(0,255,0), COLORREF bkcolor=RGB(0,0,0));

	//void List_SetItemImageAndState(int nIndex, int nImageTo, int nStateTo);
	//void List_BitmapFromTo(int nImageFrom, int nImageTo, int nStateTo);

	int ViewTaskProperties(CTaskObject* pTaskObject, LPCTSTR szTitle, LPRECT lprectItem);

	void RemoveAllTasks(BOOL bWriteToHistory);
	
	void SaveColumnWidthsToSettings();
	void SaveTasksToQueue();
	void SaveTasksToHistory(CPtrArray& theTasks);
	void LoadTasksFromQueue();
	void AppendTaskToQueue(CTaskObject& theTask);

	int listNbTasks() { return GetListCtrl().GetItemCount(); }
	CTaskObject* listGetTask(int index) { return (CTaskObject*)GetListCtrl().GetItemData(index); }

	void UpdateDisplay(CTaskObject* pTaskObject, const TCHAR* message);
	void UpdateDisplay(int index, CTaskObject* pTaskObject, const TCHAR* message);
	void CheckStillPosting();

	void StartWaitTimer();
	void StopWaitTimer();
	void UpdateWaitTime();

	BOOL __FilesSelected_Not_Q();
	BOOL __FilesInQueue_Q();
	BOOL __FilesInQueue_ERR();
	BOOL __FilesInQueue_OK();
	UINT __ComputeBatchSize();
	BOOL __FilesSelected_Posting();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPostMostView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPostMostView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL m_bPostStopNoSound;

	COLORREF m_Windows_View_Color_BkColor;
	COLORREF m_Windows_View_Color_TextColor;
	COLORREF m_Windows_View_Color_TextBkColor;	

	void UpdateItemDisplay(int nIndex);

	void List_SelectAllItems();
	void List_MoveSelected_Up();
	void List_MoveSelected_Down();
	void List_MoveSelected_Top();
	void List_MoveSelected_Bottom();

	void PostEnded(int iThread, CTaskObject* pTaskObject, BOOL bSTOP);
	void PostStopThread(int iThread, CTaskObject* pTaskObject, BOOL bSTOP);

  int GeneratePar2Vol(par2::PostFiles *post, LPCSTR subject, const CString& groups, int nFile0, CTaskObject* parentObj=NULL);

// Generated message map functions
public:
	//{{AFX_MSG(CPostMostView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdatePostThreads(CCmdUI* pCmdUI);
	afx_msg void OnPostThreads();
	afx_msg void OnUpdatePostStart(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePostStop(CCmdUI* pCmdUI);
	afx_msg void OnPostStop();
	afx_msg void OnPostStart();
	afx_msg void OnUpdatePostSettings(CCmdUI* pCmdUI);
	afx_msg void OnPostSettings();
	afx_msg void OnUpdateFileClearDoneFiles(CCmdUI* pCmdUI);
	afx_msg void OnFileClearDoneFiles();
	afx_msg void OnUpdateFileClearList(CCmdUI* pCmdUI);
	afx_msg void OnFileClearList();
	afx_msg void OnUpdateFileClearErrors(CCmdUI* pCmdUI);
	afx_msg void OnFileClearErrors();
	afx_msg void OnUpdateFileQueueAll(CCmdUI* pCmdUI);
	afx_msg void OnFileQueueAll();
	afx_msg void OnUpdateFileAddFiles(CCmdUI* pCmdUI);
	afx_msg void OnFileAddFiles();
	afx_msg void OnUpdateFileClearSelected(CCmdUI* pCmdUI);
	afx_msg void OnFileClearSelected();
	afx_msg void OnUpdateFileTaskProperties(CCmdUI* pCmdUI);
	afx_msg void OnFileTaskProperties();
	afx_msg void OnUpdateFileHistory(CCmdUI* pCmdUI);
	afx_msg void OnFileHistory();
	afx_msg void OnUpdateFileQueueSel(CCmdUI* pCmdUI);
	afx_msg void OnFileQueueSel();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateFileReassignNewsgroups(CCmdUI* pCmdUI);
	afx_msg void OnFileReassignNewsgroups();
	afx_msg void OnUpdatePostStartAt(CCmdUI* pCmdUI);
	afx_msg void OnPostStartAt();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnUpdateMoveTaskBottom(CCmdUI* pCmdUI);
	afx_msg void OnMoveTaskBottom();
	afx_msg void OnUpdateMoveTaskDown(CCmdUI* pCmdUI);
	afx_msg void OnMoveTaskDown();
	afx_msg void OnUpdateMoveTaskTop(CCmdUI* pCmdUI);
	afx_msg void OnMoveTaskTop();
	afx_msg void OnUpdateMoveTaskUp(CCmdUI* pCmdUI);
	afx_msg void OnMoveTaskUp();
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateFileSetHold(CCmdUI* pCmdUI);
	afx_msg void OnFileSetHold();
	afx_msg void OnUpdateGeneratePar2Vol(CCmdUI* pCmdUI);
	afx_msg void OnGeneratePar2Vol();
	afx_msg void OnUpdateEncodeStart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEncodeStop(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePostEncodes(CCmdUI* pCmdUI);
	afx_msg void OnPostEncodes();
	afx_msg void OnEncodeStart();
	afx_msg void OnEncodeStop();
	//}}AFX_MSG
	afx_msg void OnUpdateIndicatorFiles(CCmdUI* pCmdUI);
	afx_msg LRESULT OnFilesDropped(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnThread_Ready(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnThread_NoConnection(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnThread_ServerNotReady(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnThread_UpdateProgress(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEncode_Ready(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEncode_Stopped(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEncode_Done(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in PostMostView.cpp
inline CPostMostDoc* CPostMostView::GetDocument()
   { return (CPostMostDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSTMOSTVIEW_H__1E48643D_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
