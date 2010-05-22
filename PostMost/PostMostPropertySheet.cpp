// PostMostPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "PostMostPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPostMostPropertySheet

IMPLEMENT_DYNAMIC(CPostMostPropertySheet, CPropertySheet)

CPostMostPropertySheet::CPostMostPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	m_rectAnimateFrom.SetRectEmpty();
}

CPostMostPropertySheet::CPostMostPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	m_rectAnimateFrom.SetRectEmpty();
}

CPostMostPropertySheet::~CPostMostPropertySheet()
{
}


BEGIN_MESSAGE_MAP(CPostMostPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CPostMostPropertySheet)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPostMostPropertySheet message handlers

void CPostMostPropertySheet::OnDestroy() 
{
	CRect rectWindow;
	GetWindowRect(rectWindow);

	CPropertySheet::OnDestroy();
	
	if(!m_rectAnimateFrom.IsRectEmpty())
		::CM_DrawWireRects(&rectWindow, &m_rectAnimateFrom, 16, 15);
}


BOOL CPostMostPropertySheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	CRect rectWindow;
	GetWindowRect(rectWindow);

	if(!m_rectAnimateFrom.IsRectEmpty())
		::CM_DrawWireRects(&m_rectAnimateFrom, &rectWindow, 16, 15);
	
	
	return bResult;
}
