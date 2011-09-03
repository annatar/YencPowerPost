#if !defined(AFX_PROPPAGE_ADD_SUBJECT_H__2BE134A3_DFDB_11D3_B80B_00A0CC215BD8__INCLUDED_)
#define AFX_PROPPAGE_ADD_SUBJECT_H__2BE134A3_DFDB_11D3_B80B_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropPage_Add_Subject.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropPage_Add_Subject dialog

class CPropPage_Add_Subject : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropPage_Add_Subject)

// Construction
public:
	CPropPage_Add_Subject();
	~CPropPage_Add_Subject();

// Dialog Data
	//{{AFX_DATA(CPropPage_Add_Subject)
	enum { IDD = IDD_PROPPAGE_ADD_SUBJECT };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	DWORD* m_pdwActivation;

	CString m_strSubjectPrefixTemplate;
	CString m_last_strSubjectPrefixTemplate;
	BOOL m_bGroupsOK;

	int  m_nDir;

	BOOL m_bInsertFilesAtBeginningOfQueue;
	BOOL m_bProcessSubdirectories;
	BOOL m_bRecursive;
	BOOL m_bCountDirSeperate;
	
	CStringArray* m_pDroppedFiles;

	TCHAR m_Ex_szFilename[_MAX_PATH];
	TCHAR m_Ex_szDirectory[_MAX_PATH];
	DWORD m_Ex_Filesize;
	int   m_Ex_nFileCount;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropPage_Add_Subject)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPropPage_Add_Subject)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditSubjectTemplate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPPAGE_ADD_SUBJECT_H__2BE134A3_DFDB_11D3_B80B_00A0CC215BD8__INCLUDED_)
