#if !defined(AFX_PROPPAGE_HEADERS_H__1E486448_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
#define AFX_PROPPAGE_HEADERS_H__1E486448_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropPage_Headers.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Headers dialog

class CPropPage_Headers : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropPage_Headers)

// Construction
public:
	CPropPage_Headers();
	~CPropPage_Headers();

// Dialog Data
	//{{AFX_DATA(CPropPage_Headers)
	enum { IDD = IDD_PROPPAGE_HEADERS };
	//}}AFX_DATA
	CProgramSettings* m_pSettings;
	CProgramSettings* m_pSettings_ApplyTo;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPage_Headers)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL ApplyNewSettings();
	// Generated message map functions
	//{{AFX_MSG(CPropPage_Headers)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditFrom();
	afx_msg void OnChangeEditOrg();
	afx_msg void OnChangeEditReplyTo();
	afx_msg void OnCheckXNoArchive();
	afx_msg void OnChangeEditFromNick();
	afx_msg void OnChangeEditFollowupTo();
	afx_msg void OnCheckXServerDate();
	afx_msg void OnChangeEditDomainid();
	afx_msg void OnCheckFilethread();
	afx_msg void OnCheckPartialheader();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGE_HEADERS_H__1E486448_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
