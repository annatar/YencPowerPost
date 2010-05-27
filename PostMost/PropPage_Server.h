#if !defined(AFX_PROPPAGE_SERVER_H__1E486447_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
#define AFX_PROPPAGE_SERVER_H__1E486447_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropPage_Server.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Server dialog

class CPropPage_Server : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropPage_Server)

// Construction
public:
	CPropPage_Server();
	~CPropPage_Server();

// Dialog Data
	//{{AFX_DATA(CPropPage_Server)
	enum { IDD = IDD_PROPPAGE_SERVER };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA
	CProgramSettings* m_pSettings;
	CProgramSettings* m_pSettings_ApplyTo;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPage_Server)
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
	//{{AFX_MSG(CPropPage_Server)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckLogin();
	afx_msg void OnChangeEditPassword();
	afx_msg void OnChangeEditConfirm();
	afx_msg void OnChangeEditMaxLines();
	afx_msg void OnChangeEditPort();
	afx_msg void OnChangeEditServer();
	afx_msg void OnChangeEditUsername();
	afx_msg void OnChangeEditBwcap();
	afx_msg void OnEditchangeComboServer();
	afx_msg void OnSelchangeComboServer();
	afx_msg void OnSelendokComboServer();
	afx_msg void OnChangeEditConnections();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGE_SERVER_H__1E486447_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
