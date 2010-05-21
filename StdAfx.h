// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__1E486437_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
#define AFX_STDAFX_H__1E486437_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions

#include <afxole.h>			// Drag & Drop Support
#include <afxmt.h>			// CriticalSection support

#include <stdio.h>
#include <string.h>

#include "mmsystem.h"

#define TRACERECT(r) { TRACE2("left = %d, top = %d, ", r.left, r.top); TRACE2("right = %d, bottom = %d\n", r.right, r.bottom); }

BOOL CM_MixString(LPCTSTR szTEXT, TCHAR* szResult, int nResultMax);
BOOL CM_UnMixString(LPCTSTR szTEXT, TCHAR* szResult, int nResultMax);
BOOL CM_Mixer(LPCTSTR szTEXT, LPCTSTR szKEY, BOOL bEncrypt, TCHAR* szResult, int nResultMax);
BOOL CM_NewsgroupSelectionAllowed(LPCTSTR szGroupName);
int  CM_CSA_INSERT(CStringArray& D, LPCTSTR szValue);
int  CM_CSA_SEEK(CStringArray& D, LPCTSTR szValue);
int  CM_CalcNumberOfParts(DWORD dwFileSize, int nLinesPerPart);
void CM_BuildArticleName(TCHAR* szArticle, LPCTSTR szSubject, LPCTSTR szFilename, int nPart, int nTotalParts);
int  CM_DWA_SEEK(CDWordArray& D, int Value);
int  CM_DWA_INSERT(CDWordArray& D, int Value);
void CM_DrawWireRects(LPRECT lprcFrom, LPRECT lprcTo, UINT nSteps, UINT nMilliSecSpeed);

BOOL CM_ReadCStringFromFile(CFile& theFile, CString& theString);
BOOL CM_ReadCDWordArrayFromFile(CFile& theFile, CDWordArray& theDWA);
BOOL CM_ReadCStringArrayFromFile(CFile& theFile, CStringArray& theCSA);
BOOL CM_ReadStringFromFile(CFile& theFile, TCHAR* rvString, int nMaxChar);

BOOL CM_WriteCStringToFile(CFile& theFile, CString& theString);
BOOL CM_WriteCDWordArrayToFile(CFile& theFile, CDWordArray& theDWA);
BOOL CM_WriteCStringArrayToFile(CFile& theFile, CStringArray& theCSA);
BOOL CM_WriteStringToFile(CFile& theFile, LPCTSTR szString);

BOOL CM_IsDirectory(LPCTSTR szDir);

void CM_strcat_FileNumber(TCHAR* szText, int nFile, int nFileOf);
void CM_strcat_FileCount(TCHAR* szText, int nFileOf);

void CM_strcat_FileSizeMegaByte(TCHAR* szText, DWORD dwBytes);
void CM_strcat_FileSizeBytes(TCHAR* szText, DWORD dwBytes);
void CM_strcat_FileSizeKiloByte(TCHAR* szText, DWORD dwBytes);

BOOL CM_BuildSubjectFromTemplate(LPCTSTR szTemplate, LPCTSTR szDirectory, LPCTSTR szFilename, DWORD dwFileSize, int nFile, int nFileOf, TCHAR* rv_szSubject);

const TCHAR* fileext(const TCHAR* fname);
const TCHAR* trimdir(const TCHAR* fname);
TCHAR* fileext(TCHAR* fname);
TCHAR* trimdir(TCHAR* fname);

#define  WM_MSG_POST				(WM_USER + 1010)
#define  WM_MSG_QUIT				(WM_USER + 1011)
#define  WM_MSG_CONNECT             (WM_USER + 1012)

#define  WM_THREAD_READY			(WM_USER + 1013)
#define  WM_THREAD_NO_CONNECTION	(WM_USER + 1014)
#define  WM_THREAD_SERVER_NOT_READY (WM_USER + 1015)
#define  WM_THREAD_UPDATE_PROGRESS  (WM_USER + 1016)
#define  WM_ENCTHREAD_READY			    (WM_USER + 1017)
#define  WM_ENCTHREAD_DONE	        (WM_USER + 1018)
#define  WM_ENCTHREAD_STOPPED       (WM_USER + 1019)
#define  WM_ENCTHREAD_UPDATE_PROGRESS (WM_USER + 1020)

#define ID_ICONNOTIFY		(WM_USER + 2800)
#define WM_ICONNOTIFY		(WM_USER + 2801)
#define WM_SENDTO_TRAY      (WM_USER + 2802)

#define PAGE__SUBJECT      ((DWORD) 0x00000001)
#define PAGE__GROUPS       ((DWORD) 0x00000002)
#define PAGE__TEXT_PREFIX  ((DWORD) 0x00000004)
#define PAGE__FILE_ORDER   ((DWORD) 0x00000008)
#define PAGE__CHECKSUMS    ((DWORD) 0x00000010)
#define PAGE__RECOVERY     ((DWORD) 0x00000020)

const int YENCLEN=128;

#include "BitmapLabel.h"
#include "TaskObject.h"
#include "ProgramSettings.h"
#include "PosterThread.h"
#include "ThreadMarshal.h"
#include "SplashWindow.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__1E486437_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
