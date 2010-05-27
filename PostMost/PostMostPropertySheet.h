#if !defined(AFX_POSTMOSTPROPERTYSHEET_H__1EAE31E1_D99C_11D3_B80B_00A0CC215BD8__INCLUDED_)
#define AFX_POSTMOSTPROPERTYSHEET_H__1EAE31E1_D99C_11D3_B80B_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PostMostPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPostMostPropertySheet

class CPostMostPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CPostMostPropertySheet)

// Construction
public:
	CPostMostPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPostMostPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

	CRect m_rectAnimateFrom;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPostMostPropertySheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPostMostPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPostMostPropertySheet)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSTMOSTPROPERTYSHEET_H__1EAE31E1_D99C_11D3_B80B_00A0CC215BD8__INCLUDED_)
