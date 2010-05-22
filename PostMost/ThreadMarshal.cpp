// ThreadMarshal.cpp: implementation of the CThreadMarshal class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PostMost.h"
#include "ThreadMarshal.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThreadMarshal::CThreadMarshal()
{
	int i;
	m_bSTOPPINGALL = FALSE;
	for (i=0;i<NPOSTTHREAD;i++)
	{
	  m_pPosterThread[i] = (CPosterThread*) NULL;
	  m_iPercentProgressFile[i] = 0;
	  m_iPercentProgressArticle[i] = 0;
	  m_nCurrentPart[i] = 0;
	  m_nCurrentPartOf[i] = 0;
	  m_szCurrentTask[i][0] = '\0';
		m_nBatchTemporaryPos[i] = 0;
	}
	m_nBatchSize = 0;
	m_nBatchGlobalPos = 0;

	::ZeroMemory(m_nByteTransmissionHistory300, (300 * sizeof(TransmissionStats)));
	m_nByteTransmissionLastUpdate = time(NULL); //CTime::GetCurrentTime();

}

CThreadMarshal::~CThreadMarshal()
{

}

BOOL CThreadMarshal::GetLock()
{
	// Lock Critical Section
	if(m_bSTOPPINGALL == TRUE)
		return(FALSE);
	m_CriticalSection.Lock();
	return(TRUE);
}

void CThreadMarshal::ReleaseLock()
{
	m_CriticalSection.Unlock();
}

void CThreadMarshal::SetBatchSize(ULONGLONG nSizeBatch)
{
	m_nBatchSize = nSizeBatch;
	//m_nBatchPos = 0;
}
void CThreadMarshal::SetTemporaryPos(UINT iThread, UINT nSize)
{
	if(GetLock() == TRUE)
	{
		m_nBatchTemporaryPos[iThread] = nSize;
		ReleaseLock();
	}
}

void CThreadMarshal::FlushTemporaryPos(UINT iThread, UINT nSize)
{
	if(GetLock() == TRUE)
	{
		m_nBatchSize -= nSize;
		m_nBatchGlobalPos += nSize;
		m_nBatchTemporaryPos[iThread] = 0;
		ReleaseLock();
	}
}

void CThreadMarshal::SetProgramSettings(CProgramSettings& theSettings)
{
	if(m_bSTOPPINGALL == TRUE) return;
	m_Settings.Copy(theSettings);
}

BOOL CThreadMarshal::Start_AllThread(HWND hWndNotify)
{
	m_nBatchGlobalPos=0;
	for (int i=0;i<m_Settings.m_nConnections;i++)
	{
		m_nBatchTemporaryPos[i]=0;
		// Return FALSE if already started
		if(m_pPosterThread[i] != NULL)
			return FALSE;

		if(GetLock() == FALSE)
			return FALSE;

		// Start Thread
		m_pPosterThread[i] = (CPosterThread*) AfxBeginThread( RUNTIME_CLASS(CPosterThread), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED, NULL );
		if(m_pPosterThread[i] != NULL)
		{
			m_pPosterThread[i]->m_hWndNotify = hWndNotify;
			m_pPosterThread[i]->m_Settings.Copy(m_Settings);
			m_pPosterThread[i]->m_pThreadMarshal = this;
			m_pPosterThread[i]->m_bAutoDelete = TRUE;
			m_pPosterThread[i]->m_iThread = i;
			m_pPosterThread[i]->ResumeThread();
		}

		// Unlock Critical Section
		ReleaseLock();

		if(m_pPosterThread[i] == (CPosterThread*) NULL)
			return FALSE;
	}
	return TRUE;
}

BOOL CThreadMarshal::Stop_AllThread()
{
	TRACE("CThreadMarshal::Stop_AllThread()\n");
	int i;
	// Enter Critical Section
	if(GetLock() == FALSE)
	{
		TRACE("CThreadMarshal::Stop_AllThread() - ERROR GETTING LOCK!\n");
		return FALSE;
	}
	

	m_bSTOPPINGALL = TRUE;



	// Signal Thread To Stop
	for (i=0;i<NPOSTTHREAD;i++)
	{
  	if(m_pPosterThread[i] != NULL)
		{
			m_pPosterThread[i]->m_STOP = TRUE;
		
			if(m_pPosterThread[i]->m_hSocket != 0)
			{
				closesocket(m_pPosterThread[i]->m_hSocket);
				m_pPosterThread[i]->m_hSocket = 0;
			}
			m_pPosterThread[i]->m_bAutoDelete = FALSE;
			::PostThreadMessage(m_pPosterThread[i]->m_nThreadID, WM_MSG_QUIT, (WPARAM) 0, (LPARAM) 0);
		}
	}
	ReleaseLock();

	for (i=0;i<NPOSTTHREAD;i++)
	{
 		if(m_pPosterThread[i] != NULL)
		{
			::WaitForSingleObject(m_pPosterThread[i]->m_hThread, INFINITE);

			m_pPosterThread[i] = NULL;

			delete m_pPosterThread[i];
		}
	}
	m_bSTOPPINGALL = FALSE;

	// Return Success
	return TRUE;
}

void CThreadMarshal::Assign_Task(int iThread, CTaskObject* pTaskObject)
{
	if(m_pPosterThread[iThread] != NULL)
	{
		// Lock Critical Section
		if(GetLock() == FALSE)
			return;

		m_strCurrentTargetNewsgroups[iThread].Empty();

		//m_pPosterThread->m_pTaskObject = pTaskObject;

		if(pTaskObject != NULL && m_pPosterThread[iThread] != NULL)
			m_pPosterThread[iThread]->PostThreadMessage(WM_MSG_POST, 0, (LPARAM)pTaskObject);

		// Exit Critical Section
		ReleaseLock();
	}

}

void CThreadMarshal::SetProgress(int iThread, int nPercentFile, int nPercentArticle)
{
	if(m_bSTOPPINGALL == TRUE) return;

	// Lock Critical Section
	//if(GetLock() == FALSE)
		//return;

	m_iPercentProgressFile[iThread] = nPercentFile;
	m_iPercentProgressArticle[iThread] = nPercentArticle;

	// Exit Critical Section
	//ReleaseLock();

}

BOOL CThreadMarshal::GetProgress(int iThread, char* szFile, int& nPosFile, char* szArticle, int& nPosArticle, int& nPart, int& nPartOf)
{
	if(m_bSTOPPINGALL == TRUE) return(FALSE);

	if(m_pPosterThread[iThread] != NULL)
	{
		// Lock Critical Section
		if(GetLock() == FALSE)
			return FALSE;

		if(m_pPosterThread[iThread]->m_pTaskObjectREAD != NULL)
		{
			nPosFile = m_iPercentProgressFile[iThread];
			nPosArticle = m_iPercentProgressArticle[iThread];

			strcpy(szFile, m_pPosterThread[iThread]->m_pTaskObjectREAD->m_szFilename);
			strcpy(szArticle, m_szCurrentTask[iThread]);

			nPart = m_nCurrentPart[iThread];
			nPartOf = m_nCurrentPartOf[iThread];

		}
		else
		{
			nPosFile = 0;
			nPosArticle = 0;
			strcpy(szFile, "");
			strcpy(szArticle, "");
			nPart = 0;
			nPartOf = 0;
		}

		// Exit Critical Section
		ReleaseLock();

		return(TRUE);
	}

	return(FALSE);
}


BOOL CThreadMarshal::GetGlobalProgress(UINT& nBatchSize, UINT& nBatchPos)
{
	//if(m_bSTOPPINGALL == TRUE) return(FALSE);

	// Lock Critical Section
	if(GetLock() == FALSE)
		return FALSE;

	nBatchSize = m_nBatchSize+m_nBatchGlobalPos;
	UINT pos = m_nBatchGlobalPos;
	for (int i=0;i<NPOSTTHREAD;i++)
	{
		pos+=m_nBatchTemporaryPos[i];
	}

	nBatchPos = pos;

	// Exit Critical Section
	ReleaseLock();

	return(TRUE);
}

BOOL CThreadMarshal::GetResult(int iThread, int& bOK, char* szResult)
{
	if(m_bSTOPPINGALL == TRUE) return(FALSE);

	// Lock Critical Section
	if(GetLock() == FALSE)
		return(FALSE);

	if(m_pPosterThread[iThread] != NULL)
	{

		bOK = m_pPosterThread[iThread]->Task_Result_OK;
		strcpy(szResult, m_pPosterThread[iThread]->Task_Result);

		// Exit Critical Section
		ReleaseLock();

		return(TRUE);
	}
	else
	{
		strcpy( szResult, "STOPPED");
		bOK = TRUE;
		ReleaseLock();
		return(FALSE);
	}
}

LPCTSTR CThreadMarshal::GetServerConnectString(int iThread)
{
	if(m_pPosterThread[iThread] != NULL)
		return((LPCTSTR) m_pPosterThread[iThread]->m_NNTP_ConnectString);
	else
		return NULL;
}

void CThreadMarshal::Reconnect(int iThread)
{
	if(m_pPosterThread[iThread] != NULL)
		::PostThreadMessage(m_pPosterThread[iThread]->m_nThreadID, WM_MSG_CONNECT, (WPARAM) 0, (LPARAM) 0);

}

void CThreadMarshal::SetCurrentArticleStatus(int iThread, LPCTSTR szArticle, int nPart, int nPartOf)
{
	// Lock Critical Section
	if(GetLock() == FALSE) return;

	m_nCurrentPart[iThread] = nPart;
	m_nCurrentPartOf[iThread] = nPartOf;
	strcpy(m_szCurrentTask[iThread], szArticle);

	// Exit Critical Section
	ReleaseLock();
}

void CThreadMarshal::GetCurrentArticleStatus(int iThread, TCHAR* szArticle320, int* nPart, int* nPartOf)
{
	// Lock Critical Section
	if(GetLock() == FALSE)
	{
		szArticle320[0] = '\0';
		*nPart = 0;
		*nPartOf = 0;
		return;
	}

	strcpy(szArticle320, m_szCurrentTask[iThread]);
	*nPart = m_nCurrentPart[iThread];
	*nPartOf = m_nCurrentPartOf[iThread];

	// Exit Critical Section
	ReleaseLock();
}

// m_strCurrentTargetNewsgroups
void CThreadMarshal::SetCurrentTargetNewsgroups(int iThread, LPCTSTR szNewsgroups)
{
	if(GetLock() == FALSE) return;
	m_strCurrentTargetNewsgroups[iThread] = szNewsgroups;
	ReleaseLock();
}

void CThreadMarshal::GetCurrentTargetNewsgroupsToWindow(int iThread, CWnd* pWnd)
{
	if(pWnd != NULL)
	{
		if(GetLock() == FALSE) { pWnd->SetWindowText("(Lock Error)"); return; }

		pWnd->SetWindowText((LPCTSTR) m_strCurrentTargetNewsgroups[iThread]);

		// Exit Critical Section
		ReleaseLock();
	}

}

void CThreadMarshal::TransmissionNotify(int iThread, int nBytes, time_t t)
{
	if(GetLock() == FALSE) return;

	AppleNewByteTransmission(iThread, nBytes, t);

	// Exit Critical Section
	ReleaseLock();
}

BOOL CThreadMarshal::GetTransmissionHistoryTotal(int* pInt_300) // 300 seconds worth
{
	if(GetLock())
	{
		// Update with 0 bytes to force current time usage
		AppleNewByteTransmission(0,0,time_t(NULL));

		// Return Data Array
		for(int k = 0; k < 300; k++)
		{
			int total=0;
			for (int iThread=0;iThread<NPOSTTHREAD;iThread++)
				total+=m_nByteTransmissionHistory300[k].thread[iThread];
			pInt_300[k] = total;
		}
		// Exit Critical Section
		ReleaseLock();
		return(TRUE);
	}

	return(FALSE);
}

BOOL CThreadMarshal::GetTransmissionHistoryThreaded(TransmissionStats* pInt_300) // 300 seconds worth
{
	if(GetLock())
	{
		// Update with 0 bytes to force current time usage
		AppleNewByteTransmission(0,0,time_t(NULL));

		memcpy(pInt_300,m_nByteTransmissionHistory300,(300*sizeof(TransmissionStats)));
		/*
		// Return Data Array
		for(int k = 0; k < 300; k++)
		{
			int total=0;
			for (int iThread=0;iThread<NPOSTTHREAD;iThread++)
				total+=m_nByteTransmissionHistory300[k].thread[iThread];
			pInt_300[k] = total;
		}
		*/
		// Exit Critical Section
		ReleaseLock();
		return(TRUE);
	}

	return(FALSE);
}

// This function applies nBytes to the transmission history;
// Use nBytes = 0 to force the history to be 'current'
void CThreadMarshal::AppleNewByteTransmission(int iThread, int nBytes, time_t timeNow)
{
	// CTime m_nByteTransmissionLastUpdate;
	// int   m_nByteTransmissionHistory300[300];
	int k;
	//time_t timeNow = time(NULL);
	int nSeconds = timeNow - m_nByteTransmissionLastUpdate;
	if (nSeconds > 0)
		m_nByteTransmissionLastUpdate = timeNow;
	if (nSeconds <= -300) return;
/*
	if(nSeconds < 0)
	{
		nSeconds = 0; //nSeconds * -1;
	}
*/
	// Handle extreme case
	if(nSeconds > 300)
	{
		// More than 300 seconds went by.. zero everything and apply nBytes to [0] pos
		::ZeroMemory(m_nByteTransmissionHistory300, (300 * sizeof(TransmissionStats)));
		//m_nByteTransmissionHistory300[0].total = nBytes;
		m_nByteTransmissionHistory300[0].thread[iThread]=nBytes;
		return;
	}

	// Handle Trivial Case
	if(nSeconds <= 0)
	{
		m_nByteTransmissionHistory300[-nSeconds].thread[iThread] += nBytes;
		return;
	}

	// shift history by number of seconds
	for(k = 299; k >= nSeconds; k--)
	{
		m_nByteTransmissionHistory300[k] = m_nByteTransmissionHistory300[k - nSeconds];
	}
	::ZeroMemory(m_nByteTransmissionHistory300, (nSeconds * sizeof(TransmissionStats)));
	m_nByteTransmissionHistory300[0].thread[iThread] = nBytes;

#ifdef _DEBUG
		TRACE("Bytes/Sec : %7d - %7d - %7d - ", m_nByteTransmissionHistory300[0].thread[iThread], m_nByteTransmissionHistory300[1].thread[iThread], m_nByteTransmissionHistory300[2].thread[iThread]);
		TRACE("%7d - %7d - %7d - ", m_nByteTransmissionHistory300[3].thread[iThread], m_nByteTransmissionHistory300[4], m_nByteTransmissionHistory300[5].thread[iThread]);
		TRACE("%7d - %7d - %7d\n", m_nByteTransmissionHistory300[6].thread[iThread], m_nByteTransmissionHistory300[7], m_nByteTransmissionHistory300[8].thread[iThread]);
#endif

	return;
}

void CThreadMarshal::UpdateCap(int BWCap)
{
	int i;
	m_Settings.m_nBandwidthCap = BWCap;
	for (i=0;i<NPOSTTHREAD;i++)
	{
		if (m_pPosterThread[i]!=NULL) m_pPosterThread[i]->UpdateCap(BWCap);
	}
}
