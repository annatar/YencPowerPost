#if !defined(AFX_DELETETASKDLG_H__A793AF13_DF0A_11D3_B80B_00A0CC215BD8__INCLUDED_)
#define AFX_DELETETASKDLG_H__A793AF13_DF0A_11D3_B80B_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeleteTaskDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeleteTaskDlg dialog

class CDeleteTaskDlg : public CDialog
{
// Construction
public:
	CDeleteTaskDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDeleteTaskDlg)
	enum { IDD = IDD_DIALOG_DEL_TASK };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	BOOL  m_bCheckValue;		// Initial and Final Check Value State
	TCHAR m_szMessage[512];		// Dialog Message Text

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteTaskDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeleteTaskDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELETETASKDLG_H__A793AF13_DF0A_11D3_B80B_00A0CC215BD8__INCLUDED_)
