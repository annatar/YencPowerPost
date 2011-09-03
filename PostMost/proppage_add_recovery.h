#if !defined(AFX_PROPPAGE_ADD_RECOVERY_H__511DD361_CCF2_403D_A09A_3B2E6953F912__INCLUDED_)
#define AFX_PROPPAGE_ADD_RECOVERY_H__511DD361_CCF2_403D_A09A_3B2E6953F912__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CPropPage_Add_Recovery.h : header file
//

#include "par2format.h"

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_Recovery dialog

class CPropPage_Add_Recovery : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropPage_Add_Recovery)

// Construction
public:
	void UpdateStats();
	CPropPage_Add_Recovery();
	~CPropPage_Add_Recovery();

// Dialog Data
	//{{AFX_DATA(CPropPage_Add_Recovery)
	enum { IDD = IDD_PROPPAGE_ADD_RECOVERY };
	//}}AFX_DATA
	DWORD* m_pdwActivation;

	//CStringArray*  m_pDWA_FileNames;

	//CString m_sSFV;
	//CString m_sPAR;
	CString m_sPAR2;

	bool m_bLockUpdates;

	// infos
	int m_iPartSize;
	int m_iSourceChunks;
	int m_iSourceFiles;
	int m_iSourceSize;

	// data
	int m_iFirstChunks;
	int m_iNbChunks;
	int m_iMaxPartsPerFile;
	BOOL m_bSameSize;
	BOOL m_bGroupThread;

	//CProgramSettings* m_pSettings;

	//par2::PostFiles *m_pPostFiles;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPage_Add_Recovery)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL ApplyNewSettings();
	// Generated message map functions
	//{{AFX_MSG(CPropPage_Add_Recovery)
	afx_msg void OnCheckGroupthread();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditPar2();
	afx_msg void OnChangeEditFirst();
	afx_msg void OnChangeEditBlocks();
	afx_msg void OnChangeEditFiles();
	afx_msg void OnCheckSamesize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGE_ADD_RECOVERY_H__511DD361_CCF2_403D_A09A_3B2E6953F912__INCLUDED_)
