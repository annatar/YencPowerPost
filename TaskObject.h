// TaskObject.h: interface for the CTaskObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TASKOBJECT_H__DFB15EA3_D803_11D3_B80A_00A0CC215BD8__INCLUDED_)
#define AFX_TASKOBJECT_H__DFB15EA3_D803_11D3_B80A_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// POST THE MOST! Task Object

class CTaskObject  
{
public:
	CTaskObject();
	virtual ~CTaskObject();

	enum { PM_UNDEFINED, PM_QUEUED, PM_ERROR, PM_POSTED, PM_HOLDING, PM_POSTING, PM_ENC_QUEUED, PM_ENCODING, PM_ENC_HOLDING };

public:

	//
	// Attributes
	//

	int   m_nState;						// State or Result Code (see enumeration above)

	BOOL  m_bHistoryDeleted;			// Flag (TRUE/FALSE): Deleted in history?
	TCHAR m_szHistoryDateTime[20];		// History Date/Time

	ULONGLONG m_ullFileSize;			// Size Of File
	TCHAR m_szFilename[_MAX_PATH];		// Name Of File To Post
	TCHAR m_szSubject[_MAX_PATH];		// Subject Line (minus the " (k/n)" suffix

	BOOL    m_bPostPrefixText;			// Flag: Post Prefix Text (TRUE/FALSE)
	CString m_strTextPrefix;			// Text Prefix (part 0/n) (optional)

	//int m_nMaxLines;					// Number of Lines Per Post
	int m_nPartSize;					// Bytes Per Post

	bool postPrefixText() { return m_strTextPrefix.GetLength()>0; }
	int getNbParts() { return (m_ullFileSize+m_nPartSize-1)/m_nPartSize; }
	int getTotalParts() { int n = getNbParts(); if (postPrefixText()) ++n; return n; }
	int getMaxLines() { return (m_nPartSize+YENCLEN-1)/YENCLEN; }

	CString m_strGroups;				// Target Newsgroup(s) (if multiple, comma seperated with no spaces)

	CDWordArray m_dwaPartsToPost;		// If m_bPostAllParts == FALSE, this contains list of parts to post

	CStringArray m_Log;					// Posting Log

	TCHAR m_szLastResult[1024];			// Last Posting Result
	//
	// Operations
	//

	void Reset();							// Reset Object to PM_UNDEFINED state
	void Copy(CTaskObject* pSource);		// Copy contents of another CTaskObject

	// helper functions
	// Create a message ID if none already done
	CString GenerateMessageID(CString domainID);
	CString GenerateMessageID(int partNum, int nParts, CString domainID);

	//CString getMessageID();
	CString getMessageID();
	CString getMessageID(int partNum, int nParts);

	CString getReferences(BOOL bFirst, int nParts);
	//CString getShortReferences();

	void setMessageID(CString val);
	void setReferences(CString val);
	void ResetLog();

	void SetModified();

	BOOL ReadFromFile(CFile& theFile);
	BOOL WriteToFile(CFile& theFile, BOOL bSetTime);

};

#endif // !defined(AFX_TASKOBJECT_H__DFB15EA3_D803_11D3_B80A_00A0CC215BD8__INCLUDED_)
