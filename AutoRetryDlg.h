#if !defined(AFX_AUTORETRYDLG_H__01D9FA93_DB69_11D3_9CEC_00A0CC22BF9C__INCLUDED_)
#define AFX_AUTORETRYDLG_H__01D9FA93_DB69_11D3_9CEC_00A0CC22BF9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoRetryDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAutoRetryDlg dialog

class CAutoRetryDlg : public CDialog
{
// Construction
public:
	CAutoRetryDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAutoRetryDlg)
	enum { IDD = IDD_DIALOG_AUTO_RETRY };
	//}}AFX_DATA

	UINT m_nTimerID;
	int  m_nSeconds;

	TCHAR m_szMessage[1024];

	void SetTimerText();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoRetryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAutoRetryDlg)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSkip();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTORETRYDLG_H__01D9FA93_DB69_11D3_9CEC_00A0CC22BF9C__INCLUDED_)
