#if !defined(AFX_TASKITEMPROPDLG_H__CCCD4B86_D8BB_11D3_B80B_00A0CC215BD8__INCLUDED_)
#define AFX_TASKITEMPROPDLG_H__CCCD4B86_D8BB_11D3_B80B_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskItemPropDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTaskItemPropDlg dialog

class CTaskItemPropDlg : public CDialog
{
// Construction
public:
	CTaskItemPropDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTaskItemPropDlg)
	enum { IDD = IDD_TASK_ITEM_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskItemPropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTaskItemPropDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKITEMPROPDLG_H__CCCD4B86_D8BB_11D3_B80B_00A0CC215BD8__INCLUDED_)
