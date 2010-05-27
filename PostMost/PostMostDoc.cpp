// PostMostDoc.cpp : implementation of the CPostMostDoc class
//

#include "stdafx.h"
#include "PostMost.h"

#include "PostMostDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPostMostDoc

IMPLEMENT_DYNCREATE(CPostMostDoc, CDocument)

BEGIN_MESSAGE_MAP(CPostMostDoc, CDocument)
	//{{AFX_MSG_MAP(CPostMostDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPostMostDoc construction/destruction

CPostMostDoc::CPostMostDoc()
{
	// TODO: add one-time construction code here

}

CPostMostDoc::~CPostMostDoc()
{
}

BOOL CPostMostDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CPostMostDoc serialization

void CPostMostDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPostMostDoc diagnostics

#ifdef _DEBUG
void CPostMostDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPostMostDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPostMostDoc commands
