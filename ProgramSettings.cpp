// ProgramSettings.cpp: implementation of the CProgramSettings class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PostMost.h"
#include "ProgramSettings.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CServerSettings::CServerSettings()
{
	m_nServerPort = 119;
	m_bRequireLogin = FALSE;
	::ZeroMemory(m_szServerName, 2*132);
	::ZeroMemory(m_szUsername, 132);
	::ZeroMemory(m_szPassword, 132);
	m_nConnections = 1;
}

CProgramSettings::CProgramSettings()
{
	m_Disclaimer = 0;

	// Server Settings
	strcpy(m_szServerName, "news.home.net");
	strcpy(m_szUsername, "guest");
	strcpy(m_szPassword, "guest");

	m_nBandwidthCap = 0;

	// Header Information
	m_bXNoArchiveHeader = FALSE;
	m_bXServerDate = FALSE;
	m_bHeaderPartial = FALSE;
	m_bHeaderPartial = FALSE;
	::ZeroMemory(m_szFrom, 132);
	::ZeroMemory(m_szFromNick, 64);
	::ZeroMemory(m_szReplyTo, 132);
	::ZeroMemory(m_szOrganization, 132);
	::ZeroMemory(m_szFollowupTo, 132);
	::ZeroMemory(m_szDomainID, 132);
	m_bFileThread = FALSE;

	// Group Settings
	m_bShowProgressDialogOnStart = TRUE;
	m_GroupList.RemoveAll();

	// Program Settings
	m_nMaxLines = 5000;
	m_bHighlightCurrentFile = TRUE;
	m_bShowProgressDialogOnStart = TRUE;
	m_bSortFilesOnAdd = TRUE;
	m_bAutoRetry = TRUE;
	m_bDefaultListViewColors = FALSE;
	m_bSysTrayIcon = FALSE;
	m_bSound = TRUE;
	m_bStallTimeoutEnabled = TRUE;
	m_nMinuteStallTimeout = 2;
	m_bDetectPAR2 = FALSE;
	strcpy(m_szSkin, "Default");

	// Extra Settings
	::ZeroMemory(m_szLastSubjectTemplate, 132);
	::ZeroMemory(m_szLastAddDir, _MAX_PATH);

	// 289:42:168:195
	m_nViewColWid[0] = 289;
	m_nViewColWid[1] = 42;
	m_nViewColWid[2] = 168;
	m_nViewColWid[3] = 195;

	m_nStartTimeSel[0] = 0;
	m_nStartTimeSel[1] = 0;
	m_nStartTimeSel[2] = 0;

	m_nStopTimeSel[0] = 0;
	m_nStopTimeSel[1] = 0;
	m_nStopTimeSel[2] = 0;

	m_nCheckStop = FALSE ;

	/*TCHAR szTemp[128];
	
		AD ORIGINAL: 'X-Newsposter: NNTP POWER-POST 2000 (Build 25b) - www.CosmicWolf.com'
		AD MIXED   : ']-Rj|xutxyjw: RRYU US\FW-USXY 2000 (Bznqi 25g) - |||.Ctxrnh\tqk.htr'
	strcpy(szTemp, "]-Rj|xutxyjw: RRYU US\\FW-USXY 2000 (Bznqi 25g) - |||.Ctxrnh\tqk.htr");
	::CM_UnMixString(szTemp, m_szXBinaryPoster, 128);
	*/
	
	// TRACE("Program Settings::AD = '%s'\n", m_szXBinaryPoster);

	strcpy(m_szXBinaryPoster, "X-Newsposter: " TITLE " (Modified POWER-POST www.CosmicWolf.com)\r\n");
}

CProgramSettings::~CProgramSettings()
{

}

void CProgramSettings::Copy(CProgramSettings& SourceSettings)
{

	*(CProgramSettings0*)this = SourceSettings;

	m_GroupList.RemoveAll();
	m_GroupList.Copy(SourceSettings.m_GroupList);

}

BOOL CProgramSettings::LoadFromRegistry()
{
	CWinApp* pApp = AfxGetApp();
	CString S;
	char szKey[32];
	//char szValue[256];
	int k, nSize;

	int   nDefault_BWCap                      = 0;
#define   MaxLines_UPPER_BOUND       12345
	int   nDefault_MaxLines                   = 1950;
	int   nDefault_bShowProgressDialogOnStart = TRUE;
	BOOL  nDefault_HighlightCurrentFile       = TRUE;
	BOOL  nDefault_bAutoRetry                 = TRUE;
	BOOL  nDefault_bSound                     = TRUE;
	BOOL  bDefault_bDetectPAR2                = FALSE;
	BOOL  bDefault_XNoArchiveHeader           = FALSE;
	BOOL  bDefault_XServerDate                = FALSE;
	BOOL  bDefault_HeaderPartial              = FALSE;
	BOOL  bDefault_ShowGroupDialogBeforePosting = TRUE;
	BOOL  bDefault_SortFiles                  = TRUE;
	BOOL  bDefault_FileThread                 = TRUE;
	char szDefault_From[]                     = "Yenc@power-post.org";
	char szDefault_FromNick[]                 = "Yenc-PP-A&A";
	char szDefault_ReplyTo[]                  = "";
	char szDefault_Organization[]             = "";
	char szDefault_FollowupTo[]               = "";
	char szDefault_nViewColWid[]              = "289:42:168:195";
	char szDefault_DomainID[]                 = "powerpost2000AA.local";
	char szDefault_Skin[]                     = "Default";

	LoadServerFromRegistry("Settings");

	m_Disclaimer = pApp->GetProfileInt("Settings","Disclaimer",	0);

	m_nBandwidthCap = pApp->GetProfileInt("Settings","BandwidthCap",	nDefault_BWCap);

	m_nMaxLines = pApp->GetProfileInt("Settings","MaxLines", nDefault_MaxLines);
	if(m_nMaxLines > MaxLines_UPPER_BOUND) m_nMaxLines = MaxLines_UPPER_BOUND;
	if(m_nMaxLines < 500) m_nMaxLines = 500;

	m_bHighlightCurrentFile = pApp->GetProfileInt("Settings","HL_Last", nDefault_HighlightCurrentFile);
	if((m_bHighlightCurrentFile != TRUE) && (m_bHighlightCurrentFile != FALSE)) m_bHighlightCurrentFile = nDefault_HighlightCurrentFile;

	m_bSortFilesOnAdd = pApp->GetProfileInt("Settings", "SortOnAdd", bDefault_SortFiles);
	if((m_bSortFilesOnAdd != TRUE) && (m_bSortFilesOnAdd != FALSE)) m_bSortFilesOnAdd= bDefault_SortFiles;

	m_bSysTrayIcon = pApp->GetProfileInt("Settings", "SysTray", FALSE);
	if((m_bSysTrayIcon != TRUE) && (m_bSysTrayIcon != FALSE)) m_bSysTrayIcon = FALSE;

	m_bSound = pApp->GetProfileInt("Settings","Sound", nDefault_bSound);
	if((m_bSound != TRUE) && (m_bSound != FALSE)) m_bSound = nDefault_bSound;

	S = pApp->GetProfileString("Settings","Skin", szDefault_Skin);
	if((S.GetLength() > MAX_PATH-1) || (S.GetLength() == 0)) S = szDefault_Skin;
	strcpy(m_szSkin, (LPCTSTR) S);

	m_bAutoRetry = pApp->GetProfileInt("Settings","AutoRetry", nDefault_bAutoRetry);
	if((m_bAutoRetry != TRUE) && (m_bAutoRetry != FALSE)) m_bAutoRetry = nDefault_bAutoRetry;

	m_bDetectPAR2 = pApp->GetProfileInt("Settings","DetectPAR2", bDefault_bDetectPAR2);
	if((m_bDetectPAR2 != TRUE) && (m_bDetectPAR2 != FALSE)) m_bDetectPAR2 = bDefault_bDetectPAR2;

	m_bShowProgressDialogOnStart = pApp->GetProfileInt("Settings","AutoShowProgress", nDefault_bShowProgressDialogOnStart);
	if((m_bShowProgressDialogOnStart != TRUE) && (m_bShowProgressDialogOnStart != FALSE)) m_bShowProgressDialogOnStart = FALSE;

	m_bXNoArchiveHeader = pApp->GetProfileInt("Settings","XNoArchiveHeader", bDefault_XNoArchiveHeader);
	if((m_bXNoArchiveHeader != TRUE) && (m_bXNoArchiveHeader != FALSE)) m_bXNoArchiveHeader = bDefault_XNoArchiveHeader;

	m_bXServerDate = pApp->GetProfileInt("Settings","XServerDate", bDefault_XServerDate);
	if((m_bXServerDate != TRUE) && (m_bXServerDate != FALSE)) m_bXServerDate = bDefault_XServerDate;

	m_bHeaderPartial = pApp->GetProfileInt("Settings","HeaderPartial", bDefault_HeaderPartial);
	if((m_bHeaderPartial != TRUE) && (m_bHeaderPartial != FALSE)) m_bHeaderPartial = bDefault_HeaderPartial;

	m_nCheckStop = pApp->GetProfileInt("Settings","CheckStop", FALSE);
	if((m_nCheckStop != TRUE) && (m_nCheckStop != FALSE)) m_nCheckStop = FALSE;

	m_bFileThread = pApp->GetProfileInt("Settings","FileThread", FALSE);
	if((m_bFileThread != TRUE) && (m_bFileThread != FALSE)) m_bFileThread = FALSE;

	S = pApp->GetProfileString("Settings","H_From", szDefault_From);
	if((S.GetLength() > 131) || (S.GetLength() == 0)) S = szDefault_From;
	strcpy(m_szFrom, (LPCTSTR) S);

	S = pApp->GetProfileString("Settings","H_FromNick", szDefault_FromNick);
	if((S.GetLength() > 63) || (S.GetLength() == 0)) S = szDefault_FromNick;
	strcpy(m_szFromNick, (LPCTSTR) S);

	S = pApp->GetProfileString("Settings","H_ReplyTo", szDefault_ReplyTo);
	if((S.GetLength() > 131) || (S.GetLength() == 0)) S = szDefault_ReplyTo;
	strcpy(m_szReplyTo, (LPCTSTR) S);

	S = pApp->GetProfileString("Settings","H_Org", szDefault_Organization);
	if((S.GetLength() > 131) || (S.GetLength() == 0)) S = szDefault_Organization;
	strcpy(m_szOrganization, (LPCTSTR) S);

	S = pApp->GetProfileString("Settings","H_Followup", szDefault_FollowupTo);
	if((S.GetLength() > 131) || (S.GetLength() == 0)) S = szDefault_FollowupTo;
	strcpy(m_szFollowupTo, (LPCTSTR) S);

	S = pApp->GetProfileString("Settings","H_DomainID", szDefault_DomainID);
	if((S.GetLength() > 131) || (S.GetLength() == 0)) S = szDefault_DomainID;
	strcpy(m_szDomainID, (LPCTSTR) S);

	S = pApp->GetProfileString("Settings","LastDir", "");
	if(S.GetLength() >= _MAX_PATH) S = "";
	strcpy(m_szLastAddDir, (LPCTSTR) S);

	S = pApp->GetProfileString("Settings","LastTempl_yEnc","(\?\?\?\?) [$1/$2] - \"$F\" yEnc");
	if(S.GetLength() >= 132) S = "(\?\?\?\?) [$1/$2] - \"$F\" yEnc";
	strcpy(m_szLastSubjectTemplate, (LPCTSTR) S);

	S = pApp->GetProfileString("Settings","ViewColWid", szDefault_nViewColWid);
	k = sscanf((LPCTSTR) S, "%d:%d:%d:%d", &m_nViewColWid[0], &m_nViewColWid[1], &m_nViewColWid[2], &m_nViewColWid[3]);
	if(k != 4)
	{
		// 289:42:168:195
		m_nViewColWid[0] = 289;
		m_nViewColWid[1] = 42;
		m_nViewColWid[2] = 168;
		m_nViewColWid[3] = 195;
	}

	S = pApp->GetProfileString("Settings","StartTime","0:0:0");
	k = sscanf((LPCTSTR) S, "%d:%d:%d", &m_nStartTimeSel[0], &m_nStartTimeSel[1], &m_nStartTimeSel[2]);
	if(k != 3)
	{
		m_nStartTimeSel[0] = 0;
		m_nStartTimeSel[1] = 0;
		m_nStartTimeSel[2] = 0;
	}

	S = pApp->GetProfileString("Settings","StopTime","0:0:0");
	k = sscanf((LPCTSTR) S, "%d:%d:%d", &m_nStopTimeSel[0], &m_nStopTimeSel[1], &m_nStopTimeSel[2]);
	if(k != 3)
	{
		m_nStopTimeSel[0] = 0;
		m_nStopTimeSel[1] = 0;
		m_nStopTimeSel[2] = 0;
	}

	// Load Group List
	m_GroupList.RemoveAll();
	nSize = pApp->GetProfileInt("Settings","GrpListSize", 0);

	for(k = 0; k < nSize; k++)
	{
		sprintf(szKey, "GrpList_%04d", k);
		S = pApp->GetProfileString("Settings", szKey, "");
		if(S.GetLength() > 0)
			CM_CSA_INSERT(m_GroupList, (LPCTSTR) S);
	}

	if(nSize == 0)
		m_GroupList.Add("alt.binaries.test");

	for (k = 0; k < NSERVERS; k++)
	{
		sprintf(szKey,"Server%d",k);
		servers[k].LoadServerFromRegistry(szKey);
	}

	return TRUE;
}

BOOL CProgramSettings::SaveToRegistry()
{
	// Save Class Members To Registry
	int k;
	char szKey[32];
	char szValue[256];

	CWinApp* pApp = AfxGetApp();

	SaveServerToRegistry("Settings");
	pApp->WriteProfileInt   ("Settings","Disclaimer",			m_Disclaimer);
	pApp->WriteProfileInt   ("Settings","BandwidthCap",			m_nBandwidthCap);
	pApp->WriteProfileInt   ("Settings","MaxLines",			m_nMaxLines);
	pApp->WriteProfileInt   ("Settings","HL_Last",			m_bHighlightCurrentFile);
	pApp->WriteProfileInt   ("Settings","SortOnAdd",        m_bSortFilesOnAdd);
	pApp->WriteProfileInt   ("Settings","SysTray",          m_bSysTrayIcon);
	pApp->WriteProfileInt   ("Settings","Sound",			m_bSound);
	pApp->WriteProfileInt   ("Settings","AutoRetry",        m_bAutoRetry);
	pApp->WriteProfileInt   ("Settings","DetectPAR2",        m_bDetectPAR2);
	pApp->WriteProfileInt   ("Settings","AutoShowProgress", m_bShowProgressDialogOnStart);
	pApp->WriteProfileInt   ("Settings","XNoArchiveHeader", m_bXNoArchiveHeader);
	pApp->WriteProfileInt   ("Settings","XServerDate",      m_bXServerDate);
	pApp->WriteProfileInt   ("Settings","HeaderPartial",      m_bHeaderPartial);
	pApp->WriteProfileInt   ("Settings","FileThread", m_bFileThread);
	pApp->WriteProfileString("Settings","H_From",			m_szFrom);
	pApp->WriteProfileString("Settings","H_FromNick",       m_szFromNick);
	pApp->WriteProfileString("Settings","H_ReplyTo",		m_szReplyTo);
	pApp->WriteProfileString("Settings","H_Org",            m_szOrganization);
	pApp->WriteProfileString("Settings","H_Followup",		m_szFollowupTo);
	pApp->WriteProfileString("Settings","H_DomainID",		m_szDomainID);
	pApp->WriteProfileString("Settings","LastDir",          m_szLastAddDir);
	pApp->WriteProfileString("Settings","LastTempl_yEnc",   m_szLastSubjectTemplate);
	pApp->WriteProfileInt   ("Settings","CheckStop",        m_nCheckStop);
	pApp->WriteProfileString("Settings","Skin",   m_szSkin);
	
	sprintf(szValue, "%d:%d:%d:%d", m_nViewColWid[0], m_nViewColWid[1], m_nViewColWid[2], m_nViewColWid[3]);
	pApp->WriteProfileString("Settings","ViewColWid",       szValue);

	sprintf(szValue, "%d:%d:%d", m_nStartTimeSel[0], m_nStartTimeSel[1], m_nStartTimeSel[2]);
	pApp->WriteProfileString("Settings","StartTime", szValue);

	sprintf(szValue, "%d:%d:%d", m_nStopTimeSel[0], m_nStopTimeSel[1], m_nStopTimeSel[2]);
	pApp->WriteProfileString("Settings","StopTime", szValue);

	pApp->WriteProfileInt("Settings","GrpListSize", m_GroupList.GetSize());
	for(k = 0; k < m_GroupList.GetSize(); k++)
	{
		sprintf(szKey, "GrpList_%04d", k);
		pApp->WriteProfileString("Settings", szKey, (LPCTSTR) m_GroupList.GetAt(k));
	}

	for (k = 0; k < NSERVERS; k++)
	{
		sprintf(szKey,"Server%d",k);
		servers[k].SaveServerToRegistry(szKey);
	}

	return TRUE;
}


BOOL CServerSettings::LoadServerFromRegistry(const char* ctx)
{
	CWinApp* pApp = AfxGetApp();
	CString S;
	//int k, nSize;
	//char szKey[32];
	TCHAR szValue[256];

	int   nDefault_Port                       = 119;
	char *szDefault_Server                   = m_szServerName;
	char *szDefault_Username                 = m_szUsername;
	char *szDefault_Password                 = m_szPassword;

	m_nServerPort = pApp->GetProfileInt(ctx,"NNTP_Port",	nDefault_Port);

	S = pApp->GetProfileString(ctx,"Server", szDefault_Server);
	if((S.GetLength() > 2*132) || (S.GetLength() == 0)) S = szDefault_Server;
	strcpy(m_szServerName, (LPCTSTR) S);

	m_bRequireLogin = pApp->GetProfileInt(ctx,"Login", FALSE);
	if((m_bRequireLogin != TRUE) && (m_bRequireLogin != FALSE)) m_bRequireLogin = FALSE;

	m_nConnections = pApp->GetProfileInt(ctx,"Connections", 1);
	if(m_nConnections <1) m_nConnections = 1; else if (m_nConnections>NPOSTTHREAD) m_nConnections=NPOSTTHREAD;

	S = pApp->GetProfileString(ctx,"User", szDefault_Username);
	if((S.GetLength() > 132) || (S.GetLength() == 0))  S = szDefault_Username;
	strcpy(m_szUsername, (LPCTSTR) S);

	S = pApp->GetProfileString(ctx,"Pass", szDefault_Password);
	if((S.GetLength() > 132) || (S.GetLength() == 0)) S = szDefault_Password;
	strcpy(szValue, (LPCTSTR) S);
	::CM_UnMixString(szValue, m_szPassword, 132);
	TRACE("PASSWORD READ: from '%s' TO '%s'\n", szValue, m_szPassword);
	return TRUE;
}

BOOL CServerSettings::SaveServerToRegistry(const char* ctx)
{
	// Save Class Members To Registry
	//int k;
	//char szKey[32];
	char szValue[256];

	CWinApp* pApp = AfxGetApp();

	::CM_MixString(m_szPassword, szValue, 132);
	TRACE("PASSWORD WRITE: from '%s' TO '%s'\n", m_szPassword, szValue);

	pApp->WriteProfileInt   (ctx,"NNTP_Port",		m_nServerPort);
	pApp->WriteProfileString(ctx,"Server",			m_szServerName);
	pApp->WriteProfileInt   (ctx,"Connections",		m_nConnections);
	pApp->WriteProfileInt   (ctx,"Login",			m_bRequireLogin);
	pApp->WriteProfileString(ctx,"User",				m_szUsername);
	pApp->WriteProfileString(ctx,"Pass",				szValue);

	return TRUE;
}

void CProgramSettings::PushServerSettings()
{
	int servid;

	for (servid=0;servid<NSERVERS-1;servid++)
		if (!strcmp(m_szServerName,servers[servid].m_szServerName)) break;
  while (servid>0) {
		servers[servid]=servers[servid-1];
		--servid;
	}

	servers[0] = *(CServerSettings*)this;
}

