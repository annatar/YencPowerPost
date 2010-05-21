#include "stdafx.h"

// Text Mixer - Encrypt and Decrypt
// Original VB version written by Bob Richards & Richard Fratz

BOOL CM_MixString(LPCTSTR szTEXT, TCHAR* szResult, int nResultMax)
{
	return CM_Mixer(szTEXT, "GHENT", TRUE, szResult, nResultMax);
}

BOOL CM_UnMixString(LPCTSTR szTEXT, TCHAR* szResult, int nResultMax)
{
	return CM_Mixer(szTEXT, "GHENT", FALSE, szResult, nResultMax);
}

BOOL CM_Mixer(LPCTSTR szTEXT, LPCTSTR szKEY, BOOL bEncrypt, TCHAR* szResult, int nResultMax)
{
	if((szTEXT == NULL) || (strlen(szTEXT) == 0) || (szKEY == NULL) || (strlen(szKEY) == 0))
	{
		*szResult = '\0';
		return FALSE;
	}
	
	int i, nn, nPos, nLen;
	TCHAR ch[2];
	TCHAR szFROM[] = "~}|{zyxwvutsrqponmlkjihgfedcba`_^]\\[ZYXWVUTSRQPONMLKJIHGFEDCBA@?>=<;:9876543210/.-,+*)('&%$#\"! ";
	TCHAR szTOO[1024];

	ch[1] = '\0';
	::ZeroMemory(szTOO, 1024);
	
	// Insert KEY part
	nLen = strlen(szKEY);
	for(i = 0; i < nLen; i++)
	{
		ch[0] = szKEY[i];
		if(strchr(szTOO, ch[0]) == NULL)
			strcat(szTOO, ch);
	}

	// Insert rest of szFROM
	nLen = strlen(szFROM);
	for(i = 0; i < nLen; i++)
	{
		ch[0] = szFROM[i];
		if(strchr(szTOO, ch[0]) == NULL)
			strcat(szTOO, ch);
	}

	// Encrypt the data
	szResult[0] = '\0';
	nPos = 0;
	TCHAR* p = NULL;

	nLen = strlen(szTEXT);
	for(i = 0; i < nLen; i++)
	{
		ch[0] = szTEXT[i];
		if(bEncrypt)
		{
			p = strchr(szFROM, ch[0]);
			if(p != NULL)
			{
				nn = (int) (p - szFROM);
				ch[0] = szTOO[nn];
			}
		}
		else
		{
			p = strchr(szTOO, ch[0]);
			if(p != NULL)
			{
				nn = (int) (p - szTOO);
				ch[0] = szFROM[nn];
			}
		}
		strcat(szResult, ch);
	}
	return TRUE;
}

BOOL CM_NewsgroupSelectionAllowed(LPCTSTR szGroupName)
{
	// Do not allow ".d" or ".request" or ".requests" groups
	TCHAR szTT[1024];
	int slg = strlen(szGroupName);

	if(slg > 2)
	{
		if(strcmp(&szGroupName[slg-2], ".d") == 0)
		{
			sprintf(szTT, "Newsgroups: %s\r\n\r\nNo posting to discussion groups!", szGroupName);
			AfxMessageBox(szTT, MB_ICONEXCLAMATION);
			return FALSE;
		}
	}
	if(slg > 8)
	{
		if(strcmp(&szGroupName[slg-8], ".request") == 0)
		{
			sprintf(szTT, "Newsgroups: %s\r\n\r\nNo posting to request groups!", szGroupName);
			AfxMessageBox(szTT, MB_ICONEXCLAMATION);
			return FALSE;
		}
	}
	if(slg > 9)
	{
		if(strcmp(&szGroupName[slg-9], ".requests") == 0)
		{
			sprintf(szTT, "Newsgroups: %s\r\n\r\nNo posting to requests groups!", szGroupName);
			AfxMessageBox(szTT, MB_ICONEXCLAMATION);
			return FALSE;
		}
	}
	if(slg > 11)
	{
		// filter *.discussion
		if(strcmp(&szGroupName[slg-11], ".discussion") == 0)
		{
			sprintf(szTT, "Newsgroups: %s\r\n\r\nNo posting to request groups!", szGroupName);
			AfxMessageBox(szTT, MB_ICONEXCLAMATION);
			return FALSE;
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Ordered CStringArray Functions

// Returns array index i >= 0 such that D[i] == Value; -1 otherwise
int CM_CSA_SEEK(CStringArray& D, LPCTSTR szValue)
{
    int last = D.GetSize() - 1;
    if(last < 0) return -1; // Empty
    // if((last == 0) && (D[0] == V)) return 0;
	if((last == 0) && (strcmp((LPCTSTR) D.GetAt(0), szValue) == 0))
		return(0);
    int first, flagFound, idx;

    first = 0;
    flagFound = FALSE;
	int sr;				// search result

    while((first <= last) && !flagFound)
    {
        idx = (first + last) >> 1;
		sr = strcmp((LPCTSTR) D.GetAt(idx), szValue);

        if(sr == 0) 
			flagFound = TRUE;
        else
        {
            // if( D[idx] > V )
			if(sr > 0)
            {
                last = idx - 1;
            }
            else
            {
                first = idx + 1;
            }
        }
    }

    if(!flagFound) 
        idx = -1;
    else 
        // while((idx > 0) && (D[idx - 1] == V)) 
		while( (idx > 0) && (strcmp((LPCTSTR) D.GetAt(idx-1), szValue) == 0))
            --idx;

    return idx;
}

int CM_CSA_INSERT(CStringArray& D, LPCTSTR szValue)
{
	// Get size of current set
    int sz = D.GetSize();

    // Handle Trivial Case
    if(sz == 0)
    {
        D.Add(szValue);
        return(0);
    }

    // Seek for value; return if value exists
    int i = CM_CSA_SEEK(D, szValue);
    if(i >= 0) return(i);

    // Insert Value
    for(i = 0; i < sz; i++)
    {
        // if(D[i] < V) continue;
		if(strcmp((LPCTSTR) D.GetAt(i), szValue) < 0) continue;

        D.InsertAt(i, szValue);
        return(i);
    }
    
    // Append (Largest Value)
    D.Add(szValue);
    return(sz);
}

int CM_CalcNumberOfParts(DWORD dwFileSize, int nLinesPerPart)
{
	// Each encoded lines holds up to YENCLEN bytes of actual data
	DWORD nLines = dwFileSize / YENCLEN;
	if((dwFileSize % YENCLEN) > 0) ++nLines;

	DWORD nParts = nLines / nLinesPerPart;
	if((nLines % nLinesPerPart) > 0)
		++nParts;

	return((int) nParts);

}

void CM_BuildArticleName(TCHAR* szArticle, LPCTSTR szSubject, LPCTSTR szFilename, int nPart, int nTotalParts)
{
	int n;
	BOOL  bFilenameFound = FALSE;
	TCHAR szArticlePart[32];

	// Build Article Part Number
	::ZeroMemory(szArticlePart, 32);
	if(nTotalParts < 10) { sprintf(szArticlePart, " (%d/%d)", nPart, nTotalParts); }
	else {
		if(nTotalParts < 100) { sprintf(szArticlePart, " (%02d/%02d)", nPart, nTotalParts); }
		else {
			if(nTotalParts < 1000) { sprintf(szArticlePart, " (%03d/%03d)", nPart, nTotalParts); }
			else {
				if(nTotalParts < 10000) sprintf(szArticlePart, " (%04d/%04d)", nPart, nTotalParts);
				else sprintf(szArticlePart, " (%05d/%05d)", nPart, nTotalParts);
			}
		}
	}

	strcpy(szArticle, szSubject);
	n = strlen(szArticle);
	while((n > 0) && (szArticle[n-1] == ' '))
	{
		szArticle[n-1] = '\0';
		--n;
	}
	strcat(szArticle, szArticlePart);
}

/////////////////////////////////////////////////////////////////////////////
// Ordered CDWordArray Functions

// Returns array index i >= 0 such that D[i] == Value; -1 otherwise
int CM_DWA_SEEK(CDWordArray& D, int Value)
{
    int last = D.GetSize() - 1;
    if(last < 0) return -1; // Empty
    DWORD V = (DWORD) Value;
    if((last == 0) && (D[0] == V)) return 0;
    int first, flagFound, idx;

    first = 0;
    flagFound = FALSE;

    while((first <= last) && !flagFound)
    {
        idx = (first + last) >> 1;
        if(D[idx] == V) flagFound = TRUE;
        else
        {
            if( D[idx] > V )
            {
                last = idx - 1;
            }
            else
            {
                first = idx + 1;
            }
        }
    }

    if(!flagFound) 
        idx = -1;
    else 
        while((idx > 0) && (D[idx - 1] == V)) 
            --idx;

    return idx;
}

int CM_DWA_INSERT(CDWordArray& D, int Value)
{
    DWORD V = (DWORD) Value;

    int sz = D.GetSize();

    // Trivial Case
    if(sz == 0)
    {
        D.Add(V);
        return(0);
    }

    // Seek for value; return if value exists
    int i = CM_DWA_SEEK(D, Value);
    if(i >= 0) return(i);

    // Insert Value
    for(i = 0; i < sz; i++)
    {
        if(D[i] < V) continue;
        D.InsertAt(i, V);
        return(i);
    }
    
    // Append (Largest Value)
    D.Add(V);
    return(sz);
}

///////////////////////////////////////////////////////////////////////////////////
//// FUNCTION:	DrawWireRects//// DESCRIPTION:	Creates exploding wire rectanges//
//  INPUTS: LPRECT lprcFrom      Source Rectangle
//          LPRECT lprcTo        Destination Rectangle
//          UINT nMilliSecSpeed  Speed in millisecs for animation//
//	RETURN:	None//	NOTES:	None////  Maintenance Log
//  Author      Date    Version     Notes
//  NT Almond   011199  1.0         Origin
//  CJ Maunder  010899  1.1         Modified rectangle transition code//
////////////////////////////////////////////////////////////////////////////////
void CM_DrawWireRects(LPRECT lprcFrom, LPRECT lprcTo, UINT nSteps, UINT nMilliSecSpeed)
{
	int nNumSteps = (int) nSteps;

	GdiFlush();

    Sleep(50);
	
	HDC hDC = ::GetDC(NULL);

    HPEN hPen = ::CreatePen(PS_DOT, 0, RGB(0,0,0));

	int nMode = ::SetROP2(hDC, R2_XORPEN);

	HPEN hOldPen = (HPEN) ::SelectObject(hDC, hPen);
	POINT pt[5];
	for (int i = 0; i < nNumSteps; i++)	
	{
        double dFraction = (double) i / (double) nNumSteps;
        RECT transition;
        transition.left   = lprcFrom->left + (int)((lprcTo->left - lprcFrom->left) * dFraction);
        transition.right  = lprcFrom->right + (int)((lprcTo->right - lprcFrom->right) * dFraction);
        transition.top    = lprcFrom->top + (int)((lprcTo->top - lprcFrom->top) * dFraction);
        transition.bottom = lprcFrom->bottom + (int)((lprcTo->bottom - lprcFrom->bottom) * dFraction);
		
		pt[0] = CPoint(transition.left, transition.top);
		pt[1] = CPoint(transition.right,transition.top);
		pt[2] = CPoint(transition.right,transition.bottom);
		pt[3] = CPoint(transition.left, transition.bottom);
		pt[4] = CPoint(transition.left, transition.top);

		// Draw Sides
		::Polyline(hDC,pt,5);		
		GdiFlush();		
		Sleep(nMilliSecSpeed);		
		// UnDraw Sides
		::Polyline(hDC,pt,5);		
		GdiFlush();	
	}

	::SetROP2(hDC, nMode);
	::SelectObject(hDC, hOldPen);	
	::ReleaseDC(NULL,hDC);
}

BOOL CM_ReadCStringFromFile(CFile& theFile, CString& theString)
{
	theString.Empty();

	UINT nLength;
	if(theFile.Read(&nLength, sizeof(UINT)) != 4) return FALSE;

	if(nLength > 0)
	{
		TCHAR* pString = theString.GetBufferSetLength(nLength);
		if(pString == NULL) return FALSE;
		if(theFile.Read(pString, nLength) != nLength) return FALSE;
	}

	return TRUE;
}

BOOL CM_ReadCDWordArrayFromFile(CFile& theFile, CDWordArray& theDWA)
{
	UINT nSize;
	
	theDWA.RemoveAll();

	if(theFile.Read(&nSize, sizeof(UINT)) != sizeof(UINT)) return FALSE;

	theDWA.SetSize((int) nSize);
	
	if(nSize > 0)
	{
		DWORD* pData = theDWA.GetData();
		if(pData == NULL) return FALSE;
		if(theFile.Read(pData, nSize * sizeof(DWORD)) != (nSize * sizeof(DWORD))) return FALSE;
	}
	return TRUE;
}

BOOL CM_ReadCStringArrayFromFile(CFile& theFile, CStringArray& theCSA)
{
	int nSize;

	theCSA.RemoveAll();

	if(theFile.Read(&nSize, sizeof(int)) != sizeof(int)) return FALSE;

	int nGrowBy = nSize;
	
	if(nGrowBy == 0) nGrowBy = 1;

	theCSA.SetSize(0, nGrowBy);

	CString strText;

	if(nSize > 0)
	{
		int k;
		for(k = 0; k < nSize; k++)
		{
			if(!CM_ReadCStringFromFile(theFile, strText)) return FALSE;
			theCSA.Add((LPCTSTR) strText);
		}
	}

	return TRUE;
}

BOOL CM_ReadStringFromFile(CFile& theFile, TCHAR* rvString, int nMaxChar)
{
	int nLength;
	if(theFile.Read(&nLength, sizeof(int)) != sizeof(int)) return FALSE;
	if(nLength > nMaxChar) return FALSE;
	if(theFile.Read(rvString, nLength) != (UINT) nLength) return FALSE;
	return TRUE;
}

BOOL CM_WriteCStringToFile(CFile& theFile, CString& theString)
{
	// Write Length First
	UINT nLength = theString.GetLength();
	theFile.Write(&nLength, sizeof(UINT));

	// Write String Data
	if(nLength > 0)
		theFile.Write((LPCTSTR) theString, nLength);

	return TRUE;
}

BOOL CM_WriteCDWordArrayToFile(CFile& theFile, CDWordArray& theDWA)
{
	UINT nSize = (UINT) theDWA.GetSize();

	theFile.Write(&nSize, sizeof(UINT));

	if(nSize > 0)
	{
		DWORD* pData = theDWA.GetData();
		if(pData == NULL) return FALSE;
		theFile.Write(pData, nSize * sizeof(DWORD));
	}

	return TRUE;
}

BOOL CM_WriteCStringArrayToFile(CFile& theFile, CStringArray& theCSA)
{
	int nSize = theCSA.GetSize();

	theFile.Write(&nSize, sizeof(int));

	if(nSize > 0)
	{
		for(int k = 0; k < nSize; k++)
			CM_WriteCStringToFile(theFile, theCSA.GetAt(k));
	}

	return TRUE;
}

BOOL CM_WriteStringToFile(CFile& theFile, LPCTSTR szString)
{
	int nLength = strlen(szString) + 1;
	theFile.Write(&nLength, sizeof(int));
	theFile.Write(szString, nLength);
	return TRUE;
}

BOOL CM_IsDirectory(LPCTSTR szDir)
{
	TCHAR szT[_MAX_PATH+1];
	
	strcpy(szT, szDir);
	
	int sl = strlen(szT);
	if(sl == 0) return FALSE;
	
	if(szT[sl-1] != '\\') strcat(szT, "\\*.*");
	else strcat(szT, "*.*");
	
	CFileFind FF;

	if(FF.FindFile(szT) == TRUE)
	{
		FF.Close();
		return TRUE;
	}

	DWORD dwLastError = GetLastError();

	FF.Close();

	if(dwLastError == ERROR_FILE_NOT_FOUND) return TRUE;
	if(dwLastError == ERROR_NOT_READY) 		return TRUE;

	return FALSE;
}

void CM_strcat_FileSizeMegaByte(TCHAR* szText, DWORD dwBytes)
{
	double dbl_megs = (double) dwBytes;
	dbl_megs = dbl_megs / (double) 1048576.0;

	TCHAR szAppendString[64];
	sprintf(szAppendString, "%.1lf", dbl_megs);
	strcat(szText, szAppendString);
}

void CM_strcat_FileSizeBytes(TCHAR* szText, DWORD dwBytes)
{
	TCHAR szAppendString[64];
	sprintf(szAppendString, "%d", dwBytes);
	strcat(szText, szAppendString);
}

void CM_strcat_FileSizeKiloByte(TCHAR* szText, DWORD dwBytes)
{
	TCHAR szAppendString[64];
	if(dwBytes == 0)
		strcpy(szAppendString, "0");
	else
	{
		if(dwBytes < 1024)
			strcpy(szAppendString, "1");
		else
			sprintf(szAppendString, "%d", dwBytes >> 10);
	}
	strcat(szText, szAppendString);
}

void CM_strcat_FileNumber(TCHAR* szText, int nFile, int nFileOf)
{
	TCHAR szNumber[32];

	if(nFileOf < 10)
		sprintf(szNumber, "%d", nFile);
	else if(nFileOf < 100)
	{
		sprintf(szNumber, "%02d", nFile);
	}
	else if (nFileOf < 1000)
	{
		sprintf(szNumber, "%03d", nFile);
	}
	else if (nFileOf < 10000)
	{
		sprintf(szNumber, "%04d", nFile);
	}
	else
	{
		sprintf(szNumber, "%05d", nFile);
	}

	strcat(szText, szNumber);
}

void CM_strcat_FileCount(TCHAR* szText, int nFileOf)
{
	TCHAR szNumber[32];
	sprintf(szNumber, "%d", nFileOf);
	strcat(szText, szNumber);
}


//
// Returns TRUE if $F used; false otherwise..
//
BOOL CM_BuildSubjectFromTemplate(LPCTSTR szTemplate, 
								 LPCTSTR szDirectory,
								 LPCTSTR szFilename, 
								 DWORD dwFileSize, 
								 int nFile, 
								 int nFileOf, 
								 TCHAR* rv_szSubject)
{
	int nPosT = 0;	// Position in Template String
	int nPosS = 0;  // Position in Subject String
	BOOL bFileUsed = FALSE;		// Flag (TRUE/FALSE): Was $F used?
	rv_szSubject[0] = '\0';

	while(szTemplate[nPosT] != '\0')
	{
		if(szTemplate[nPosT] != '$')
		{
			rv_szSubject[nPosS] = szTemplate[nPosT];
			++nPosS;
			++nPosT;
			continue;
		}

		switch(szTemplate[nPosT+1])
		{
		case '\0':
			rv_szSubject[nPosS] = '\0';
			return(bFileUsed);
		case '$': 
			rv_szSubject[nPosS] = '$';
			++nPosS;
			++nPosT;
			break;
		case '1':
			rv_szSubject[nPosS] = '\0';
			CM_strcat_FileNumber(rv_szSubject, nFile, nFileOf);
			nPosS = strlen(rv_szSubject);
			++nPosT;
			break;
		case '2':
			rv_szSubject[nPosS] = '\0';
			CM_strcat_FileCount(rv_szSubject, nFileOf);
			nPosS = strlen(rv_szSubject);
			++nPosT;
			break;
		case 'p':
		case 'P':
			rv_szSubject[nPosS] = '\0';
			strcat(rv_szSubject, szDirectory);
			nPosS = strlen(rv_szSubject);
			++nPosT;
			break;			
		case 'd':
		case 'D':
			rv_szSubject[nPosS] = '\0';
			{
			  const char * first = szDirectory+strlen(szDirectory)-1;
			  while (first>szDirectory && first[-1]!='/' && first[-1]!='\\')
			    --first;
			  strcat(rv_szSubject, first);
			  nPosS = strlen(rv_szSubject);
			  ++nPosT;
			}
			break;			
		case 'F':
		case 'f':
			bFileUsed = TRUE;
			rv_szSubject[nPosS] = '\0';
			strcat(rv_szSubject, szFilename);
			nPosS = strlen(rv_szSubject);
			++nPosT;
			break;
		case 'B':
		case 'b':
			rv_szSubject[nPosS] = '\0';
			CM_strcat_FileSizeBytes(rv_szSubject, dwFileSize);
			nPosS = strlen(rv_szSubject);
			++nPosT;
			break;
		case 'K':
		case 'k':
			rv_szSubject[nPosS] = '\0';
			CM_strcat_FileSizeKiloByte(rv_szSubject, dwFileSize);
			nPosS = strlen(rv_szSubject);
			++nPosT;
			break;
		case 'M':
		case 'm':
			rv_szSubject[nPosS] = '\0';
			CM_strcat_FileSizeMegaByte(rv_szSubject, dwFileSize);
			nPosS = strlen(rv_szSubject);
			++nPosT;
			break;
		default:
			break;
		}

		++nPosT;
	}

	rv_szSubject[nPosS] = '\0';
	return(bFileUsed);
}

const TCHAR* fileext(const TCHAR* fname)
{
	const TCHAR *e=fname;
	while (*fname)
	{
		if (*fname=='.') e=fname;
		++fname;
	}
	return e;
}

TCHAR* fileext(TCHAR* fname)
{
	TCHAR *e=fname;
	while (*fname)
	{
		if (*fname=='.') e=fname;
		++fname;
	}
	return e;
}

const TCHAR* trimdir(const TCHAR* fname)
{
	const TCHAR *e=fname;
	while (*fname)
	{
		if (*fname=='/' || *fname=='\\') e=fname+1;
		++fname;
	}
	return e;
}

TCHAR* trimdir(TCHAR* fname)
{
	TCHAR *e=fname;
	while (*fname)
	{
		if (*fname=='/' || *fname=='\\') e=fname+1;
		++fname;
	}
	return e;
}
