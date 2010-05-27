// PostMostDoc.h : interface of the CPostMostDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_POSTMOSTDOC_H__1E48643B_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
#define AFX_POSTMOSTDOC_H__1E48643B_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPostMostDoc : public CDocument
{
protected: // create from serialization only
	CPostMostDoc();
	DECLARE_DYNCREATE(CPostMostDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPostMostDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPostMostDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPostMostDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSTMOSTDOC_H__1E48643B_C1FB_11D2_B7DD_00A0CC215BD8__INCLUDED_)
