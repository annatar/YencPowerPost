#if !defined(AFX_TASKPROP_GROUPS_H__CCCD4B88_D8BB_11D3_B80B_00A0CC215BD8__INCLUDED_)
#define AFX_TASKPROP_GROUPS_H__CCCD4B88_D8BB_11D3_B80B_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskProp_Groups.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTaskProp_Groups dialog

class CTaskProp_Groups : public CPropertyPage
{
	DECLARE_DYNCREATE(CTaskProp_Groups)

// Construction
public:
	CTaskProp_Groups();
	~CTaskProp_Groups();

// Dialog Data
	//{{AFX_DATA(CTaskProp_Groups)
	enum { IDD = IDD_PROPPAGE_TASK_ITEM_GROUPS };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	CTaskObject*      m_pTaskObject;
	CProgramSettings* m_pSettings;
	CImageList        m_StateImageList;
	CStringArray      m_CSA_TaskGroupList;
	
	BOOL m_bSound;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTaskProp_Groups)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void AddGroups(CListCtrl* pListCtrl);
	int  GetSelectedGroupCount();
	void GetSelectedGroupString(CString& rString);
	// Generated message map functions
	//{{AFX_MSG(CTaskProp_Groups)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickListTargetGroups(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonTaskAddGroup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKPROP_GROUPS_H__CCCD4B88_D8BB_11D3_B80B_00A0CC215BD8__INCLUDED_)
