// BitmapLabel.cpp : implementation file
//

#include "stdafx.h"
#include "BitmapLabel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBitmapLabel

CBitmapLabel::CBitmapLabel()
{
	m_pCBitmap = (CBitmap*) NULL;
	m_Width = 0; 
	m_Height = 0;
}

CBitmapLabel::~CBitmapLabel()
{
	if(m_pCBitmap != NULL)
	{
		delete m_pCBitmap;
		m_pCBitmap = NULL;
	}
}


BEGIN_MESSAGE_MAP(CBitmapLabel, CStatic)
	//{{AFX_MSG_MAP(CBitmapLabel)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBitmapLabel message handlers

void CBitmapLabel::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// Render any bitmap we have
	if(m_pCBitmap != (CBitmap*) NULL)
	{
		CDC MemDC;
		MemDC.CreateCompatibleDC(&dc);
		CBitmap* pOldBMP = MemDC.SelectObject(m_pCBitmap);
		dc.BitBlt(0, 0, m_Width, m_Height, &MemDC, 0, 0, SRCCOPY);
		MemDC.SelectObject(pOldBMP);
	}
}

BOOL CBitmapLabel::SetCBitmap(CBitmap* pBitmap)
{
	if(pBitmap == (CBitmap*) NULL)
	{
		m_pCBitmap = (CBitmap*) NULL;
		return(TRUE);
	}

	BITMAP bmpinfo;
	pBitmap->GetBitmap(&bmpinfo);

	m_Width = bmpinfo.bmWidth;
	m_Height = bmpinfo.bmHeight;

	m_pCBitmap = pBitmap;

	return TRUE;
}

CBitmap* CBitmapLabel::GetCBitmap()
{
	return(m_pCBitmap);
}


void CBitmapLabel::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CStatic::OnLButtonDown(nFlags, point);
}
