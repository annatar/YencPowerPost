#if !defined(AFX_ENCODINGPROGRESSDLG_H__CAC77293_C4FF_11D2_B7DD_00A0CC215BD8__INCLUDED_)
#define AFX_ENCODINGPROGRESSDLG_H__CAC77293_C4FF_11D2_B7DD_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EncodingProgressDlg.h : header file
//

#include "par2format.h"

#define IDTIMER_UPDATE_PROGRESS  8192

/////////////////////////////////////////////////////////////////////////////
// CEncodingProgressDlg dialog

class CEncodingProgressDlg : public CDialog
{
// Construction
public:
	CEncodingProgressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEncodingProgressDlg)
	enum { IDD = IDD_DIALOG_ENCODING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	CPostMostView* m_pView;
	CRect          m_rectAnimateFrom;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEncodingProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	UINT            m_uiTimerID;
	//CBitmapLabel	m_BitmapLabel;
	//CFont			m_StatusFont;
	//CBitmap*		CreateBitmapForDlgItem(int nID);
	enum { NSAMPLE=10 };
	par2::u64 last_bytes[NSAMPLE];
	par2::u64 last_time[NSAMPLE];
	CEncoderThread* last_worker;
	int last_batch;


	BOOL            m_bForceUpdateTimeToGo;
	CTime           m_nTimeToGoLastUpdated;

	void UpdateTransmissionGraph();

	void Start_Update_Timer();
	void Stop_Update_Timer();

	// Generated message map functions
	//{{AFX_MSG(CEncodingProgressDlg)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENCODINGPROGRESSDLG_H__CAC77293_C4FF_11D2_B7DD_00A0CC215BD8__INCLUDED_)
