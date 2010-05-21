#if !defined(AFX_TASKPROP_PREFIXTEXT_H__CCCD4B89_D8BB_11D3_B80B_00A0CC215BD8__INCLUDED_)
#define AFX_TASKPROP_PREFIXTEXT_H__CCCD4B89_D8BB_11D3_B80B_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskProp_PrefixText.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTaskProp_PrefixText dialog

class CTaskProp_PrefixText : public CPropertyPage
{
	DECLARE_DYNCREATE(CTaskProp_PrefixText)

// Construction
public:
	CTaskProp_PrefixText();
	~CTaskProp_PrefixText();

// Dialog Data
	//{{AFX_DATA(CTaskProp_PrefixText)
	enum { IDD = IDD_PROPPAGE_TASK_ITEM_PREFIX_TEXT };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	CTaskObject* m_pTaskObject;
	BOOL m_bSound;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTaskProp_PrefixText)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTaskProp_PrefixText)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditTaskItemPrefixText();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKPROP_PREFIXTEXT_H__CCCD4B89_D8BB_11D3_B80B_00A0CC215BD8__INCLUDED_)
