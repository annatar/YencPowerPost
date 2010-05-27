// DelayedPostingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PostMost.h"
#include "DelayedPostingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDelayedPostingDlg dialog


CDelayedPostingDlg::CDelayedPostingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDelayedPostingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDelayedPostingDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nInit_Hour = 0;
	m_nInit_Min = 0;
	m_nInit_AMPM = 0;
	m_nInit_Hour_End = 0;
	m_nInit_Min_End = 0;
	m_nInit_AMPM_End = 0;

}


void CDelayedPostingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDelayedPostingDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDelayedPostingDlg, CDialog)
	//{{AFX_MSG_MAP(CDelayedPostingDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_HOUR, OnSelchangeComboHour)
	ON_CBN_SELCHANGE(IDC_COMBO_MIN, OnSelchangeComboMin)
	ON_CBN_SELCHANGE(IDC_COMBO_AM_PM, OnSelchangeComboAmPm)
	ON_CBN_SELCHANGE(IDC_COMBO_HOUR_END, OnSelchangeComboHourEnd)
	ON_CBN_SELCHANGE(IDC_COMBO_MIN_END, OnSelchangeComboMinEnd)
	ON_CBN_SELCHANGE(IDC_COMBO_AM_PM_END, OnSelchangeComboAmPmEnd)
	ON_BN_CLICKED(IDC_CHECK_STOP, OnCheckChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelayedPostingDlg message handlers

BOOL CDelayedPostingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CComboBox* pComboHour = (CComboBox*) GetDlgItem(IDC_COMBO_HOUR);
	CComboBox* pComboMin  = (CComboBox*) GetDlgItem(IDC_COMBO_MIN);
	CComboBox* pComboAMPM = (CComboBox*) GetDlgItem(IDC_COMBO_AM_PM);

	CComboBox* pComboHourEnd = (CComboBox*) GetDlgItem(IDC_COMBO_HOUR_END);
	CComboBox* pComboMinEnd  = (CComboBox*) GetDlgItem(IDC_COMBO_MIN_END);
	CComboBox* pComboAMPMEnd = (CComboBox*) GetDlgItem(IDC_COMBO_AM_PM_END);


	((CButton*) GetDlgItem(IDC_CHECK_STOP))->SetCheck(CheckStop); 
	OnCheckChange();

	int n;
	TCHAR szNumber[32];

	for(n = 1; n <= 12; n++)
	{
		sprintf(szNumber, "%02d", n);
		pComboHour->AddString(szNumber);
		pComboHourEnd->AddString(szNumber);
	}

	for(n = 0; n <= 60; n++)	
//	for(n = 0; n <= 55; n += 5)
	{
		sprintf(szNumber, "%02d", n);
		pComboMin->AddString(szNumber);
		pComboMinEnd->AddString(szNumber);
	}

	pComboAMPM->AddString("AM");
	pComboAMPM->AddString("PM");
	pComboAMPMEnd->AddString("AM");
	pComboAMPMEnd->AddString("PM");

	pComboHour->SetCurSel(m_nInit_Hour);
	pComboMin->SetCurSel(m_nInit_Min);
	pComboAMPM->SetCurSel(m_nInit_AMPM);
	pComboHourEnd->SetCurSel(m_nInit_Hour_End);
	pComboMinEnd->SetCurSel(m_nInit_Min_End);
	pComboAMPMEnd->SetCurSel(m_nInit_AMPM_End);

	UpdateTimeToGo();


	if(CheckRepeat == TRUE) OnOK();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDelayedPostingDlg::OnOK() 
{
	UpdateTimeToGo();
	CDialog::OnOK();
	CheckRepeat = IsDlgButtonChecked(IDC_CHECK_REPEAT)? true : false;
}

void CDelayedPostingDlg::UpdateTimeToGo()
{
	TCHAR szTime[128];
	TCHAR szText[64];
	TCHAR szTimeEnd[128];
	TCHAR szTextEnd[64];

	CComboBox* pComboHour = (CComboBox*) GetDlgItem(IDC_COMBO_HOUR);
	CComboBox* pComboMin  = (CComboBox*) GetDlgItem(IDC_COMBO_MIN);
	CComboBox* pComboAMPM = (CComboBox*) GetDlgItem(IDC_COMBO_AM_PM);

	CComboBox* pComboHourEnd = (CComboBox*) GetDlgItem(IDC_COMBO_HOUR_END);
	CComboBox* pComboMinEnd  = (CComboBox*) GetDlgItem(IDC_COMBO_MIN_END);
	CComboBox* pComboAMPMEnd = (CComboBox*) GetDlgItem(IDC_COMBO_AM_PM_END);

	// Save Selection
	m_nInit_Hour = pComboHour->GetCurSel();
	m_nInit_Min  = pComboMin->GetCurSel();
	m_nInit_AMPM = pComboAMPM->GetCurSel();

	m_nInit_Hour_End = pComboHourEnd->GetCurSel();
	m_nInit_Min_End = pComboMinEnd->GetCurSel();
	m_nInit_AMPM_End = pComboAMPMEnd->GetCurSel();

	pComboHour->GetWindowText(szText, 32);
	int nSelectedHour = atoi(szText);

	pComboMin->GetWindowText(szText, 32);
	int nSelectedMin = atoi(szText);

	pComboAMPM->GetWindowText(szText, 32);
	if(strcmp(szText, "PM") == 0)
	{
		// PM - if 12 PM, then don't add 12 hours
		if(nSelectedHour != 12)
			nSelectedHour += 12;
	}
	else
	{
		// AM - if 12 AM, then subtract 12 hours
		if(nSelectedHour == 12)
			nSelectedHour = 0;
	}

	TRACE ("UpdateTimeToGo: ===============================\n");
	TRACE2("UpdateTimeToGo: Selected %d HH, %d MM\n", nSelectedHour, nSelectedMin);

	CTime theTime = CTime::GetCurrentTime();	
	
	int nStartMin = (nSelectedHour * 60) + nSelectedMin;
	int nCurMin = (theTime.GetHour() * 60) + theTime.GetMinute();

	TRACE2("UpdateTimeToGo: Selected Min = %d, Current Min = %d\n", nStartMin, nCurMin);

	CTime theTime2 = theTime;

	if(nStartMin < nCurMin)
	{
		// Start on next day
		CTimeSpan timeSpan_1_Day(1,0,0,0);
		theTime2 += timeSpan_1_Day;
		TRACE2("UpdateTimeToGo: Adding 1 Day\n", nStartMin, nCurMin);
	}

	int nStart_Year = theTime2.GetYear();
	int nStart_Month = theTime2.GetMonth();
	int nStart_Day = theTime2.GetDay();
	int nStart_Hour = theTime2.GetHour();
	int nStart_Minute = theTime2.GetMinute();

	CTime startTime( nStart_Year, nStart_Month, nStart_Day, nSelectedHour, nSelectedMin, 0);

	TRACE3("UpdateTimeToGo: Start Time: %d Y, %d M, %d D ", nStart_Year, nStart_Month, nStart_Day);
	TRACE2("%d HH, %d MM\n", nStart_Hour, nStart_Minute);

	CTimeSpan timeToGo = startTime - theTime;

	m_StartTime = startTime;

	sprintf(szTime, "Post will start in %d Hours, %d Minutes", timeToGo.GetHours(), timeToGo.GetMinutes());
	SetDlgItemText(IDC_STATIC_TIME, szTime);


// BEGINNING END TIME SELECT

	pComboHourEnd->GetWindowText(szTextEnd, 32);
	int nSelectedHourEnd = atoi(szTextEnd);

	pComboMinEnd->GetWindowText(szTextEnd, 32);
	int nSelectedMinEnd = atoi(szTextEnd);

	pComboAMPMEnd->GetWindowText(szTextEnd, 32);
	if(strcmp(szTextEnd, "PM") == 0)
	{
		// PM - if 12 PM, then don't add 12 hours
		if(nSelectedHourEnd != 12)
			nSelectedHourEnd += 12;
	}
	else
	{
		// AM - if 12 AM, then subtract 12 hours
		if(nSelectedHourEnd == 12)
			nSelectedHourEnd = 0;
	}

	TRACE ("UpdateTimeToGo: ===============================\n");
	TRACE2("UpdateTimeToGo: Selected %d HH, %d MM\n", nSelectedHourEnd, nSelectedMinEnd);

	CTime theTimeEnd = CTime::GetCurrentTime();	
	
	int nStartMinEnd = (nSelectedHourEnd * 60) + nSelectedMinEnd;
//	int nCurMinEnd = (theTimeEnd.GetHour() * 60) + theTimeEnd.GetMinute();
	int nCurMinEnd = ((startTime.GetDay()-theTimeEnd.GetDay()) * 24 * 60) + (startTime.GetHour() * 60) + startTime.GetMinute();

	TRACE2("UpdateTimeToGo: Selected Min = %d, Current Min = %d\n", nStartMinEnd, nCurMinEnd);

	CTime theTime2End = theTimeEnd;

	if(nStartMinEnd < nCurMinEnd)
	{
		// Start on next day
		CTimeSpan timeSpan_2_Day(1+startTime.GetDay()-theTimeEnd.GetDay(),0,0,0);
		theTime2End += timeSpan_2_Day;
		TRACE2("UpdateTimeToGo: Adding 1 Day\n", nStartMinEnd, nCurMinEnd);
	}

	int nStart_YearEnd = theTime2End.GetYear();
	int nStart_MonthEnd = theTime2End.GetMonth();
	int nStart_DayEnd = theTime2End.GetDay();
	int nStart_HourEnd = theTime2End.GetHour();
	int nStart_MinuteEnd = theTime2End.GetMinute();

	CTime startTimeEnd( nStart_YearEnd, nStart_MonthEnd, nStart_DayEnd, nSelectedHourEnd, nSelectedMinEnd, 0);

	TRACE3("UpdateTimeToGo: Start TimeEnd: %d Y, %d M, %d D ", nStart_YearEnd, nStart_MonthEnd, nStart_DayEnd);
	TRACE2("%d HH, %d MM\n", nStart_HourEnd, nStart_MinuteEnd);

	CTimeSpan timeToGoEnd = startTimeEnd - theTimeEnd;

	m_StartTimeEnd = startTimeEnd;

	sprintf(szTimeEnd, "Post will stop in %d Days, %d Hours, %d Minutes", timeToGoEnd.GetDays(), timeToGoEnd.GetHours(), timeToGoEnd.GetMinutes());
	SetDlgItemText(IDC_STATIC_TIME_END, szTimeEnd);
}

void CDelayedPostingDlg::OnSelchangeComboHour() 
{
	UpdateTimeToGo();
}

void CDelayedPostingDlg::OnSelchangeComboMin() 
{
	UpdateTimeToGo();	
}

void CDelayedPostingDlg::OnSelchangeComboAmPm()
{
	UpdateTimeToGo();	
}

void CDelayedPostingDlg::OnSelchangeComboHourEnd() 
{
	UpdateTimeToGo();
}

void CDelayedPostingDlg::OnSelchangeComboMinEnd() 
{
	UpdateTimeToGo();	
}

void CDelayedPostingDlg::OnSelchangeComboAmPmEnd()
{
	UpdateTimeToGo();	
}

void CDelayedPostingDlg::OnCheckChange()
{
  bool vChecked = IsDlgButtonChecked(IDC_CHECK_STOP)? true : false; 

  GetDlgItem(IDC_COMBO_HOUR_END)->EnableWindow(vChecked); 
  GetDlgItem(IDC_COMBO_MIN_END)->EnableWindow(vChecked); 
  GetDlgItem(IDC_COMBO_AM_PM_END)->EnableWindow(vChecked);
  m_StopValue = vChecked;
  CheckStop = vChecked;
}