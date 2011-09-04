#if !defined(AFX_PROPPAGE_ADD_CHECKSUMS_H__521DD361_CCF2_403D_A09A_3B2E6953F912__INCLUDED_)
#define AFX_PROPPAGE_ADD_CHECKSUMS_H__521DD361_CCF2_403D_A09A_3B2E6953F912__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CPropPage_Add_Checksums.h : header file
//

#include "par2format.h"

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_Checksums dialog

class CPropPage_Add_Checksums : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropPage_Add_Checksums)

// Construction
public:
	CPropPage_Add_Checksums();
	~CPropPage_Add_Checksums();

// Dialog Data
	//{{AFX_DATA(CPropPage_Add_Checksums)
	enum { IDD = IDD_PROPPAGE_ADD_CHECKSUMS };
	//}}AFX_DATA
	DWORD* m_pdwActivation;

	CStringArray*  m_pDWA_FileNames;

	CString m_sSFV;
	CString m_sPAR2;
	CString m_sNZB;

	int m_iPartSize;

	BOOL m_bGroupThread;

	//CProgramSettings* m_pSettings;

	//par2::PostFiles *m_pPostFiles;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPage_Add_Checksums)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL ApplyNewSettings();
	// Generated message map functions
	//{{AFX_MSG(CPropPage_Add_Checksums)
	afx_msg void OnCheckSfv();
	afx_msg void OnChangeEditSfv();
	afx_msg void OnCheckGroupthread();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSfv();
	afx_msg void OnCheckPar2();
	afx_msg void OnChangeEditPar2();
	afx_msg void OnButtonPar2();
	afx_msg void OnChangeEditPartsize();
	afx_msg void OnChangeEditNzb();
	afx_msg void OnButtonNzb();
	afx_msg void OnCheckNzb();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGE_ADD_CHECKSUMS_H__511DD361_CCF2_403D_A09A_3B2E6953F912__INCLUDED_)
