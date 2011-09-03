#if !defined(AFX_PROPPAGE_ADD_TEXTPREFIX_H__2BE134A5_DFDB_11D3_B80B_00A0CC215BD8__INCLUDED_)
#define AFX_PROPPAGE_ADD_TEXTPREFIX_H__2BE134A5_DFDB_11D3_B80B_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropPage_Add_TextPrefix.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_TextPrefix dialog

class CPropPage_Add_TextPrefix : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropPage_Add_TextPrefix)

// Construction
public:
	CPropPage_Add_TextPrefix();
	~CPropPage_Add_TextPrefix();

// Dialog Data
	//{{AFX_DATA(CPropPage_Add_TextPrefix)
	enum { IDD = IDD_PROPPAGE_ADD_TEXT_PREFIX };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	DWORD* m_pdwActivation;

	BOOL    m_bSendTextPrefix;
	BOOL    m_bSendTextFirstFileOnly;

	CString m_strTextPrefix;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPage_Add_TextPrefix)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bFirstChange;
	// Generated message map functions
	//{{AFX_MSG(CPropPage_Add_TextPrefix)
	afx_msg void OnChangeEditPrefixText();
	afx_msg void OnCheckSendTextPrefix();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGE_ADD_TEXTPREFIX_H__2BE134A5_DFDB_11D3_B80B_00A0CC215BD8__INCLUDED_)
