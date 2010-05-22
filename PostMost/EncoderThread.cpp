#include "stdafx.h"
#include "PostMost.h"

#include "EncoderThread.h"

#include "PostMostDoc.h"
#include "PostMostView.h"

#include "MainFrm.h"

#include "EncodingProgressDlg.h"

#include "par2format.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEncoderThread* CEncoderThread::Create(CPostMostView* view, CProgramSettings* settings)
{
	CEncoderThread* instance = new CEncoderThread();
	if (!instance->Init(view,settings))
	{
		delete instance;
		return NULL;
	}
	else
	{
		return instance;
	}
}

CEncoderThread::CEncoderThread()
: m_NewTask(FALSE,FALSE) // auto reset
, m_Mutex(TRUE)
{
	m_STOP=false;
	//m_READY=false;
	m_Task=NULL;
	stat_numbatch = 0;
	stat_CurrentFile[0]='\0';
	stat_CurrentFileNum = 0;
	stat_TotalFiles = 0;
	stat_CurrentPer1000 = 0;
	stat_octets_processed = 0;

	m_Mutex.Unlock();
}

CEncoderThread::~CEncoderThread()
{
	CSingleLock(&m_Mutex,TRUE);
	if (m_Task=NULL)
	{
	}
}

bool CEncoderThread::Init(CPostMostView* view, CProgramSettings* settings)
{
	m_View = view;
	if (settings!=NULL)
		m_Settings.Copy(*settings);

	//AfxBeginThread(&ThreadFunction,this,settings->m_nEncoderPriority,0,0,NULL);
	AfxBeginThread(&ThreadFunction,this,THREAD_PRIORITY_BELOW_NORMAL,0,0,NULL);

	return true;
}

bool CEncoderThread::STOP() // we can only set to true never back to false!
{
	CSingleLock lock(&m_Mutex,TRUE);
	if (m_STOP) return false;
	m_STOP=true;
	m_NewTask.SetEvent();
	return true;
}

unsigned int CEncoderThread::ThreadFunction(void* arg)
{
	return ((CEncoderThread*)arg)->ThreadMethod();
}

void CEncoderThread::clearTask()
{
	CSingleLock lock(&m_Mutex,TRUE);
	m_Task=NULL;
}

bool CEncoderThread::sendTask(par2::PostPar2Volumes* task)
{
	CSingleLock lock(&m_Mutex,TRUE);
	if (m_Task!=NULL || m_STOP)
		return false; // not ready
	m_Task=task; // set the new task
	m_NewTask.SetEvent(); // notify the thread
	return true; // that's it :)
}

par2::PostPar2Volumes* CEncoderThread::getNewTask()
{
	CSingleLock lock(&m_Mutex,TRUE);
	CSingleLock wait(&m_NewTask,FALSE);
	while (m_Task==NULL && !m_STOP)
	{
		lock.Unlock();
		::PostMessage(m_View->GetSafeHwnd(), WM_ENCTHREAD_READY, (WPARAM)this, 0);
		wait.Lock();
		wait.Unlock();
		lock.Lock();
	}
	return (m_STOP?NULL:m_Task);
}

void CEncoderThread::sendCompletedTask(par2::PostPar2Volumes* task)
{
	{
		CSingleLock lock(&m_Mutex,TRUE);
		m_Task = NULL;
	}
	::PostMessage(m_View->GetSafeHwnd(), WM_ENCTHREAD_DONE, (WPARAM)this, (LPARAM)task);
}	

void CEncoderThread::sendStopped(par2::PostPar2Volumes* task)
{
	{
		//CSingleLock lock(&m_Mutex,TRUE);
		//m_Task = NULL;
	}
	::PostMessage(m_View->GetSafeHwnd(), WM_ENCTHREAD_STOPPED, (WPARAM)this, (LPARAM)0); //task);
}

unsigned int CEncoderThread::ThreadMethod()
{
	par2::PostPar2Volumes* task = NULL;
	
	CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	CString progress;

	while (!m_STOP)
	{
		task = getNewTask();
		if (task==NULL || m_STOP) break;
		{
			CSingleLock lock(&m_Mutex,TRUE);
			stat_VolumeFiles.RemoveAll();
			for (int k=0;k<task->nb_vol_files;k++)
				stat_VolumeFiles.Add(task->vol_files_name[k]);
			stat_CurrentFile[0]='\0';
			stat_CurrentFileNum = 0;
			stat_TotalFiles = task->sources->nbfiles;
			stat_CurrentPer1000 = 0;
			++stat_numbatch;
			stat_octets_processed = 0;
		}

		int f=0;
		par2::u64 ofs,size;
		task->GetNextFragmentInfo(&f,&ofs,&size);

		while (f<task->sources->nbfiles && !m_STOP)
		{
			stat_CurrentFileNum = f;
			//memset(stat_CurrentFile,0,_MAX_PATH);
			//strncpy(stat_CurrentFile,task->sources->files[f].name,_MAX_PATH);
			stat_CurrentPer1000 = 0;
			_snprintf(stat_CurrentFile,_MAX_PATH,"Processing file %s...",trimdir(task->sources->files[f].name));
			//pMainFrame->SetStatusText(progress);
			if (!task->ProcessFile(f,m_STOP,stat_CurrentPer1000,stat_octets_processed)) break;
			++f;
			stat_CurrentPer1000 = 1000;
		}
		stat_CurrentFileNum = f;
		stat_CurrentPer1000 = 0;
/*
		if (m_STOP && task->isReady())
		{
			task->errorMsg == "Stopped.";
		}
*/
		if (task->isReady())
		{ // finished
			//progress.Format("Finishing encoding batch...");
			//pMainFrame->SetStatusText(progress);
			_snprintf(stat_CurrentFile,_MAX_PATH,"Finalizing...");
			task->Finish(stat_CurrentPer1000);
			stat_CurrentPer1000 = 1000;
			_snprintf(stat_CurrentFile,_MAX_PATH,"Done.");
		}
		{
			CSingleLock lock(&m_Mutex,TRUE);
			stat_VolumeFiles.RemoveAll();
			stat_CurrentFile[0]='\0';
			stat_CurrentFileNum = 0;
			stat_TotalFiles = 0;
			stat_CurrentPer1000 = 0;
			stat_CurrentPer1000 = 1000;
			++stat_numbatch;
			stat_octets_processed = 0;
		}
		if (!m_STOP)
		{
			sendCompletedTask(task);
			task = NULL;
		}
	}

	if (task!=NULL && task->errorMsg.GetLength()==0)
	{
		task->errorMsg = "Stopped.";
	}

	sendStopped(task);
	return 0;	
}
