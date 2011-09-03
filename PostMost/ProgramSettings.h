// ProgramSettings.h: interface for the CProgramSettings class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROGRAMSETTINGS_H__1E48644A_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
#define AFX_PROGRAMSETTINGS_H__1E48644A_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CServerSettings
{
public:
	int				m_nServerPort;
	char			m_szServerName[2*132];
	BOOL			m_bRequireLogin;
	char			m_szUsername[132];
	char			m_szPassword[132];
	int                     m_nConnections;
	CServerSettings();
	BOOL SaveServerToRegistry(const char* ctx);
	BOOL LoadServerFromRegistry(const char* ctx);
};

class CProgramSettings0 : public CServerSettings
{
public:

	int m_Disclaimer;

	// Server Settings
	enum { NSERVERS = 10 };
	CServerSettings servers[NSERVERS];
	int       m_nBandwidthCap;

	// Header Information

	BOOL			m_bXNoArchiveHeader;
	BOOL      m_bXServerDate;
	BOOL			m_bHeaderPartial;
	char			m_szFrom[132];
	char      m_szFromNick[64];
	char			m_szReplyTo[132];
	char			m_szOrganization[132];
	char      m_szFollowupTo[132];
	BOOL      m_bFileThread;
	char      m_szDomainID[132];
	
	// Program Settings
	int				m_nMaxLines;
	BOOL			m_bHighlightCurrentFile;
	BOOL			m_bShowProgressDialogOnStart;
	BOOL      m_bSortFilesOnAdd;
	BOOL      m_bAutoRetry;
	BOOL      m_bDefaultListViewColors;
	BOOL      m_bSysTrayIcon;
	BOOL			m_bStallTimeoutEnabled;
	int       m_nMinuteStallTimeout;
	BOOL      m_bDetectPAR2;
	char	  m_szSkin[MAX_PATH];

	// Other Information
	TCHAR           m_szLastSubjectTemplate[132];
	TCHAR           m_szLastAddDir[_MAX_PATH];
	int             m_nViewColWid[4];
	int             m_nStartTimeSel[3];
	int             m_nStopTimeSel[3];
	BOOL			m_nCheckStop;

	// Not Saved
	TCHAR           m_szXBinaryPoster[128];
};

class CProgramSettings : public CProgramSettings0
{
public:
	CProgramSettings();
	virtual ~CProgramSettings();

	void Copy(CProgramSettings& SourceSettings);
	BOOL SaveToRegistry();
	BOOL LoadFromRegistry();

	void PushServerSettings();

public:
	// Group Information
	CStringArray	m_GroupList;

};

#endif // !defined(AFX_PROGRAMSETTINGS_H__1E48644A_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
