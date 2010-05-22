#if !defined(AFX_BITMAPLABEL_H__0D2F8184_5BF1_11D3_B7F4_00A0CC215BD8__INCLUDED_)
#define AFX_BITMAPLABEL_H__0D2F8184_5BF1_11D3_B7F4_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BitmapLabel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBitmapLabel window

class CBitmapLabel : public CStatic
{
// Construction
public:
	CBitmapLabel();

// Attributes
public:
	BOOL SetCBitmap(CBitmap* pBitmap);
	CBitmap* GetCBitmap();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapLabel)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBitmapLabel();
protected:
	CBitmap* m_pCBitmap;
	int m_Width;
	int m_Height;

	// Generated message map functions
protected:
	//{{AFX_MSG(CBitmapLabel)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BITMAPLABEL_H__0D2F8184_5BF1_11D3_B7F4_00A0CC215BD8__INCLUDED_)
