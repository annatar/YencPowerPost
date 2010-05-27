// TaskObject.cpp: implementation of the CTaskObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PostMost.h"
#include "TaskObject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTaskObject::CTaskObject()
{
	Reset();							// Reset Object to PM_UNDEFINED state
}

CTaskObject::~CTaskObject()
{

}

void CTaskObject::Reset()
{
	m_nState = PM_UNDEFINED;
	m_bHistoryDeleted = FALSE;
	::ZeroMemory(m_szHistoryDateTime, 20);
	m_ullFileSize = 0;
	::ZeroMemory(m_szFilename, _MAX_PATH);
	::ZeroMemory(m_szSubject, _MAX_PATH);
	m_bPostPrefixText = TRUE;
	m_strTextPrefix.Empty();
	m_strGroups.Empty();
	m_dwaPartsToPost.RemoveAll();
	m_Log.RemoveAll();

	::ZeroMemory(m_szLastResult, 1024);

}

const CString HDR_MessageID = "%Message-ID: ";

// See: http://www.faerber.muc.de/temp/20020304-partial-message-ids.html
const char* MessageID_Format = "<part%dof%d.%s>";
const CString HDR_References = "%References: ";

void CTaskObject::SetModified()
{
	::ZeroMemory(m_szHistoryDateTime, 20);
}

extern void init_genrand(unsigned long s);
extern void init_by_array(unsigned long* init_key, unsigned long key_length);;
extern unsigned long genrand_int32(void);

const char* generateUniqueID()
{
	unsigned long hash[4];

	static int inited=0;

	if (!inited)
	{
		unsigned char initdata[624];
		unsigned char* cur = initdata;
		cur = (unsigned char*)strcpy((char*)cur,TITLE);
		GetSystemTimeAsFileTime((LPFILETIME)cur); cur+=sizeof(FILETIME);

		init_by_array((unsigned long*) initdata, cur-initdata);

		inited=1;
	}

	hash[0]=genrand_int32();
	hash[1]=genrand_int32();
	hash[2]=genrand_int32();
	hash[3]=genrand_int32();
	
	//((unsigned char*)hash)[7] &= 0x0F;
	//((unsigned char*)hash)[7] |= 0x30;
	//((unsigned char*)hash)[8] &= 0x3F;
	//((unsigned char*)hash)[8] |= 0x80;


	static char digits[65]="0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ$&";
	//static char sep[5]="/*-+";

	static char str[21];

	for (int i=0;i<4;i++)
	{
		str[5*i+0]=digits[(hash[i]>>(32-6))&63];
		str[5*i+1]=digits[(hash[i]>>(32-12))&63];
		str[5*i+2]=digits[(hash[i]>>(32-18))&63];
		str[5*i+3]=digits[(hash[i]>>(32-24))&63];
		str[5*i+4]=digits[(hash[i]>>(32-30))&63];
	}
	str[20]='\0';
	return str;
}

CString CTaskObject::GenerateMessageID(CString domainID)
{
	CString mid = getMessageID();
	if (mid.GetLength()>0) return mid;
	//mid = HDR_MessageID;
	mid=generateUniqueID();
	mid+= '@';
	mid+= domainID;
	m_Log.Add(HDR_MessageID+mid);
	return mid;
}

CString CTaskObject::GenerateMessageID(int partNum, int nParts, CString domainID)
{
	CString mid = GenerateMessageID(domainID);
	CString result;
	result.Format(MessageID_Format,partNum+1, nParts, (LPCSTR)mid);
	return result;
}

CString CTaskObject::getMessageID()
{
	for (int i=0;i<m_Log.GetSize();i++)
	{
		CString mid = m_Log.GetAt(i);
		if (mid.Left(HDR_MessageID.GetLength()).CompareNoCase(HDR_MessageID) == 0)
		{
			return mid.Right(mid.GetLength()-HDR_MessageID.GetLength());
		}
	}
	return CString("");
}

CString CTaskObject::getMessageID(int partNum, int nParts)
{
	CString mid = getMessageID();
	if (mid.GetLength()<=0) return mid;
	CString result;
	result.Format(MessageID_Format,partNum+1, nParts, (LPCSTR)mid);
	return result;
}

CString CTaskObject::getReferences(BOOL bFirst, int nParts)
{
	for (int i=0;i<m_Log.GetSize();i++)
	{
		CString str = m_Log.GetAt(i);
		if (str.Left(HDR_References.GetLength()).CompareNoCase(HDR_References) == 0)
		{
			CString result;
			result = str.Right(str.GetLength()-HDR_References.GetLength());
			if (bFirst)
			{
				CString mid=getMessageID(0,nParts);
				if (mid.GetLength()>0)
				{
					result += " ";
					result += mid;
				}
			}
			return result;
		}
	}
	if (bFirst)
		return getMessageID(0,nParts);
	else return CString("");
}

void CTaskObject::setMessageID(CString val)
{
	val = HDR_MessageID+val;
	for (int i=0;i<m_Log.GetSize();i++)
	{
		CString str = m_Log.GetAt(i);
		if (str.Left(HDR_MessageID.GetLength()).CompareNoCase(HDR_MessageID) == 0)
		{
			m_Log.SetAt(i,val);
			return;
		}
	}
	m_Log.Add(val);
}

void CTaskObject::setReferences(CString val)
{
	val = HDR_References+val;
	for (int i=0;i<m_Log.GetSize();i++)
	{
		CString str = m_Log.GetAt(i);
		if (str.Left(HDR_References.GetLength()).CompareNoCase(HDR_References) == 0)
		{
			m_Log.SetAt(i,val);
			return;
		}
	}
	m_Log.Add(val);
}

void CTaskObject::ResetLog()
{
	// special headers are hacked into log, so we need to hack a little but...
	CString mid = getMessageID();
	CString ref = getReferences(TRUE,0);
	m_Log.RemoveAll();
	if (mid.GetLength()>0) setMessageID(mid);
	if (ref.GetLength()>0) setReferences(ref);
}

/*
	File Format - Records Of The Following Layout:

	BOOL			m_bHistoryDeleted
	char[20]		Date/Time Of History Archiving
	int				m_nState;
	DWORD			m_dwFileSize;
	TCHAR			m_szFilename[_MAX_PATH];
	TCHAR			m_szSubject[_MAX_PATH];
	int				m_nMaxLines;
	BOOL			m_bPostPrefixText;
	CString			m_strTextPrefix;
	CString			m_strGroups;
	CDWordArray		m_dwaPartsToPost;
	CStringArray	m_Log;

	CM_ReadCStringFromFile(CFile& theFile, CString& theString);
	CM_ReadCDWordArrayFromFile(CFile& theFile, CDWordArray& theDWA);
	CM_ReadCStringArrayFromFile(CFile& theFile, CStringArray& theCSA);

	CM_WriteCStringToFile(CFile& theFile, CString& theString);
	CM_WriteCDWordArrayToFile(CFile& theFile, CDWordArray& theDWA);
	CM_WriteCStringArrayToFile(CFile& theFile, CStringArray& theCSA);

*/

BOOL CTaskObject::ReadFromFile(CFile& theFile)
{
	Reset();

	if(theFile.Read(&m_bHistoryDeleted, sizeof(BOOL)) != sizeof(BOOL)) return FALSE;
	if(theFile.Read(m_szHistoryDateTime, 20) != 20) return FALSE;
	if(theFile.Read(&m_nState, sizeof(int)) != sizeof(int)) return FALSE;
	if(theFile.Read(&m_ullFileSize, sizeof(ULONGLONG)) != sizeof(ULONGLONG)) return FALSE;
	if(theFile.Read(m_szFilename, _MAX_PATH) != _MAX_PATH) return FALSE;
	if(theFile.Read(m_szSubject, _MAX_PATH) != _MAX_PATH) return FALSE;
	int maxlines;
	if(theFile.Read(&maxlines, sizeof(int)) != sizeof(int)) return FALSE;
	if (maxlines>0) m_nPartSize = maxlines*YENCLEN; else m_nPartSize=-maxlines;

	if(theFile.Read(&m_bPostPrefixText, sizeof(BOOL)) != sizeof(BOOL)) return FALSE;
	if(CM_ReadCStringFromFile(theFile, m_strTextPrefix) == FALSE) return FALSE;
	if(CM_ReadCStringFromFile(theFile, m_strGroups) == FALSE) return FALSE;
	if(CM_ReadCDWordArrayFromFile(theFile, m_dwaPartsToPost) == FALSE) return FALSE;
	if(CM_ReadCStringArrayFromFile(theFile, m_Log) == FALSE) return FALSE;
	if(CM_ReadStringFromFile(theFile, m_szLastResult, 1024) == FALSE) return FALSE;
	return TRUE;
}

BOOL CTaskObject::WriteToFile(CFile& theFile, BOOL bSetTime)
{
	BOOL  bValue = FALSE;

	if(bSetTime)
	{
		CTime theTime = CTime::GetCurrentTime();
		sprintf(m_szHistoryDateTime, "%04d-%02d-%02d at %02d:%02d", theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), theTime.GetHour(), theTime.GetMinute());
	}

	try
	{
		theFile.Write(&bValue, sizeof(BOOL));		// Write m_bHistoryDeleted flag as FALSE
		theFile.Write(m_szHistoryDateTime, 20);
		theFile.Write(&m_nState, sizeof(int));
		theFile.Write(&m_ullFileSize, sizeof(ULONGLONG));
		theFile.Write(m_szFilename, _MAX_PATH);
		theFile.Write(m_szSubject, _MAX_PATH);
		int maxlines;
		if (m_nPartSize%YENCLEN==0) maxlines = m_nPartSize/YENCLEN; else maxlines=-m_nPartSize;
		theFile.Write(&maxlines, sizeof(int));
		theFile.Write(&m_bPostPrefixText, sizeof(BOOL));
		CM_WriteCStringToFile(theFile, m_strTextPrefix);
		CM_WriteCStringToFile(theFile, m_strGroups);
		CM_WriteCDWordArrayToFile(theFile, m_dwaPartsToPost);
		CM_WriteCStringArrayToFile(theFile, m_Log);
		CM_WriteStringToFile(theFile, m_szLastResult);
	}
	catch(CFileException* fe)
	{
		fe->Delete();
		Reset();
		return FALSE;
	}
	
	return TRUE;
}

void CTaskObject::Copy(CTaskObject* pSource)
{
	Reset();

	m_nState = pSource->m_nState;

	m_ullFileSize = pSource->m_ullFileSize;

	::CopyMemory(m_szFilename , pSource->m_szFilename, _MAX_PATH);
	::CopyMemory(m_szSubject , pSource->m_szSubject, _MAX_PATH);

	m_bPostPrefixText = pSource->m_bPostPrefixText;

	m_strTextPrefix = (LPCTSTR) pSource->m_strTextPrefix;

	m_nPartSize = pSource->m_nPartSize;

	m_strGroups = (LPCTSTR) pSource->m_strGroups;

	if(pSource->m_dwaPartsToPost.GetSize() > 0)
		m_dwaPartsToPost.Copy(pSource->m_dwaPartsToPost);

	if(pSource->m_Log.GetSize() > 0)
		m_Log.Copy(pSource->m_Log);

	::CopyMemory(m_szLastResult, pSource->m_szLastResult, 1024);
	::CopyMemory(m_szHistoryDateTime, pSource->m_szHistoryDateTime, 20);
}
