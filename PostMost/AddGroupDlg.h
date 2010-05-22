#if !defined(AFX_ADDGROUPDLG_H__A8B29538_C290_11D2_B7DD_00A0CC215BD8__INCLUDED_)
#define AFX_ADDGROUPDLG_H__A8B29538_C290_11D2_B7DD_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddGroupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddGroupDlg dialog

class CAddGroupDlg : public CDialog
{
// Construction
public:
	CAddGroupDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddGroupDlg)
	enum { IDD = IDD_DIALOG_ADDGROUP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CString m_GroupName;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddGroupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddGroupDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDGROUPDLG_H__A8B29538_C290_11D2_B7DD_00A0CC215BD8__INCLUDED_)
