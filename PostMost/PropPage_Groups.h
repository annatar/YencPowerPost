#if !defined(AFX_PROPPAGE_GROUPS_H__1E486449_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
#define AFX_PROPPAGE_GROUPS_H__1E486449_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropPage_Groups.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Groups dialog

class CPropPage_Groups : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropPage_Groups)

// Construction
public:
	CPropPage_Groups();
	~CPropPage_Groups();

// Dialog Data
	//{{AFX_DATA(CPropPage_Groups)
	enum { IDD = IDD_PROPPAGE_GROUPS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	CProgramSettings* m_pSettings;
	CProgramSettings* m_pSettings_ApplyTo;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPage_Groups)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	BOOL ApplyNewSettings();
	void AddGroups(CListCtrl* pListCtrl);
	void UpdateButtons(CListCtrl* pListCtrl);

	// Generated message map functions
	//{{AFX_MSG(CPropPage_Groups)
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonRemove();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListGroups(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListGroups(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListGroups(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGE_GROUPS_H__1E486449_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
