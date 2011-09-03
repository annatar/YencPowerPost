#if !defined(AFX_PROPPAGE_ADD_GROUPS_H__2BE134A6_DFDB_11D3_B80B_00A0CC215BD8__INCLUDED_)
#define AFX_PROPPAGE_ADD_GROUPS_H__2BE134A6_DFDB_11D3_B80B_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropPage_Add_Groups.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_Groups dialog

class CPropPage_Add_Groups : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropPage_Add_Groups)

// Construction
public:
	CPropPage_Add_Groups();
	~CPropPage_Add_Groups();

// Dialog Data
	//{{AFX_DATA(CPropPage_Add_Groups)
	enum { IDD = IDD_PROPPAGE_ADD_GROUPS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	DWORD* m_pdwActivation;

	CString m_strGroupList;

	CProgramSettings* m_pSettings;

	CImageList m_StateImageList;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPage_Add_Groups)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void AddGroups(CListCtrl* pListCtrl,CStringArray& selgroups);

	// Generated message map functions
	//{{AFX_MSG(CPropPage_Add_Groups)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAddGroup();
	afx_msg void OnClickListGroups(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListGroups(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListGroups(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGE_ADD_GROUPS_H__2BE134A6_DFDB_11D3_B80B_00A0CC215BD8__INCLUDED_)
