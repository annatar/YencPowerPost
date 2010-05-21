#if !defined(AFX_DELETEHISTORYCONFIRMDLG_H__7F1265A3_DB18_11D3_B80B_00A0CC215BD8__INCLUDED_)
#define AFX_DELETEHISTORYCONFIRMDLG_H__7F1265A3_DB18_11D3_B80B_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeleteHistoryConfirmDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeleteHistoryConfirmDlg dialog

class CDeleteHistoryConfirmDlg : public CDialog
{
// Construction
public:
	CDeleteHistoryConfirmDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDeleteHistoryConfirmDlg)
	enum { IDD = IDD_DIALOG_DEL_HIST };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	TCHAR m_szText[1024];		// Message Text

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeleteHistoryConfirmDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeleteHistoryConfirmDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELETEHISTORYCONFIRMDLG_H__7F1265A3_DB18_11D3_B80B_00A0CC215BD8__INCLUDED_)
