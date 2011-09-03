// PosterThread.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "PosterThread.h"

#include "par2format.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPosterThread

IMPLEMENT_DYNCREATE(CPosterThread, CWinThread)

#define __CM_ENC(c)  ((c) ? ((c) & 077) + ' ' : '`')

par2::CRC32 CRC;
par2::MD5Hash md5hash;

CPosterThread::CPosterThread()
{
	m_hSocket = 0;
	m_hWndNotify = (HWND) NULL;
	m_pThreadMarshal = (CThreadMarshal*) NULL;
	m_pTaskObjectREAD = (CTaskObject*) NULL;
	m_STOP = FALSE;
	Task_Result_OK = FALSE;
	strcpy(m_NNTP_ConnectString, "");
	// strcpy(Task_Article, "");
	strcpy(Task_Result, "");
	m_nSendBufferSize = 8*1024;
	m_nSendBufferPos = 0;
}

CPosterThread::~CPosterThread()
{
}

BOOL CPosterThread::InitInstance()
{
	TRACE("CPosterThread::InitInstance() - Connecting To Server\n");

	m_szVersionName.LoadString(IDS_POSTMOST_VERSION);
	m_szVersionInfo.LoadString(IDS_POSTMOST_VERSION_INFO);
	
	m_pThreadMarshal->SetProgress(m_iThread,0,0);	
	SetTask_Article("Connecting To Server ...", 0, 0);
	
	// Connect to server
	if(NNTP_ConnectServer(m_Settings.m_szServerName, m_Settings.m_nServerPort, m_NNTP_ConnectString) == FALSE)
	{
		SetTask_Result(FALSE, m_NNTP_ConnectString,NULL);

		if(m_hWndNotify != NULL)
		{
			::PostMessage(m_hWndNotify, WM_THREAD_NO_CONNECTION, m_iThread*2, 0);
		}
		return TRUE;
	}
	TRACE("%s\n", m_NNTP_ConnectString);

	if(atoi(m_NNTP_ConnectString) / 100 != 2)
	{
		NNTP_DisconnectServer();
		::PostMessage(m_hWndNotify, WM_THREAD_SERVER_NOT_READY, m_iThread*2, 0);
	}
	else
		::PostMessage(m_hWndNotify, WM_THREAD_READY, m_iThread*2, 0);

	return TRUE;
}

void CPosterThread::OnMessage_CONNECT(WPARAM wParam, LPARAM lParam)
{
	NNTP_DisconnectServer();

	m_pThreadMarshal->SetProgress(m_iThread,0,0);	
	SetTask_Article("Connecting To Server ...", 0, 0);
	
	// Connect to server
	if(NNTP_ConnectServer(m_Settings.m_szServerName, m_Settings.m_nServerPort, m_NNTP_ConnectString) == FALSE)
	{
		strcpy(m_NNTP_ConnectString, "Error Connecting To Server");
		::PostMessage(m_hWndNotify, WM_THREAD_NO_CONNECTION, m_iThread*2, 0);
		return;
	}
	TRACE("%s\n", m_NNTP_ConnectString);

	if(atoi(m_NNTP_ConnectString) / 100 != 2)
		::PostMessage(m_hWndNotify, WM_THREAD_SERVER_NOT_READY, m_iThread*2, 0);
	else
		::PostMessage(m_hWndNotify, WM_THREAD_READY, m_iThread*2, 0);

}

int CPosterThread::ExitInstance()
{
	TRACE("CPosterThread::ExitInstance\n");
	NNTP_DisconnectServer();

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CPosterThread, CWinThread)
	//{{AFX_MSG_MAP(CPosterThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
    ON_THREAD_MESSAGE (WM_MSG_POST, OnMessage_POST)
	ON_THREAD_MESSAGE (WM_MSG_QUIT, OnMessage_QUIT)
	ON_THREAD_MESSAGE (WM_MSG_CONNECT, OnMessage_CONNECT)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPosterThread message handlers

void CPosterThread::SetTask_Article(char* szArticle, int nPart, int nPartOf)
{
	m_pThreadMarshal->SetCurrentArticleStatus(m_iThread, szArticle, nPart, nPartOf);
	::PostMessage(m_hWndNotify, WM_THREAD_UPDATE_PROGRESS, m_iThread*2, 0);

	/*
	if(m_pThreadMarshal->GetLock())
	{
		strcpy(Task_Article, szArticle);
		m_pThreadMarshal->ReleaseLock();
	}
	*/
}

void CPosterThread::SetTask_Result(BOOL bSuccessful, char* szResult, CTaskObject* pTaskObject)
{
	if(m_pThreadMarshal->GetLock())
	{
		Task_Result_OK = bSuccessful;
		strcpy(Task_Result, szResult);
		if(pTaskObject != NULL)
			strcpy(pTaskObject->m_szLastResult, szResult);
		m_pThreadMarshal->ReleaseLock();
	}
}

void CPosterThread::SendHeaders(LPCTSTR szArticleSubject, BOOL bIncludeTime, CTime& theServerTime, CTaskObject* pTaskObject, int partNum, int nParts, CString& postok)
{
	TCHAR szArticleFrom[256];

	// Build szArticleFrom string from the From / FromNick settings
	if(strlen(m_Settings.m_szFromNick) > 0)
		sprintf(szArticleFrom, "%s (%s)", m_Settings.m_szFrom, m_Settings.m_szFromNick);
	else
		strcpy(szArticleFrom, m_Settings.m_szFrom);

	TCHAR szLine[512];

	// Send "From:" Header
	sprintf(szLine, "From: %s\r\nSender: %s\r\n", szArticleFrom, m_Settings.m_szFrom);
	NNTP_SendLine(szLine);

	// Send "Newsgroups:" header
	sprintf(szLine, "Newsgroups: ");
	strcat(szLine, (LPCTSTR) pTaskObject->m_strGroups);
	strcat(szLine, "\r\n");
	NNTP_SendLine(szLine);

	// Send "Subject:" header
	sprintf(szLine, "Subject: %s\r\n", szArticleSubject);
	NNTP_SendLine(szLine);
	
	// Send "Organization:" header (optional)
	if(strlen(m_Settings.m_szOrganization) > 0)
	{
		sprintf(szLine, "Organization: %s\r\n", m_Settings.m_szOrganization);
		NNTP_SendLine(szLine);
	}

	// Send "Reply-To:" header (optional)
	if(strlen(m_Settings.m_szReplyTo) > 0)
	{
		sprintf(szLine, "Reply-To: %s\r\n", m_Settings.m_szReplyTo);
		NNTP_SendLine(szLine);
	}

	// Send "Followup-To:" header (optional)
	if(strlen(m_Settings.m_szFollowupTo) > 0)
	{
		sprintf(szLine, "Followup-To: %s\r\n", m_Settings.m_szFollowupTo);
		NNTP_SendLine(szLine);
	}

	NNTP_SendLine(m_Settings.m_szXBinaryPoster);

	if (m_Settings.m_bFileThread || pTaskObject->getMessageID().GetLength()>0)
	{
		CString messageid = pTaskObject->GenerateMessageID(partNum, nParts, m_Settings.m_szDomainID);

		CString msgid_begin = messageid.Left(messageid.Find("@"));
		CString msgid_end = messageid.Right(messageid.GetLength()-messageid.Find("@"));

		// CHANGE MSGID IF ALREADY POSTED (by looking at history)
		postok = CString("POST OK: ")+msgid_begin;
		int npost = 0;
		for (int i=0;i<pTaskObject->m_Log.GetSize();i++)
		{
			if (pTaskObject->m_Log.GetAt(i).Left(postok.GetLength())==postok)
			{
				int n = atoi(((LPCTSTR)(pTaskObject->m_Log.GetAt(i)))+postok.GetLength());
				if (n>=npost) npost=n+1;
			}
		}
		if (npost>0)
		{
			messageid.Format("%s%d%s",(LPCTSTR)msgid_begin,npost,(LPCSTR)msgid_end);
		}
		
		postok.Format("POST OK: %s %s",(LPCTSTR)messageid,szArticleSubject);

		sprintf(szLine, "Message-ID: %s\r\n", (LPCTSTR)messageid);
		NNTP_SendLine(szLine);
	}
	else
		postok.Format("POST OK: <> %s",szArticleSubject);

	CString references = pTaskObject->getReferences((partNum>0 && m_Settings.m_bFileThread)?TRUE:FALSE,nParts);
	if (references.GetLength()>0)
	{
		sprintf(szLine, "References: %s\r\n", references);
		NNTP_SendLine(szLine);
	}

	// Include X-No-Archive header if option set
	if(m_Settings.m_bXNoArchiveHeader)
	{
		sprintf(szLine, "X-No-Archive: yes\r\n");
		NNTP_SendLine(szLine);
	}

	// Some NNTP Servers may not support the 'DATE' command extension
	if(bIncludeTime&&m_Settings.m_bXServerDate)
	{
		// X-Server-Date: 26 May 2000 12:51:42 GMT
		switch(theServerTime.GetMonth())
		{
		case 1: sprintf(szLine, "X-Server-Date: %02d Jan %04d %02d:%02d:%02d GMT\r\n", theServerTime.GetDay(), theServerTime.GetYear(), 
					theServerTime.GetHour(), theServerTime.GetMinute(), theServerTime.GetSecond());
				NNTP_SendLine(szLine);
				break;
		case 2: sprintf(szLine, "X-Server-Date: %02d Feb %04d %02d:%02d:%02d GMT\r\n", theServerTime.GetDay(), theServerTime.GetYear(), 
					theServerTime.GetHour(), theServerTime.GetMinute(), theServerTime.GetSecond());
				NNTP_SendLine(szLine);
				break;
		case 3: sprintf(szLine, "X-Server-Date: %02d Mar %04d %02d:%02d:%02d GMT\r\n", theServerTime.GetDay(), theServerTime.GetYear(), 
					theServerTime.GetHour(), theServerTime.GetMinute(), theServerTime.GetSecond());
				NNTP_SendLine(szLine);
				break;
		case 4: sprintf(szLine, "X-Server-Date: %02d Apr %04d %02d:%02d:%02d GMT\r\n", theServerTime.GetDay(), theServerTime.GetYear(), 
					theServerTime.GetHour(), theServerTime.GetMinute(), theServerTime.GetSecond());
				NNTP_SendLine(szLine);
				break;
		case 5: sprintf(szLine, "X-Server-Date: %02d May %04d %02d:%02d:%02d GMT\r\n", theServerTime.GetDay(), theServerTime.GetYear(), 
					theServerTime.GetHour(), theServerTime.GetMinute(), theServerTime.GetSecond());
				NNTP_SendLine(szLine);
				break;
		case 6: sprintf(szLine, "X-Server-Date: %02d Jun %04d %02d:%02d:%02d GMT\r\n", theServerTime.GetDay(), theServerTime.GetYear(), 
					theServerTime.GetHour(), theServerTime.GetMinute(), theServerTime.GetSecond());
				NNTP_SendLine(szLine);
				break;
		case 7: sprintf(szLine, "X-Server-Date: %02d Jul %04d %02d:%02d:%02d GMT\r\n", theServerTime.GetDay(), theServerTime.GetYear(), 
					theServerTime.GetHour(), theServerTime.GetMinute(), theServerTime.GetSecond());
				NNTP_SendLine(szLine);
				break;
		case 8: sprintf(szLine, "X-Server-Date: %02d Aug %04d %02d:%02d:%02d GMT\r\n", theServerTime.GetDay(), theServerTime.GetYear(), 
					theServerTime.GetHour(), theServerTime.GetMinute(), theServerTime.GetSecond());
				NNTP_SendLine(szLine);
				break;
		case 9: sprintf(szLine, "X-Server-Date: %02d Sep %04d %02d:%02d:%02d GMT\r\n", theServerTime.GetDay(), theServerTime.GetYear(), 
					theServerTime.GetHour(), theServerTime.GetMinute(), theServerTime.GetSecond());
				NNTP_SendLine(szLine);
				break;
		case 10: sprintf(szLine, "X-Server-Date: %02d Oct %04d %02d:%02d:%02d GMT\r\n", theServerTime.GetDay(), theServerTime.GetYear(), 
					theServerTime.GetHour(), theServerTime.GetMinute(), theServerTime.GetSecond());
				NNTP_SendLine(szLine);
				break;
		case 11: sprintf(szLine, "X-Server-Date: %02d Nov %04d %02d:%02d:%02d GMT\r\n", theServerTime.GetDay(), theServerTime.GetYear(), 
					theServerTime.GetHour(), theServerTime.GetMinute(), theServerTime.GetSecond());
				NNTP_SendLine(szLine);
				break;
		case 12: sprintf(szLine, "X-Server-Date: %02d Dec %04d %02d:%02d:%02d GMT\r\n", theServerTime.GetDay(), theServerTime.GetYear(), 
					theServerTime.GetHour(), theServerTime.GetMinute(), theServerTime.GetSecond());
				NNTP_SendLine(szLine);
				break;

		}
	}

	if (m_Settings.m_bHeaderPartial && nParts > 1)
	{
		NNTP_SendLine("MIME-Version: 1.0\r\n");
		sprintf(szLine,"\ttotal=%d;\r\n",nParts);
		NNTP_SendLine("Content-Type: message/partial;\r\n");
		sprintf(szLine,"\ttotal=%d;\r\n",nParts);
		NNTP_SendLine(szLine);
		CString messageid = pTaskObject->GenerateMessageID(m_Settings.m_szDomainID);
		sprintf(szLine,"\tid=\"partial-%s\";\r\n",(LPCSTR)messageid);
		NNTP_SendLine(szLine);
		sprintf(szLine,"\tnumber=%d\r\n",partNum+1);
		NNTP_SendLine(szLine);
		if (partNum==0)
		{ // send an aditionnal empty line
			sprintf(szLine, "\r\n\r\n");
			NNTP_SendLine(szLine);
		}

	}

	sprintf(szLine, "\r\n\r\n");
	NNTP_SendLine(szLine);
}

void CPosterThread::OnMessage_QUIT(WPARAM wParam, LPARAM lParam)
{
	TRACE("CPosterThread::OnMessage_QUIT()\n");
	PostQuitMessage(0);
}

void CPosterThread::OnMessage_POST(WPARAM wParam, LPARAM lParam)
{
	CTaskObject* pTaskObject = (CTaskObject*) lParam;
	if(m_pThreadMarshal->GetLock())
	{
		m_pTaskObjectREAD = pTaskObject;
		m_pThreadMarshal->ReleaseLock();
	}
	OnMessage_POST_in(wParam,pTaskObject);
	if(m_pThreadMarshal->GetLock())
	{
		m_pTaskObjectREAD = NULL;
		m_pThreadMarshal->ReleaseLock();
	}
        return;
}
void CPosterThread::OnMessage_POST_in(WPARAM wParam, CTaskObject* pTaskObject)
{
	TRACE("CPosterThread::OnMessage_POST()\n");

	CFile theFile;
	CFileException cfe;
	int nResultCode;
	int nFileSizeK;
	int /*nLines,*/ nParts, curPart;
	DWORD nFileSizeBytes;
	//int nBatchLinesInc;
	char szArticle[1024];
	char szLine[1024];
	TCHAR szLOG[1024];

	pTaskObject->SetModified();

	if(theFile.Open(pTaskObject->m_szFilename, CFile::modeRead | CFile::shareDenyNone, &cfe) == FALSE)
	{
		switch(cfe.m_cause)
		{
		case CFileException::generic:			SetTask_Result(FALSE, "File Open Error (Generic)",pTaskObject); pTaskObject->m_Log.Add("File Open Error (Generic)"); break;
		case CFileException::fileNotFound:		SetTask_Result(FALSE, "File Open Error (File Not Found)",pTaskObject); pTaskObject->m_Log.Add("File Open Error (File Not Found)"); break;
		case CFileException::badPath:			SetTask_Result(FALSE, "File Open Error (Bad Path)",pTaskObject); pTaskObject->m_Log.Add("File Open Error (Bad Path)"); break;
		case CFileException::tooManyOpenFiles:	SetTask_Result(FALSE, "File Open Error (Too Many Open Files)",pTaskObject); pTaskObject->m_Log.Add("File Open Error (Too Many Open Files)"); break;
		case CFileException::accessDenied:		SetTask_Result(FALSE, "File Open Error (Access Denied)",pTaskObject); pTaskObject->m_Log.Add("File Open Error (Access Denied)"); break;
		case CFileException::invalidFile:		SetTask_Result(FALSE, "File Open Error (Invalid File)",pTaskObject); pTaskObject->m_Log.Add("File Open Error (Invalid File)"); break;
		case CFileException::removeCurrentDir:	SetTask_Result(FALSE, "File Open Error (Removed Current Dir)",pTaskObject); pTaskObject->m_Log.Add("File Open Error (Removed Current Dir)"); break;
		case CFileException::directoryFull:		SetTask_Result(FALSE, "File Open Error (Directory Full)",pTaskObject); pTaskObject->m_Log.Add("File Open Error (Directory Full)"); break;
		case CFileException::badSeek:			SetTask_Result(FALSE, "File Open Error (Bad Seek)",pTaskObject); pTaskObject->m_Log.Add("File Open Error (Bad Seek)"); break;
		case CFileException::hardIO:			SetTask_Result(FALSE, "File Open Error (Hard IO Error)",pTaskObject); pTaskObject->m_Log.Add("File Open Error (Hard IO Error)"); break;
		case CFileException::sharingViolation:	SetTask_Result(FALSE, "File Open Error (Sharing Violation)",pTaskObject); pTaskObject->m_Log.Add("File Open Error (Sharing Violation)"); break;
		case CFileException::lockViolation:		SetTask_Result(FALSE, "File Open Error (Lock Violation)",pTaskObject); pTaskObject->m_Log.Add("File Open Error (Lock Violation)"); break;
		case CFileException::diskFull:			SetTask_Result(FALSE, "File Open Error (Disk Full)",pTaskObject); pTaskObject->m_Log.Add("File Open Error (Disk Full)"); break;
		case CFileException::endOfFile:			SetTask_Result(FALSE, "File Open Error (End Of File)",pTaskObject); pTaskObject->m_Log.Add("File Open Error (End Of File)"); break;
		default: SetTask_Result(FALSE, "File Open Error (Unknown Cause)",pTaskObject); break;
		}
		sprintf(szLOG, " --> Article '%s' Not Posted!", pTaskObject->m_szSubject);
		pTaskObject->m_Log.Add(szLOG);
		if(m_STOP == TRUE) return;
		NNTP_DisconnectServer();
		if(m_hWndNotify != NULL) ::PostMessage(m_hWndNotify, WM_THREAD_NO_CONNECTION, m_iThread*2, (LPARAM)pTaskObject);
		return;
	}

	m_pThreadMarshal->SetCurrentTargetNewsgroups(m_iThread, (LPCTSTR) pTaskObject->m_strGroups);

	// Get File Size and Calculate Number of Parts
	nFileSizeBytes = theFile.GetLength();
	
	nFileSizeK = ((nFileSizeBytes-1) / 1024) + 1;

	nParts = pTaskObject->getNbParts();

	if (pTaskObject->m_ullFileSize!=nFileSizeBytes)
	{
	  SetTask_Result(FALSE,"File size changed",pTaskObject);
	  sprintf(szLOG, " --> Article '%s' Not Posted!", pTaskObject->m_szSubject);
	  pTaskObject->m_Log.Add(szLOG);
	  if(m_STOP == TRUE) return;
	  NNTP_DisconnectServer();
	  if(m_hWndNotify != NULL) ::PostMessage(m_hWndNotify, WM_THREAD_NO_CONNECTION, m_iThread*2, (LPARAM)pTaskObject);
	  return;
	}

	CString szBaseFilename, szPathToFile;
	_BreakFilename(pTaskObject->m_szFilename, szPathToFile, szBaseFilename);

	TRACE3("File Size = %d  -  Total Lines = %d  -  Number of Parts To Post: %d\n", nFileSizeBytes, ((nFileSizeBytes+YENCLEN-1) / YENCLEN), nParts);

	int nLinesSentTotal = 0;
	int nLinesSentThisPart = 0;
	double d_TOT, d_DON, d_POS;
	int nProgressPart;
	int nProgressTotal;

	BOOL  bXServerDate = FALSE;
	CTime tmServerDate;

	// Send Prefix Text (0/n) article if there is anything in pTaskObject->m_strTextPrefix
	LPCTSTR pText = NULL;
	int lpos;

	int bPostPrefixText = FALSE;
	int part0Num = 0;
	if(pTaskObject->postPrefixText()) part0Num=1;
	if((pTaskObject->m_strTextPrefix.GetLength() > 0) && (pTaskObject->m_bPostPrefixText == TRUE))
		bPostPrefixText = TRUE;

	if( (pTaskObject->m_dwaPartsToPost.GetSize() == 0) && (bPostPrefixText == FALSE))
	{
		//ASSERT: This is not an error anymore
		pTaskObject->m_Log.Add("Queued, But No Parts Flagged For Posting");
		theFile.Close();
		m_pThreadMarshal->SetProgress(m_iThread, 0,0);
		if(m_STOP == TRUE)
			SetTask_Result(FALSE, "Stopped By User",pTaskObject);
		else 
			SetTask_Result(TRUE, "No Parts Flagged To Post!",pTaskObject); 
		if(m_hWndNotify != NULL) ::PostMessage(m_hWndNotify, WM_THREAD_READY, m_iThread*2+m_STOP, (LPARAM)pTaskObject);
		return;
	}

	if(m_Settings.m_bXServerDate)
	{
		for (int i=0;i<2;i++) {
			if(NNTP_SendLine("DATE\r\n"))
			{
				if(!NNTP_GetLine(szLine, 1023))
				{
					theFile.Close();
					SetTask_Result(FALSE, "Error Sending POST Command To Server",pTaskObject);
					pTaskObject->m_Log.Add("Error Sending POST Command!");
					sprintf(szLOG, "  --> Article '%s' Not Posted!", szArticle);
					pTaskObject->m_Log.Add(szLOG);
					NNTP_DisconnectServer();
					if(m_STOP == TRUE) return;
					if(m_hWndNotify != NULL) ::PostMessage(m_hWndNotify, WM_THREAD_NO_CONNECTION, m_iThread*2, (LPARAM)pTaskObject);
					return;
				}

				nResultCode = atoi(szLine);

				if (nResultCode==480&&Authenticate(szLOG)) continue;
		
				if(nResultCode == 111)
				{
					// szLine should look like this:
					// 111 20000526212626
					// Which is GMT for Year 2000 Month 05 Day 26 Hour 21 Min 26 Sec 26
					if(ParseServerDate(tmServerDate, (LPCTSTR) (szLine + 4)))
						bXServerDate = TRUE;
				}
			}
			break;
		}

	}

	if(bPostPrefixText)
	{
		CM_BuildArticleName(szArticle, pTaskObject->m_szSubject, pTaskObject->m_szFilename, 0, nParts);
		SetTask_Article(szArticle, 0, nParts);

		TRACE("Posting Article: %s\n", szArticle);

		TRACE("Sending 'post' command...\n");

		for (int i=0;i<2;i++) {
			if(!NNTP_SendLine("post\r\n"))
			{
				theFile.Close();
				SetTask_Result(FALSE, "Error Sending POST Command To Server",pTaskObject);
				pTaskObject->m_Log.Add("Error Sending POST Command!");
				sprintf(szLOG, "  --> Article '%s' Not Posted!", szArticle);
				pTaskObject->m_Log.Add(szLOG);
				NNTP_DisconnectServer();
				if(m_STOP == TRUE) return;
				if(m_hWndNotify != NULL) ::PostMessage(m_hWndNotify, WM_THREAD_NO_CONNECTION, m_iThread*2, (LPARAM)pTaskObject);
				return;
			}

			TRACE("Getting 'post' command Response...\n");

			if(!NNTP_GetLine(szLine, 1023))
			{
				theFile.Close();
				SetTask_Result(FALSE, "Error Receiving POST Command Response From Server",pTaskObject);
				pTaskObject->m_Log.Add("Error Receiving POST Command Response!");
				sprintf(szLOG, "  --> Article '%s' Not Posted!", szArticle);
				pTaskObject->m_Log.Add(szLOG);
				NNTP_DisconnectServer();
				if(m_STOP == TRUE) return;
				if(m_hWndNotify != NULL) ::PostMessage(m_hWndNotify, WM_THREAD_NO_CONNECTION, m_iThread*2, (LPARAM)pTaskObject);
				return;
			}

			TRACE1("Got Post Command Response: %s\n", szLine);

			nResultCode = atoi(szLine);

			if (nResultCode==480&&Authenticate(szLOG)) continue;

			if(nResultCode != 340)
			{
				theFile.Close();
				SetTask_Result(FALSE, szLine,pTaskObject);
				sprintf(szLOG, "POST Command Response Error: %s", szLine);
				pTaskObject->m_Log.Add(szLOG);
				sprintf(szLOG, "  --> Article '%s' Not Posted!", szArticle);
				pTaskObject->m_Log.Add(szLOG);
				NNTP_DisconnectServer();
				if(m_hWndNotify != NULL) ::PostMessage(m_hWndNotify, WM_THREAD_NO_CONNECTION, m_iThread*2, (LPARAM)pTaskObject);
				return;
			}
			break;
		}

		// Send Article Header (From:, Newsgroups:, Subject:, Organization:)
		TRACE("Sending Article Header...\n");
		CString postok;
		SendHeaders(szArticle, bXServerDate, tmServerDate,pTaskObject, 0, nParts+part0Num,postok);

		// Post Text
		pText = (LPCTSTR) pTaskObject->m_strTextPrefix;
		while(*pText != '\0')
		{
			sprintf(szLine, "");
			lpos = 0;

			while((*pText != '\0') && (*pText != '\r') && (*pText != '\n'))
			{
				szLine[lpos] = *pText;
				++pText;
				++lpos;
				if((lpos == 1) && (*pText == '.'))
				{
					szLine[lpos] = '.';	// Double-up periods if lines starts with one
					++lpos;
				}
			}
			szLine[lpos] = '\0';
			strcat(szLine, "\r\n");
			
			// if(strcmp(szLine, ".\r\n") == 0)
			//	sprintf(szLine, "..\r\n");

			NNTP_SendLine(szLine);

			if(*pText == '\r') ++pText;
			if(*pText == '\n') ++pText;
		}

		// Finish Message
		NNTP_SendLine(".\r\n");

		// Get Reponse To Article Post
		TRACE("Done - Waiting For Server Response To Article Post\n");

		if(!NNTP_GetLine(szLine, 1023))
		{
			theFile.Close();
			SetTask_Result(FALSE, "Error Receiving POST Acknowledgement From Server",pTaskObject);
			pTaskObject->m_Log.Add("Error Receiving POST Acknowledgement From Server");
			sprintf(szLOG, "  --> Article '%s' Not Posted!", szArticle);
			pTaskObject->m_Log.Add(szLOG);
			NNTP_DisconnectServer();
			if(m_STOP == TRUE) return;
			if(m_hWndNotify != NULL) ::PostMessage(m_hWndNotify, WM_THREAD_NO_CONNECTION, m_iThread*2, (LPARAM)pTaskObject);
			return;
		}

		TRACE("Response Received: %s\n", szLine);
		nResultCode = atoi(szLine);

		if(nResultCode != 240)
		{
			theFile.Close();
			SetTask_Result(FALSE, szLine,pTaskObject);
			sprintf(szLOG, "POST Failed: %s", szLine);
			pTaskObject->m_Log.Add(szLOG);
			sprintf(szLOG, "  --> Article '%s' Not Posted!", szArticle);
			pTaskObject->m_Log.Add(szLOG);
			NNTP_DisconnectServer();
			if(m_hWndNotify != NULL) ::PostMessage(m_hWndNotify, WM_THREAD_NO_CONNECTION, m_iThread*2, (LPARAM)pTaskObject);
			return;
		}
		
		//sprintf(szLOG, "POST OK: %s", /*szLine,*/ szArticle);
		pTaskObject->m_Log.Add(postok);
		pTaskObject->m_bPostPrefixText = FALSE;

		strcpy(szLine, "Part 0 Posted");
	}

	//nBatchLinesInc = 0;
	int nMaxLinesThisPart;
	int nFileSeekPos;

	while(pTaskObject->m_dwaPartsToPost.GetSize() > 0)
	{
		if(m_STOP) break;

		curPart = pTaskObject->m_dwaPartsToPost.GetAt(0);

		CM_BuildArticleName(szArticle, pTaskObject->m_szSubject, pTaskObject->m_szFilename, curPart, nParts);

		SetTask_Article(szArticle, curPart, nParts);

		TRACE("Posting Article: %s\n", szArticle);

		TRACE("Sending 'post' command...\n");

		for (int i=0;i<2;i++) {
			if(!NNTP_SendLine("post\r\n"))
			{
				theFile.Close();
				SetTask_Result(FALSE, "Error Sending POST Command To Server",pTaskObject);
				pTaskObject->m_Log.Add("Error Sending POST Command!");
				sprintf(szLOG, "  --> Article '%s' Not Posted!", szArticle);
				pTaskObject->m_Log.Add(szLOG);
				NNTP_DisconnectServer();
				if(m_STOP == TRUE) return;
				if(m_hWndNotify != NULL) ::PostMessage(m_hWndNotify, WM_THREAD_NO_CONNECTION, m_iThread*2, (LPARAM)pTaskObject);
				return;
			}

			TRACE("Getting 'post' command Response...\n");

			if(!NNTP_GetLine(szLine, 1023))
			{
				theFile.Close();
				SetTask_Result(FALSE, "Error Receiving POST Command Response From Server",pTaskObject);
				pTaskObject->m_Log.Add("Error Receiving POST Command Response!");
				sprintf(szLOG, "  --> Article '%s' Not Posted!", szArticle);
				pTaskObject->m_Log.Add(szLOG);
				NNTP_DisconnectServer();
				if(m_STOP == TRUE) return;
				if(m_hWndNotify != NULL) ::PostMessage(m_hWndNotify, WM_THREAD_NO_CONNECTION, m_iThread*2, (LPARAM)pTaskObject);
				return;
			}

			TRACE1("Got Post Command Response: %s\n", szLine);

			nResultCode = atoi(szLine);

			if (nResultCode==480&&Authenticate(szLOG)) continue;

			if(nResultCode != 340)
			{
				theFile.Close();
				SetTask_Result(FALSE, szLine,pTaskObject);
				sprintf(szLOG, "POST Command Response Error: %s", szLine);
				pTaskObject->m_Log.Add(szLOG);
				sprintf(szLOG, "  --> Article '%s' Not Posted!", szArticle);
				pTaskObject->m_Log.Add(szLOG);
				NNTP_DisconnectServer();
				if(m_hWndNotify != NULL) ::PostMessage(m_hWndNotify, WM_THREAD_NO_CONNECTION, m_iThread*2, (LPARAM)pTaskObject);
				return;
			}
			break;
		}

		// Send Article Header (From:, Newsgroups:, Subject:, Organization:)
		TRACE("Sending Article Header...\n");
		CString postok;
		SendHeaders(szArticle, bXServerDate, tmServerDate,pTaskObject,curPart+part0Num-1,nParts+part0Num,postok);

		nLinesSentThisPart = 0;
		
		// Seek to correct position in file
		nFileSeekPos = (curPart - 1) * pTaskObject->m_nPartSize;
		theFile.Seek(nFileSeekPos, CFile::begin);
		nMaxLinesThisPart = (pTaskObject->m_nPartSize+YENCLEN-1)/YENCLEN;

		nLinesSentThisPart = 0;
		
		CString str;

		const int LOUTBUF=YENCLEN+1;
		char obuf[LOUTBUF+3];

		const int LINBUF=10000;
		unsigned char ibuf[LINBUF];

		int size=theFile.GetLength();
		int cur_ibuf=LINBUF;
		int chars_per_part=pTaskObject->m_nPartSize;

		int pos=nFileSeekPos+1;
		int chars_in_part = size-pos+1>chars_per_part ? chars_per_part : size-pos+1;
		
		// Begin the UUENCODING if this is Part 1

		// NNTP_SendLine((char*) (LPCTSTR) m_szVersionName);
		// NNTP_SendLine((char*) (LPCTSTR) m_szVersionInfo);
		// NNTP_SendLine("\r\n");
		str.Format("=ybegin part=%d line=%d size=%u name=%s\r\n=ypart begin=%u end=%u\r\n",
 		            curPart,YENCLEN,size,szBaseFilename,pos,pos+chars_in_part-1);
		NNTP_SendLine((LPTSTR)((LPCTSTR)str));

		if(nFileSeekPos + pTaskObject->m_nPartSize > (int) pTaskObject->m_ullFileSize)
			nMaxLinesThisPart = ((pTaskObject->m_ullFileSize - nFileSeekPos) / YENCLEN) + 1;

		unsigned int crc32=CRC.crc_begin();

		for (int count=0;count<chars_in_part;) {
			if(m_STOP) break;
			int l = 0;
			for (l=0;l<YENCLEN&&count<chars_in_part;l++) {
				if (cur_ibuf==LINBUF) {
					cur_ibuf=0;
					theFile.Read(ibuf,LINBUF);
				}
				unsigned char c=ibuf[cur_ibuf++];

				crc32 = CRC.crc_add(crc32,c);

				c+=42;
				switch (c) {
				case 0:
				case '\r':
				case '\n':
				case '=':
					obuf[l++]='=';
					c+=64;
				}
				obuf[l]=c;
	
				count++;
			}
			obuf[l++]='\r';
			obuf[l++]='\n';
			obuf[l]=0;
			if (obuf[0]=='.') NNTP_SendLine(".");
			NNTP_SendLine(obuf);
			++nLinesSentTotal;
			++nLinesSentThisPart;

			//if((count % 256) == 1)
			{
				d_TOT = (double) nMaxLinesThisPart;
				d_DON = (double) nLinesSentThisPart;
				d_POS = (d_DON / d_TOT) * (double) 100.0;
				nProgressPart = (int) d_POS;
				d_TOT = (double) pTaskObject->m_ullFileSize;
				d_DON = (double) theFile.GetPosition();
				d_POS = (d_DON / d_TOT) * (double) 100.0;
				nProgressTotal = (int) d_POS;
				m_pThreadMarshal->SetProgress(m_iThread, nProgressTotal, nProgressPart);
				m_pThreadMarshal->SetTemporaryPos(m_iThread, nLinesSentThisPart); 
			}
		}

		if(m_STOP) break;

		m_pThreadMarshal->SetTemporaryPos(m_iThread, nLinesSentThisPart); 
		//m_pThreadMarshal->IncrementBatchPos(nBatchLinesInc); nBatchLinesInc = 0;

		// Update Progress every now and then...
		if(curPart < nParts)
		{
			d_TOT = (double) pTaskObject->getMaxLines();
			d_DON = (double) nLinesSentThisPart;
			d_POS = (d_DON / d_TOT) * (double) 100.0;
			nProgressPart = (int) d_POS;
			m_pThreadMarshal->SetProgress(m_iThread, nProgressTotal, 100);
			::Sleep(0);
		}

		if(curPart == nParts)
		{
			m_pThreadMarshal->SetProgress(m_iThread, 100, 100);
		}

		crc32 = CRC.crc_end(crc32);

		str.Format("=yend size=%u part=%d pcrc32=%08x\r\n",
		           chars_in_part,curPart,crc32);

		NNTP_SendLine((LPTSTR)((LPCTSTR)str));

		// Finish Message
		NNTP_SendLine(".\r\n");

		// Get Reponse To Article Post
		TRACE("Done - Waiting For Server Response To Article Post\n");

		if(!NNTP_GetLine(szLine, 1023))
		{
			theFile.Close();
			if(m_STOP)
			{
				SetTask_Result(FALSE, "Stopped By User",pTaskObject);
				pTaskObject->m_Log.Add("Stopped By User");
				sprintf(szLOG, "  --> Article '%s' Not Posted!", szArticle);
				pTaskObject->m_Log.Add(szLOG);
				return;
			}
			SetTask_Result(FALSE, "Error Receiving POST Acknowledgement From Server",pTaskObject);
			pTaskObject->m_Log.Add("Error Receiving POST Acknowledgement From Server");
			sprintf(szLOG, "  --> Article '%s' Not Posted!", szArticle);
			pTaskObject->m_Log.Add(szLOG);
			NNTP_DisconnectServer();
			if(m_STOP == TRUE) return;
			m_pThreadMarshal->SetTemporaryPos(m_iThread, 0);
			if(m_hWndNotify != NULL) ::PostMessage(m_hWndNotify, WM_THREAD_NO_CONNECTION, m_iThread*2, (LPARAM)pTaskObject);
			return;
		}

		TRACE("Response Received: %s\n", szLine);
		nResultCode = atoi(szLine);

		if(nResultCode != 240)
		{
			theFile.Close();
			SetTask_Result(FALSE, szLine,pTaskObject);
			sprintf(szLOG, "POST Failed: %s", szLine);
			pTaskObject->m_Log.Add(szLOG);
			sprintf(szLOG, "  --> Article '%s' Not Posted!", szArticle);
			pTaskObject->m_Log.Add(szLOG);
			NNTP_DisconnectServer();
			m_pThreadMarshal->SetTemporaryPos(m_iThread, 0);
			if(m_hWndNotify != NULL) ::PostMessage(m_hWndNotify, WM_THREAD_NO_CONNECTION, m_iThread*2, (LPARAM)pTaskObject);
			return;
		}

		//sprintf(szLOG, "POST OK: %s", /*szLine,*/ szArticle);
		pTaskObject->m_Log.Add(postok); //szLOG);
		pTaskObject->m_dwaPartsToPost.RemoveAt(0);
		m_pThreadMarshal->FlushTemporaryPos(m_iThread, nLinesSentThisPart);

		if(m_STOP) break;

	}

	theFile.Close();

	m_pThreadMarshal->SetProgress(m_iThread, 0,0);

	if((m_STOP == TRUE) && (pTaskObject->m_dwaPartsToPost.GetSize() > 0))
		SetTask_Result(FALSE, "Stopped By User",pTaskObject);
	else 
	{
		sprintf(szLOG, "POST OK: %s", szLine);
		SetTask_Result(TRUE, szLOG,pTaskObject);
	}

	if(m_hWndNotify != NULL) ::PostMessage(m_hWndNotify, WM_THREAD_READY, m_iThread*2+m_STOP, (LPARAM)pTaskObject);

}

int CPosterThread::NNTP_GetLine(char* p, int MaxChar)
{
	char ch = 0;
	int n = 0;
	int rv;

	NNTP_FlushOutputBuffer();

	for(;;)
	{
		rv = recv(m_hSocket, &ch, 1, 0);
		if(rv == SOCKET_ERROR) { TRACE("\n"); return(0); }
		if(rv == 0)	break;

		TRACE2(" RCV: '%c' (%d)", ch, (UINT) ch);
		if(n % 8 == 7) TRACE("\n");

		if(ch == 0x0D) { p[n] = '\0'; break; }
		if((ch >= 0x20) || (ch == 0x09)) { p[n] = ch; ++n; }
		if(n >= MaxChar) { p[MaxChar] = '\0'; TRACE("\n"); return(MaxChar); }
	}
	TRACE("\n");
	return(n);
}

int CPosterThread::NNTP_SendLine(const char* p)
{
	m_nSendBufferSize = (m_Settings.m_nBandwidthCap==0 || m_Settings.m_nBandwidthCap>8*m_Settings.m_nConnections)?8*1024:(m_Settings.m_nBandwidthCap*1024)/m_Settings.m_nConnections;
	// m_nSendBufferSize = 8400;
	// m_nSendBufferPos = 0;

	// Get size of text to transmit
	int sl = strlen(p);	
	if(sl == 0) return(0);

	// Check to see if buffer will hold it
	if( (sl + m_nSendBufferPos) < m_nSendBufferSize)
	{
		// Yes.. just add it to the buffer..
		::CopyMemory(&m_szSendBuffer[m_nSendBufferPos], p, sl);
		m_nSendBufferPos += sl;
		return(sl);
	}

	// No room.. flush the buffer and set the new buffer contents..
	NNTP_FlushOutputBuffer();

	// Special Case: Send data now if it won't fit in buffer
	if(sl > m_nSendBufferSize)
	{
		//m_pThreadMarshal->TransmissionNotify(sl);
		return(SendCapped(p, sl));
	}

	// OK, it fits.. Set send buffer to contents (we just flushed it)
	::CopyMemory(m_szSendBuffer, p, sl);
	m_nSendBufferPos = sl;
	return(sl);
}

int CPosterThread::NNTP_FlushOutputBuffer()
{
	// Is there anything in the buffer?
	if(m_nSendBufferPos > 0)
	{
		// Yes..
		int nRetVal;
		//m_pThreadMarshal->TransmissionNotify(m_nSendBufferPos);
		nRetVal = SendCapped(m_szSendBuffer, m_nSendBufferPos);
		m_nSendBufferPos = 0;
	}
	return(0);
}

BOOL CPosterThread::Authenticate(char* rvConnectString)
{
	int nResultCode;
	char szLine[1024];
		
	// Send "authinfo user xxx"
	sprintf(szLine, "authinfo user %s\r\n", m_Settings.m_szUsername);
		
	TRACE1("Sending User Authentication: %s", szLine);

	if(!NNTP_SendLine(szLine))
	{
		strcpy(rvConnectString, "Error Sending Authentication Command (User) To Server");
		NNTP_DisconnectServer();
		return(FALSE);
	}

	TRACE("Getting User Authentication Response\n");
	szLine[0] = '\0';

	if(!NNTP_GetLine(szLine, 1023))
	{
		strcpy(rvConnectString, "Error Receiving Authentication Response (User) From Server");
		NNTP_DisconnectServer();
		return(FALSE);
	}

	TRACE1("User Authentication Response: %s", szLine);

	nResultCode = atoi(szLine);

	if(nResultCode != 381)
	{
		strcpy(rvConnectString, "Error Submitting Authentication (User) To Server:\n");
		strcat(rvConnectString, szLine);
		NNTP_DisconnectServer();
		return(FALSE);
	}
		
	// Send "authinfo pass xxxx"
	sprintf(szLine, "authinfo pass %s\r\n", m_Settings.m_szPassword);

	TRACE1("Sending Pass Authentication: %s", szLine);

	if(!NNTP_SendLine(szLine))
	{
		strcpy(rvConnectString, "Error Sending Authentication Command (Password) To Server");
		NNTP_DisconnectServer();
		return(FALSE);
	}

	TRACE("Getting User Authentication Response\n");
	szLine[0] = '\0';

	if(!NNTP_GetLine(szLine, 1023))
	{
		strcpy(rvConnectString, "Error Receiving Authentication Response (Password) From Server");
		NNTP_DisconnectServer();
		return(FALSE);
	}

	TRACE1("Pass Authentication Response: %s", szLine);

	nResultCode = atoi(szLine);

	if(nResultCode != 281)
	{
		strcpy(rvConnectString, "Error Submitting Authentication (Password) To Server:\n");
		strcat(rvConnectString, szLine);
		NNTP_DisconnectServer();
		return(FALSE);
	}
	return TRUE;
}

BOOL CPosterThread::NNTP_ConnectServer(char* ServerName, int nPort, char* rvConnectString)
{
	struct hostent* HostEnt;
	if((HostEnt = ::gethostbyname(ServerName)) == NULL)
	{
		strcpy(rvConnectString, "ERROR: Unable To Resolve Host Name '");
		strcat(rvConnectString, ServerName);
		strcat(rvConnectString, "'");
		return(FALSE);
	}

	struct sockaddr_in SA;
	SA.sin_family = AF_INET;
	SA.sin_port   = htons(nPort);
	memcpy(&(SA.sin_addr.s_addr), HostEnt->h_addr_list[0], 4);
	m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(m_hSocket == 0)
	{
		strcpy(rvConnectString, "ERROR: Can't Create Socket");
		return(FALSE);
	}

	char szLine[1024];

	if(connect(m_hSocket, (struct sockaddr*) &SA, sizeof(SA)) != 0)
	{
		strcpy(rvConnectString, "ERROR: Can't Connect To Server");
		return(FALSE);
	}

	if(NNTP_GetLine(szLine, 1023) == 0)
	{
		strcpy(rvConnectString, "ERROR: No Connection Response From Server");
		NNTP_DisconnectServer();
		return(FALSE);
	}

	strcpy(rvConnectString, szLine);

	// Authenticate User if required
	if(m_Settings.m_bRequireLogin == TRUE)
	{
		return Authenticate(rvConnectString);
	}
	return(TRUE);	
}

BOOL CPosterThread::NNTP_DisconnectServer()
{
	if(m_hSocket != 0)
	{
		SOCKET hSocket = m_hSocket;
		m_hSocket = 0;

		if(closesocket(hSocket) == 0)
			return TRUE;
		else
			return FALSE;
	}
	return TRUE;
}

void CPosterThread::_BreakFilename(LPCTSTR FullFilename, CString& Path, CString& File)
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

BOOL CPosterThread::ParseServerDate(CTime& rvTime, LPCTSTR szDATE)
{
	TRACE("CPosterThread::ParseServerDate(%s)\n", szDATE);

	TCHAR szYear[8];
	TCHAR szMonth[8];
	TCHAR szDay[8];
	TCHAR szHour[8];
	TCHAR szMin[8];
	TCHAR szSec[8];

	if(strlen(szDATE) != 14) return(FALSE);

	::ZeroMemory(szYear, 8);
	::ZeroMemory(szMonth, 8);
	::ZeroMemory(szDay, 8);
	::ZeroMemory(szHour, 8);
	::ZeroMemory(szMin, 8);
	::ZeroMemory(szSec, 8);

	strncpy(szYear, &szDATE[0], (size_t) 4);
	strncpy(szMonth, &szDATE[4], (size_t) 2);
	strncpy(szDay, &szDATE[6], (size_t) 2);
	strncpy(szHour, &szDATE[8], (size_t) 2);
	strncpy(szMin, &szDATE[10], (size_t) 2);
	strncpy(szSec, &szDATE[12], (size_t) 2);

	rvTime = CTime::CTime(atoi(szYear), atoi(szMonth), atoi(szDay), atoi(szHour), atoi(szMin), atoi(szSec));
	return(TRUE);
}


void CPosterThread::UpdateCap(int BWCap)
{
	m_Settings.m_nBandwidthCap = BWCap;
}

//CCriticalSection CAP_CriticalSection;
CMutex CAP_Mutex;

#if 1

// this array contains the data each thread want to post
static int to_send[NPOSTTHREAD] = { 0 };
// this array contains the data each thread can post
static int schedule_send[NPOSTTHREAD] = { 0 };
// this array contains the last time the sending was scheduled
static time_t last_send = 0;
// this array contains the data already sent or scheduled at the time last_send
static int sent = 0;

int CPosterThread::CalcCAP(int it, int n)
{
  time_t now;
  int i;
	CSingleLock lock(&CAP_Mutex,FALSE);
  if (m_Settings.m_nConnections>1) lock.Lock();
  to_send[it]=n;
	now = time(NULL);
  while ( m_Settings.m_nBandwidthCap>0
			 && schedule_send[it]==0
	     && sent>=m_Settings.m_nBandwidthCap*1024
			 && last_send==now
			 ) {
    if (m_Settings.m_nConnections>1) lock.Unlock();
    Sleep(500);
    if (m_STOP == TRUE) return -1;
    if (m_Settings.m_nConnections>1) lock.Lock();
		now = time(NULL);
  }
	// let's see which condition became false
	int cap = m_Settings.m_nBandwidthCap*1024;
	if (cap<=0)
	{ // NOTHING TO DO :-)
		m_pThreadMarshal->TransmissionNotify(m_iThread, n,now);
		/*return n;*/
	}
	else if (schedule_send[it]!=0)
	{ // some scheduled us to send some data :)
		if (n>schedule_send[it]) n=schedule_send[it];
		schedule_send[it]=0; // give up any other scheduled allocation
		m_pThreadMarshal->TransmissionNotify(m_iThread, n,last_send);
	}
	else if (sent<cap || m_Settings.m_nConnections<=1)
	{ // we do not reach the limit :)
		// or we are the only one thread, so we can take everything :)
		if (last_send!=now)
		{
			last_send=now;
			sent=0;
		}
		sent += n;
		if (sent>cap)
		{
			n-=sent-cap;
			sent = cap;
		}
		m_pThreadMarshal->TransmissionNotify(m_iThread, n,last_send);
	}
	else //if (last_send!=now) // this is implicit...
	{ // we are the first
		last_send=now;
		sent=0;
		//for (i=0;i<NPOSTTHREAD;i++) schedule_send[i]=0;
		int nt=0;
		for (i=0;i<m_Settings.m_nConnections;i++)
			if (to_send[i]) ++nt;
		while (sent<cap && nt>0)
		{
			int avail=cap-sent;
			// search the min
			int imin=-1;
			for (i=0;i<m_Settings.m_nConnections;i++)
			{
				if (to_send[i] && (imin==-1 || to_send[i]<to_send[imin]))
				{
					imin=i;
				}
			}
			if (imin==-1) break; // no more request
			if (to_send[imin]<=avail/nt)
			{ // we can fullfill the request totally
				schedule_send[imin]+=to_send[imin];
				sent+=to_send[imin];
				to_send[imin]=0;
				--nt;
			}
			else
			{ // partial fullfill of all requests
				for (i=0;i<m_Settings.m_nConnections;i++)
				{
					if (to_send[i])
					{
							int c = avail/nt;
    					schedule_send[i]+=c;
  						to_send[i]-=c;
							sent+=c;
							avail-=c;
					}
				}
				nt=0;
			}
		}
		if (n>schedule_send[it]) n = schedule_send[it];
		m_pThreadMarshal->TransmissionNotify(m_iThread, n,last_send);
	}

  //if (n+sent>m_Settings.m_nBandwidthCap*1024) n=m_Settings.m_nBandwidthCap*1024-sent;

  to_send[it]=0;
  schedule_send[it]=0;

	if (n==0)
	{
		return 0;
	}

  //if (m_Settings.m_nConnections>1) lock.Unlock();
	//sent += n;

  return n;
}

void CPosterThread::SentCAP(int it, int n)
{
/*
  if (m_Settings.m_nConnections>1)
	{
		CSingleLock(&CAP_Mutex,TRUE);
	  sent += n;
	}
	else
	  sent += n;
*/
}

#else



#endif

int CPosterThread::SendCapped(const TCHAR *buffer, int n)
{
	int n0=n;
	if (m_Settings.m_nBandwidthCap==0) { // no cap
		m_pThreadMarshal->TransmissionNotify(m_iThread, n, time(NULL));
		while (n>0){
			int sent=send(m_hSocket, buffer, n, 0);
			if (sent<=0) break;
			n-=sent;
			buffer+=sent;
		}
	}
	else {
		//m_pThreadMarshal->TransmissionNotify(m_iThread, n);
		while (n>0){// && m_STOP==FALSE) {
			int sent=CalcCAP(m_iThread,n);
			if (sent<0) break;
			if (sent>0) {
				sent = send(m_hSocket, buffer, sent, 0);
				if (sent<=0) break;
				buffer+=sent;
				n-=sent;
				SentCAP(m_iThread,sent);
			}
		}
	}
	return n0-n;
}
