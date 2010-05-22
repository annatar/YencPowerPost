#ifndef ENCODER_THREAD_H
#define ENCODER_THREAD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// EncoderThread.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEncoderThread thread

#include "par2format.h"

class CPostMostView;

class CEncoderThread
{

// Attributes
public:

	void setSettings(CProgramSettings* newSettings);

	par2::PostPar2Volumes* getTask() { return m_Task; }
	bool getSTOP() { return m_STOP; }
	bool STOP(); // we can only set to true never back to false!

	bool isReady() { return (m_Task==NULL) && !m_STOP; }

	bool sendTask(par2::PostPar2Volumes* task);
	void clearTask();


// Stats
public:
	int stat_numbatch;
	CStringArray stat_VolumeFiles;

	int stat_CurrentFileNum;
	char stat_CurrentFile[_MAX_PATH];
	int stat_TotalFiles;

	par2::u64 stat_octets_processed;

	int stat_CurrentPer1000;

// Properties
public:
	//HWND			  m_hWndNotify;
	//CThreadMarshal*   m_pThreadMarshal;
	CPostMostView*    m_View;

	bool              m_STOP;

	CMutex m_Mutex;
	// MUTEX PROTECTED DATA

	CProgramSettings  m_Settings;
	par2::PostPar2Volumes*  m_Task;

	CEvent m_NewTask;

// Operations
public:

	static CEncoderThread* Create(CPostMostView* view, CProgramSettings* settings);
	void Delete()
	{
		delete this;
	}

// Implementation
protected:
	CEncoderThread();
	bool Init(CPostMostView* view, CProgramSettings* settings);
	virtual ~CEncoderThread();

	unsigned int ThreadMethod();

	static unsigned int ThreadFunction(void* arg);
	// internal fn

	par2::PostPar2Volumes* getNewTask();
	void sendCompletedTask(par2::PostPar2Volumes* task);
	void sendStopped(par2::PostPar2Volumes* task);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // ifndef ENCODER_THREAD_H
