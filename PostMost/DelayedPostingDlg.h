#if !defined(AFX_DELAYEDPOSTINGDLG_H__5EE17023_E735_11D3_9CED_00A0CC22BF9C__INCLUDED_)
#define AFX_DELAYEDPOSTINGDLG_H__5EE17023_E735_11D3_9CED_00A0CC22BF9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DelayedPostingDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDelayedPostingDlg dialog

class CDelayedPostingDlg : public CDialog
{
// Construction
public:
	CDelayedPostingDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDelayedPostingDlg)
	enum { IDD = IDD_DIALOG_POST_AT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	CTime m_StartTime;
	CTime m_StartTimeEnd;
	BOOL  m_StopValue;
	BOOL  CheckStop;
	BOOL  CheckRepeat;

	int   m_nInit_Hour;
	int   m_nInit_Min;
	int   m_nInit_AMPM;
	int   m_nInit_Hour_End;
	int   m_nInit_Min_End;
	int   m_nInit_AMPM_End;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDelayedPostingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateTimeToGo();

	// Generated message map functions
	//{{AFX_MSG(CDelayedPostingDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeComboHour();
	afx_msg void OnSelchangeComboMin();
	afx_msg void OnSelchangeComboAmPm();
	afx_msg void OnSelchangeComboHourEnd();
	afx_msg void OnSelchangeComboMinEnd();
	afx_msg void OnSelchangeComboAmPmEnd();
	afx_msg void OnCheckChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELAYEDPOSTINGDLG_H__5EE17023_E735_11D3_9CED_00A0CC22BF9C__INCLUDED_)
