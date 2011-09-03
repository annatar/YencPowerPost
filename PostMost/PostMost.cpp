// PostMost.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PostMost.h"
#include "MainFrm.h"
#include "PostMostDoc.h"
#include "PostMostView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPostMostApp

BEGIN_MESSAGE_MAP(CPostMostApp, CWinApp)
	//{{AFX_MSG_MAP(CPostMostApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPostMostApp construction

CPostMostApp::CPostMostApp()
{
	m_pSplashWindow = (CSplashWindow*) NULL;
	m_pView = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPostMostApp object

CPostMostApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPostMostApp initialization

BOOL CPostMostApp::InitInstance()
{
	loadLocalizedResources();

	m_pSplashWindow = new CSplashWindow();
	m_pSplashWindow->Create();
	m_pSplashWindow->SetFocus();

	if(AfxOleInit() == 0)
	{
		AfxMessageBox("OLE Initialization Failed.  Possibly the OLE system DLL versions are not correct.", MB_ICONSTOP);
		return FALSE;
	}

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	// Set Registry Key to Current Directory; This allows multiple instances
	// of this program to have their own settings (as long as they are in
	// different directories.)
	free((void*) m_pszProfileName);
	::GetCurrentDirectory(_MAX_PATH, m_szAppDir);
	if((strlen(m_szAppDir) > 0) && (m_szAppDir[strlen(m_szAppDir)-1] != '\\'))
		strcat(m_szAppDir, "\\");
	TCHAR szCurrentDir[_MAX_PATH];
	strcpy(szCurrentDir, m_szAppDir);
	strcat(szCurrentDir, "POWRPOST.INI");
	m_pszProfileName = _tcsdup(szCurrentDir);

	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	m_ImageList.Create(ID_BITMAP_IMAGELIST, 24, 1, RGB(0,0,0));

#ifdef _DEBUG
	TCHAR szTEMP1[256];
	TCHAR szTEMP2[256];
	sprintf(szTEMP1, "X-Newsposter: NNTP YENCPOST Assert (Build 5)");
	::CM_MixString(szTEMP1, szTEMP2, 256);
	TRACE1("AD ORIGINAL: '%s'\n", szTEMP1);
	TRACE1("AD MIXED   : '%s'\n", szTEMP2);

#endif

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CPostMostDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CPostMostView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->SetWindowText(" " TITLE);
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	return TRUE;
}

void CPostMostApp::loadLocalizedResources()
{
	LANGID language = GetUserDefaultUILanguage();
	TCHAR buf[20];
	GetLocaleInfo(GetThreadLocale(), LOCALE_SNAME,  buf, sizeof buf);
	TCHAR szLibraryName[32];
	sprintf(szLibraryName, "ResourceDll_%s.dll", buf);
	#ifdef _DEBUG
		// Load the debug version of the localized resources.
		m_hInstResDLL = LoadLibrary(szLibraryName);
	#else
		// Load the release version of the localized resources.		
		// ex:  ResourceDll_en-UK.dll
		m_hInstResDLL = LoadLibrary(szLibraryName);
	#endif
		
		if (m_hInstResDLL == NULL)
		{
			// loading of country localized DLL failed, try to fallback to language
			GetLocaleInfo(GetThreadLocale(), LOCALE_SABBREVLANGNAME,  buf, sizeof buf);
			// ex:  ResourceDll_ENU.dll
			sprintf(szLibraryName, "ResourceDll_%s.dll", buf);
			m_hInstResDLL = LoadLibrary(szLibraryName);			
			if (m_hInstResDLL == NULL)
			{
				// if no localized version found try to default to US English
				m_hInstResDLL = LoadLibrary("ResourceDll_en-US.dll");
				DWORD lastError = GetLastError();
				// if US English DLL not present fail
				ASSERT( m_hInstResDLL != NULL );
			}
		}

	AfxSetResourceHandle(m_hInstResDLL);
}

int CPostMostApp::ExitInstance()
{
   // In case you load multiple DLL's make sure to free them,
   // and avoid calling FreeLibrary with a NULL pointer.

   FreeLibrary(m_hInstResDLL);
   return CWinApp::ExitInstance();
}


void CPostMostApp::ApplySkin(const char* skin)
{
  COLORREF bkcolor,color;

  bool color_ok, toolbar_ok, imglist_ok;
  color_ok=toolbar_ok=imglist_ok=false;

  char buffer[3*MAX_PATH];
  strcpy(buffer,theApp.m_szAppDir);
  strcat(buffer,"\\");
  strcat(buffer,skin);
  strcat(buffer,"\\skin.txt");
  {
    FILE* fskin = fopen(buffer,"r");
    if (fskin!=NULL)
    {
      int r,g,b;
      r=g=b=0;
      fscanf(fskin,"%d,%d,%d",&r,&g,&b);
      bkcolor = RGB(r,g,b);
      r=0; g=255; b=0;
      fscanf(fskin,"%d,%d,%d",&r,&g,&b);
      color = RGB(r,g,b);
      color_ok=true;
      fclose(fskin);
    }
  }
  if (m_pView!=NULL)
    m_pView->SetListControlColors(color,bkcolor);
  strcpy(buffer,theApp.m_szAppDir);
  strcat(buffer,"\\");
  strcat(buffer,skin);
  strcat(buffer,"\\ListIcons.bmp");

  HBITMAP hbmp;
  hbmp = (HBITMAP)::LoadImage(
                NULL,
                buffer,
                IMAGE_BITMAP,
                0,
                0,
                LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_SHARED
                );
  if (hbmp!=NULL)
  {

    CSize dim; // = bmp->GetBitmapDimension();
    dim.cx=dim.cy=0;

    BITMAP bm;
    ::GetObject( hbmp, sizeof( bm ), &bm );
    dim.cx=bm.bmWidth; dim.cy=bm.bmHeight;
    //GetBitmapDimensionEx(hbmp,&dim);
    dim.cx/=7;
    if (dim.cx>0 && dim.cy>0)
    {
      CBitmap *bmp = CBitmap::FromHandle(hbmp);
/*
	HDC hdc = GetDC(NULL);
	HDC memdc = CreateCompatibleDC(hdc);
	ReleaseDC(NULL,hdc);
	SelectObject(memdc, hbmp);
	COLORREF bcolor;
	bcolor = GetPixel(memdc,0,0);
	DeleteDC(memdc);
*/
      //while (m_ImageList.GetImageCount()>0)
      //  m_ImageList.Remove(0);
      m_ImageList.Detach();
      m_ImageList.Create(dim.cx,dim.cy,ILC_COLOR32,0,20);
      //CImageList newImgList;
      //newImgList.Create(dim.cx,dim.cy,ILC_COLOR,0);
      m_ImageList.Add(bmp,RGB(0,0,0));
      // Set image lists for list control
      if (m_pView!=NULL)
      {
	CListCtrl& listCtrl = m_pView->GetListCtrl();
	listCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);
      }
      imglist_ok=true;
    }
  }

  strcpy(buffer,theApp.m_szAppDir);
  strcat(buffer,"\\");
  strcat(buffer,skin);
  strcat(buffer,"\\Toolbar.bmp");

  hbmp = (HBITMAP)::LoadImage(
                NULL,
                buffer,
                IMAGE_BITMAP,
                0,
                0,
                LR_LOADFROMFILE|LR_CREATEDIBSECTION|LR_SHARED
                );
  if (hbmp==NULL)
  {
    hbmp = LoadBitmap(this->m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
  }

  {
    //CBitmap bmp;
    //bmp.Attach(hbmp);

    CSize dim; // = bmp.GetBitmapDimension();
    BITMAP bm;
    ::GetObject( hbmp, sizeof( bm ), &bm );
    dim.cx=bm.bmWidth; dim.cy=bm.bmHeight;
    dim.cx/=20;
    if (dim.cx>0 && dim.cy>0)
    {

      SIZE bsize;
      bsize.cx=dim.cx+7;
      bsize.cy=dim.cy+6;
      if (m_pMainWnd!=NULL)
      {
	((CMainFrame*)m_pMainWnd)->m_wndToolBar.SetSizes(bsize,dim);
	((CMainFrame*)m_pMainWnd)->m_wndToolBar.SetBitmap(hbmp);
        ((CMainFrame*)m_pMainWnd)->m_wndToolBar.GetToolBarCtrl().AutoSize();
	((CMainFrame*)m_pMainWnd)->RecalcLayout();

	HDC hdc = GetDC(NULL);
	HDC memdc = CreateCompatibleDC(hdc);
	ReleaseDC(NULL,hdc);
	SelectObject(memdc, hbmp);
	COLORREF bcolor;
	bcolor = GetPixel(memdc,0,0);
	DeleteDC(memdc);
	CImageList * ilist;
	ilist = new CImageList();
	ilist->Create(dim.cx,dim.cy,ILC_COLOR32|ILC_MASK,7,14);
	//ilist = ((CMainFrame*)m_pMainWnd)->m_wndToolBar.GetToolBarCtrl().GetImageList();
	if (ilist!=NULL) 
	{
	  ilist->SetBkColor(bcolor);
	  CBitmap *bmp = CBitmap::FromHandle(hbmp);
	  IMAGEINFO info;
	  /*
	  ilist->GetImageInfo(0,&info);
	  while (ilist->GetImageCount()>0)
	    ilist->Remove(0);
	  */
	  //CImageList newImgList;
	  //newImgList.Create(dim.cx,dim.cy,ILC_COLOR,0);
	  ilist->Add(bmp,bcolor);
	  ilist->GetImageInfo(0,&info);
	  ((CMainFrame*)m_pMainWnd)->m_wndToolBar.GetToolBarCtrl().SetImageList(ilist);
	  ((CMainFrame*)m_pMainWnd)->m_wndToolBar.SetSizes(bsize,dim);
	  ((CMainFrame*)m_pMainWnd)->m_wndToolBar.GetToolBarCtrl().AutoSize();
	  ((CMainFrame*)m_pMainWnd)->RecalcLayout();
	}
	//ilist = ((CMainFrame*)m_pMainWnd)->m_wndToolBar.GetToolBarCtrl().GetDisabledImageList();
	//if (ilist!=NULL) ilist->SetBkColor(RGB(192,192,192));
	//ilist = ((CMainFrame*)m_pMainWnd)->m_wndToolBar.GetToolBarCtrl().GetHotImageList();
	//if (ilist!=NULL) ilist->SetBkColor(RGB(192,192,192));

      }
      toolbar_ok=true;
    }
  }

  if (!color_ok && m_pView!=NULL)
    m_pView->SetListControlColors();

  if (!imglist_ok)
  {
    m_ImageList.Detach();
    m_ImageList.Create(ID_BITMAP_IMAGELIST, 24, 1, RGB(0,0,0));    
    if (m_pView!=NULL)
    {
      CListCtrl& listCtrl = m_pView->GetListCtrl();
      listCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);
    }
  }
/*
  if (!toolbar_ok && m_pMainWnd!=NULL)
  {
    SIZE bsize, isize;
    isize.cx=16;
    isize.cy=15;
    bsize.cx=isize.cx+7;
    bsize.cy=isize.cy+6;
    ((CMainFrame*)m_pMainWnd)->m_wndToolBar.LoadToolBar(IDR_MAINFRAME);
    ((CMainFrame*)m_pMainWnd)->m_wndToolBar.SetSizes(bsize,isize);
    ((CMainFrame*)m_pMainWnd)->m_wndToolBar.LoadBitmap(IDR_MAINFRAME);
    ((CMainFrame*)m_pMainWnd)->m_wndToolBar.GetToolBarCtrl().AutoSize();
    ((CMainFrame*)m_pMainWnd)->RecalcLayout();
  }
*/
  if (m_pView!=NULL)
    UpdateWindow(m_pView->GetListCtrl());
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	CRect			m_rectAnimateFrom;

	CBitmap			m_Bitmap;
	int             m_Bitmap_Width;
	int             m_Bitmap_Height;

	CBitmapLabel	m_BitmapLabel;

	CBitmap*		CreateBitmapForDlgItem(int nID);

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonGo();
	afx_msg void OnButtonGoMail();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
	m_rectAnimateFrom.SetRectEmpty();
	m_Bitmap_Width = 0;
	m_Bitmap_Width = 0;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_BUTTON_GO, OnButtonGo)
	ON_BN_CLICKED(IDC_BUTTON_GO_MAIL, OnButtonGoMail)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

// App command to run the dialog
void CPostMostApp::OnAppAbout()
{
	CMainFrame* pMainFrame = (CMainFrame*) m_pMainWnd;

	CAboutDlg aboutDlg;

	pMainFrame->GetButtonRect(ID_APP_ABOUT, &aboutDlg.m_rectAnimateFrom);

	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CPostMostApp message handlers

CBitmap* CAboutDlg::CreateBitmapForDlgItem(int nID)
{
	CWnd* pTargetWnd = GetDlgItem(nID);
	CDC* pWndCDC = pTargetWnd->GetDC();
	CBitmap* pNewCBitmap = new CBitmap();
	RECT TargetRect;
	pTargetWnd->GetClientRect(&TargetRect);

	if(pNewCBitmap->CreateCompatibleBitmap(pWndCDC, TargetRect.right, TargetRect.bottom) == TRUE)
	{
		// Create Memory DC to select bitmap into
		CDC MemDC;
		CDC MemDC_AboutBitmap;

		MemDC.CreateCompatibleDC(pWndCDC);
		MemDC_AboutBitmap.CreateCompatibleDC(pWndCDC);
		CBitmap* pOldBMP_About = MemDC_AboutBitmap.SelectObject(&m_Bitmap);

		// Get Bitmap Info (width, height)
		BITMAP bmpInfo;
		pNewCBitmap->GetBitmap(&bmpInfo);
		int w = bmpInfo.bmWidth;
		int h = bmpInfo.bmHeight;

		MemDC.FillSolidRect(&TargetRect, RGB(0,0,0));
		CBitmap* pOldBMP = MemDC.SelectObject(pNewCBitmap);

		MemDC.BitBlt((TargetRect.right - m_Bitmap_Width) / 2,
					 (TargetRect.bottom - m_Bitmap_Height) / 2,
					 m_Bitmap_Width, m_Bitmap_Height, &MemDC_AboutBitmap, 0, 0, SRCCOPY);

		MemDC.SelectObject(pOldBMP);
		MemDC_AboutBitmap.SelectObject(pOldBMP_About);
	} 
	else 
	{ 
		delete pNewCBitmap; 
		pNewCBitmap = (CBitmap*) NULL; 
	}
	pTargetWnd->ReleaseDC(pWndCDC);

	return(pNewCBitmap);
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Display Version Number
	CString szVersion;
	szVersion.LoadString(IDS_POSTMOST_VERSION);
	GetDlgItem(IDC_STATIC_VERSION)->SetWindowText(szVersion);

	BITMAP bm;
	m_Bitmap.LoadBitmap(IDB_BITMAP_ABOUT);
	m_Bitmap.GetObject(sizeof(BITMAP), &bm);
	m_Bitmap_Width = bm.bmWidth;
	m_Bitmap_Height = bm.bmHeight;

	CenterWindow();

	if(!m_rectAnimateFrom.IsRectEmpty())
	{
		CRect rectWindow;
		GetWindowRect(rectWindow);
		::CM_DrawWireRects(&m_rectAnimateFrom, &rectWindow, 16, 15);
	}

	// Create CBitmap Object for usage with IDC_STATIC_LABEL
	CBitmap* pNewCBitmap = CreateBitmapForDlgItem(IDC_STATIC_IMAGE);
	m_BitmapLabel.SubclassDlgItem(IDC_STATIC_IMAGE, this);
	m_BitmapLabel.SetCBitmap(pNewCBitmap);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnButtonGo()
{
	GetDlgItem(IDC_BUTTON_GO)->EnableWindow(FALSE);
	ShellExecute(NULL, "open", "http://powerpost.cjb.net/", NULL, NULL, SW_MAXIMIZE );
	CWaitCursor wc;
	::Sleep(3000);
	GetDlgItem(IDC_BUTTON_GO)->EnableWindow(TRUE);	
}

void CAboutDlg::OnButtonGoMail() 
{
	GetDlgItem(IDC_BUTTON_GO_MAIL)->EnableWindow(FALSE);
	ShellExecute(NULL, "open", "mailto:assert@powerpost.cjb.net", NULL, NULL, SW_NORMAL);
	CWaitCursor wc;
	::Sleep(3000);
	GetDlgItem(IDC_BUTTON_GO_MAIL)->EnableWindow(TRUE);	
}

void CAboutDlg::OnDestroy() 
{
	CRect rectWindow;
	GetWindowRect(rectWindow);

	CDialog::OnDestroy();

	if(!m_rectAnimateFrom.IsRectEmpty())
		::CM_DrawWireRects(&rectWindow, &m_rectAnimateFrom, 16, 15);
	
}

void CAboutDlg::OnBnClickedOk()
{
	// TODO : ajoutez ici le code de votre gestionnaire de notification de contrôle
	OnOK();
}
