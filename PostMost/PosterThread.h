#if !defined(AFX_POSTERTHREAD_H__CAC7719A_C4FF_11D2_B7DD_00A0CC215BD8__INCLUDED_)
#define AFX_POSTERTHREAD_H__CAC7719A_C4FF_11D2_B7DD_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PosterThread.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPosterThread thread

class CThreadMarshal;

class CPosterThread : public CWinThread
{
	DECLARE_DYNCREATE(CPosterThread)
protected:
	CPosterThread();           // protected constructor used by dynamic creation

// Attributes
public:
	int               m_hSocket;
	HWND			  m_hWndNotify;
	CThreadMarshal*   m_pThreadMarshal;
	BOOL              m_STOP;
	CProgramSettings  m_Settings;

	CTaskObject*      m_pTaskObjectREAD;

	BOOL   Task_Result_OK;

	// char   Task_File[1024];
	// char   Task_Title[1024];
	
	// char   Task_Article[1024];
	char   Task_Result[1024];

	char   m_NNTP_ConnectString[1024];

// Operations
public:
	int SendCapped(const TCHAR* buffer, int n);
	void UpdateCap(int BWCap);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPosterThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	int m_iThread;
// Implementation
	virtual ~CPosterThread();
protected:
	int m_nSendBufferSize;
	int m_nSendBufferPos;
	TCHAR m_szSendBuffer[8400];

	CString m_szVersionName;
	CString m_szVersionInfo;

	void SetTask_Article(char* szArticle, int nPart, int nPartOf);
	void SetTask_Result(BOOL bSuccessful, char* szResult, CTaskObject* pTaskObject);

	int  NNTP_GetLine(char* p, int MaxChar);
	int  NNTP_SendLine(const char* p);
	int  NNTP_FlushOutputBuffer();
	BOOL NNTP_ConnectServer(char* ServerName, int nPort, char* rvConnectString);
	BOOL NNTP_DisconnectServer();

	void SendHeaders(LPCTSTR szArticleSubject, BOOL bIncludeTime, CTime& theServerTime, CTaskObject* pTaskObject, int partNum, int nParts, CString& postok);

	void _BreakFilename(LPCTSTR FullFilename, CString& Path, CString& File);

	BOOL ParseServerDate(CTime& rvTime, LPCTSTR szDATE);

	// Generated message map functions
	//{{AFX_MSG(CPosterThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg void OnMessage_POST(WPARAM wParam, LPARAM lParam);
	void OnMessage_POST_in(WPARAM wParam, CTaskObject* pTaskObject);
	afx_msg void OnMessage_QUIT(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMessage_CONNECT(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	BOOL Authenticate(char* rvConnectString);

  void SentCAP(int it, int n);
  int CalcCAP(int it, int n);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSTERTHREAD_H__CAC7719A_C4FF_11D2_B7DD_00A0CC215BD8__INCLUDED_)
