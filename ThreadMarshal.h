// ThreadMarshal.h: interface for the CThreadMarshal class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THREADMARSHAL_H__CAC77199_C4FF_11D2_B7DD_00A0CC215BD8__INCLUDED_)
#define AFX_THREADMARSHAL_H__CAC77199_C4FF_11D2_B7DD_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define NPOSTTHREAD 4

struct TransmissionStats
{
  //int total;
  int thread[NPOSTTHREAD];
};

class CThreadMarshal  
{
public:
	void UpdateCap(int BWCap);
	CThreadMarshal();
	virtual ~CThreadMarshal();

	BOOL Start_Thread(int iThread, HWND hWndNotify);
	BOOL Start_AllThread(HWND hWndNotify);
	BOOL Stop_Thread(int iThread);
	BOOL Stop_AllThread();

	void Assign_Task(int iThread, CTaskObject* pTaskObject);
	
	void SetBatchSize(UINT nSizeBatch);
	void SetTemporaryPos(UINT iThread, UINT nSize);
	void FlushTemporaryPos(UINT iThread, UINT nSize);

	void SetProgress(int iThread, int nPercentFile, int nPercentArticle);
	
	void TransmissionNotify(int iThread, int nBytes, time_t t);
	BOOL GetTransmissionHistoryTotal(int* pInt_300); // 300 seconds worth
	BOOL GetTransmissionHistoryThreaded(TransmissionStats* pInt_300); // 300 seconds worth
	
	BOOL GetProgress(int iThread, char* szFile, int& nPosFile, char* szArticle, int& nPosArticle, int& nPart, int& nPartOf);
	BOOL GetGlobalProgress(UINT& nBatchSize, UINT& nBatchPos);

	BOOL GetResult(int iThread, int& bOK, char* szResult);

	void SetProgramSettings(CProgramSettings& theSettings);

	BOOL GetLock();
	void ReleaseLock();

	LPCTSTR GetServerConnectString(int iThread);

	void Reconnect(int iThread);

	void SetCurrentArticleStatus(int iThread, LPCTSTR szArticle, int nPart, int nPartOf);
	void GetCurrentArticleStatus(int iThread, TCHAR* szArticle320, int* nPart, int* nPartOf);

	// m_strCurrentTargetNewsgroups
	void SetCurrentTargetNewsgroups(int iThread, LPCTSTR szNewsgroups);
	void GetCurrentTargetNewsgroupsToWindow(int iThread, CWnd* pWnd);

	CProgramSettings m_Settings;
private:
	void AppleNewByteTransmission(int iThread, int nBytes, time_t t);

	time_t m_nByteTransmissionLastUpdate;
	TransmissionStats m_nByteTransmissionHistory300[300];

	BOOL			 m_bSTOPPINGALL;
	CCriticalSection m_CriticalSection;
	CPosterThread*   m_pPosterThread[NPOSTTHREAD];
	int              m_iPercentProgressFile[NPOSTTHREAD];
	int              m_iPercentProgressArticle[NPOSTTHREAD];
	UINT             m_nBatchSize;
	UINT             m_nBatchGlobalPos;
	UINT             m_nBatchTemporaryPos[NPOSTTHREAD];
	int              m_nCurrentPart[NPOSTTHREAD];
	int              m_nCurrentPartOf[NPOSTTHREAD];
	TCHAR            m_szCurrentTask[NPOSTTHREAD][320];
	CString          m_strCurrentTargetNewsgroups[NPOSTTHREAD];
};

#endif // !defined(AFX_THREADMARSHAL_H__CAC77199_C4FF_11D2_B7DD_00A0CC215BD8__INCLUDED_)
