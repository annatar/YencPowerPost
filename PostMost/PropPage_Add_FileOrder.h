#if !defined(AFX_PROPPAGE_ADD_FILEORDER_H__2AD2DD13_FB61_11D3_B80E_00A0CC215BD8__INCLUDED_)
#define AFX_PROPPAGE_ADD_FILEORDER_H__2AD2DD13_FB61_11D3_B80E_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropPage_Add_FileOrder.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_FileOrder dialog

class CPropPage_Add_FileOrder : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropPage_Add_FileOrder)

// Construction
public:
	CPropPage_Add_FileOrder();
	~CPropPage_Add_FileOrder();

// Dialog Data
	//{{AFX_DATA(CPropPage_Add_FileOrder)
	enum { IDD = IDD_PROPPAGE_ADD_ORDER };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	DWORD* m_pdwActivation;

	CStringArray*  m_pDWA_FileNames;

	void _BreakFilename(LPCTSTR FullFilename, char* szDir, char* szFile);

	TCHAR m_szDirectory[_MAX_PATH];
	TCHAR m_szLastDirectory[_MAX_PATH];

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPage_Add_FileOrder)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateButtons();

	// Generated message map functions
	//{{AFX_MSG(CPropPage_Add_FileOrder)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAfoUp();
	afx_msg void OnButtonAfoDown();
	afx_msg void OnClickListFileOrder(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListFileOrder(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownListFileOrder(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonAfoSortpar();
	afx_msg void OnButtonAfoDelfile();
	afx_msg void OnButtonAdd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGE_ADD_FILEORDER_H__2AD2DD13_FB61_11D3_B80E_00A0CC215BD8__INCLUDED_)
