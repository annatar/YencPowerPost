#if !defined(AFX_HISTORYDLG_H__1EAE31E4_D99C_11D3_B80B_00A0CC215BD8__INCLUDED_)
#define AFX_HISTORYDLG_H__1EAE31E4_D99C_11D3_B80B_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HistoryDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHistoryDlg dialog

class CPostMostView;

class CHistoryDlg : public CDialog
{
// Construction
public:
	CHistoryDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHistoryDlg)
	enum { IDD = IDD_DIALOG_POSTMOST_HISTORY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	BOOL		m_bSound;
	CRect		m_rectAnimateFrom;

	CPostMostView* m_pView;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistoryDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void BuildHistoryFileStats(CListCtrl* pListCtrl);

	void UpdateHistoryInfoLabel();

	int  m_nEntries;
	int  m_nDeleted;

	void UpdateButtons();

	// Generated message map functions
	//{{AFX_MSG(CHistoryDlg)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButtonHistCompact();
	afx_msg void OnButtonHistDelSel();
	afx_msg void OnButtonHistQueueSel();
	afx_msg void OnClickListHistory(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListHistory(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListHistory(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HISTORYDLG_H__1EAE31E4_D99C_11D3_B80B_00A0CC215BD8__INCLUDED_)
