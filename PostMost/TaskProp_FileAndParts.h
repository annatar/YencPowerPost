#if !defined(AFX_TASKPROP_FILEANDPARTS_H__CCCD4B87_D8BB_11D3_B80B_00A0CC215BD8__INCLUDED_)
#define AFX_TASKPROP_FILEANDPARTS_H__CCCD4B87_D8BB_11D3_B80B_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskProp_FileAndParts.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTaskProp_FileAndParts dialog

class CTaskProp_FileAndParts : public CPropertyPage
{
	DECLARE_DYNCREATE(CTaskProp_FileAndParts)

// Construction
public:
	CTaskProp_FileAndParts();
	~CTaskProp_FileAndParts();

// Dialog Data
	//{{AFX_DATA(CTaskProp_FileAndParts)
	enum { IDD = IDD_PROPPAGE_TASK_ITEM_MAIN };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	CTaskObject* m_pTaskObject;
	
	CImageList m_StateImageList;

	int m_nPartsChecked;	// IDC_STATIC_N_CHECKED
	

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTaskProp_FileAndParts)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	
	void Refresh_Num_Checked_Label();
	void Refresh_Parts_List(CListCtrl* pListCtrl);

	// Generated message map functions
	//{{AFX_MSG(CTaskProp_FileAndParts)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonTaskPartsCheckAll();
	afx_msg void OnButtonTaskPartsUncheckAll();
	afx_msg void OnButtonPartsModify();
	afx_msg void OnClickListTaskParts(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKPROP_FILEANDPARTS_H__CCCD4B87_D8BB_11D3_B80B_00A0CC215BD8__INCLUDED_)
