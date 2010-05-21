#if !defined(AFX_PROPPAGE_PROGRAMSETTINGS_H__2DD308D3_C539_11D2_88DB_00A0CC24E368__INCLUDED_)
#define AFX_PROPPAGE_PROGRAMSETTINGS_H__2DD308D3_C539_11D2_88DB_00A0CC24E368__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropPage_ProgramSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropPage_ProgramSettings dialog

class CPropPage_ProgramSettings : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropPage_ProgramSettings)

// Construction
public:
	CPropPage_ProgramSettings();
	~CPropPage_ProgramSettings();

// Dialog Data
	//{{AFX_DATA(CPropPage_ProgramSettings)
	enum { IDD = IDD_PROPPAGE_PROGRAM_SETTINGS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA
	CProgramSettings* m_pSettings;
	CProgramSettings* m_pSettings_ApplyTo;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPage_ProgramSettings)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL ApplyNewSettings();

	// Generated message map functions
	//{{AFX_MSG(CPropPage_ProgramSettings)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckHLLast();
	afx_msg void OnCheckShowProgressOnStart();
	afx_msg void OnCheckSound();
	afx_msg void OnChangeEditMaxLines();
	afx_msg void OnCheckSortfiles();
	afx_msg void OnCheckAutoRetry();
	afx_msg void OnCheckSystray();
	afx_msg void OnCheckDetectpar2();
	afx_msg void OnEditchangeComboSkin();
	afx_msg void OnSelchangeComboSkin();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGE_PROGRAMSETTINGS_H__2DD308D3_C539_11D2_88DB_00A0CC24E368__INCLUDED_)
