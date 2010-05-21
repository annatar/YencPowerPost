#if !defined(AFX_POSTINGPROGRESSDLG_H__CAC77193_C4FF_11D2_B7DD_00A0CC215BD8__INCLUDED_)
#define AFX_POSTINGPROGRESSDLG_H__CAC77193_C4FF_11D2_B7DD_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PostingProgressDlg.h : header file
//

#define IDTIMER_UPDATE_PROGRESS  8192

/////////////////////////////////////////////////////////////////////////////
// CPostingProgressDlg dialog

class CPostingProgressDlg : public CDialog
{
// Construction
public:
	CPostingProgressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPostingProgressDlg)
	enum { IDD = IDD_DIALOG_PROGRESS };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	CPostMostView* m_pView;
	CRect          m_rectAnimateFrom;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPostingProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	UINT            m_uiTimerID;
	CBitmapLabel	m_BitmapLabel;
	CFont			m_StatusFont;
	CBitmap*		CreateBitmapForDlgItem(int nID);

	BOOL            m_bForceUpdateTimeToGo;
	CTime           m_nTimeToGoLastUpdated;

	void UpdateTransmissionGraph();

	void Start_Update_Timer();
	void Stop_Update_Timer();

	// Generated message map functions
	//{{AFX_MSG(CPostingProgressDlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChangeEditBwcap();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSTINGPROGRESSDLG_H__CAC77193_C4FF_11D2_B7DD_00A0CC215BD8__INCLUDED_)
