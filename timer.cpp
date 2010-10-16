#include "stdafx.h"
#include "timer.h"
DWORD thread_id;
HANDLE h_thread;
DWORD CALLBACK Thread(PVOID   pvoid)   
{
	MSG  msg;
	PeekMessage(&msg,NULL,WM_USER,WM_USER,PM_NOREMOVE);   
	timer_info *p_timer_info = (timer_info *)pvoid;
	UINT  timer_id=SetTimer(NULL, p_timer_info->timer_id, p_timer_info->seconds * 1000, p_timer_info->timer_proc);   
	BOOL  bRet;

	while((bRet = GetMessage(&msg,NULL,0,0))!=0)   
	{     
		if(bRet==-1)   
		{   
			//   handle   the   error   and   possibly   exit   
		}   
		else   
		{
			TranslateMessage(&msg);     
			DispatchMessage(&msg);     
		}
	}
	KillTimer(NULL,timer_id); 
	delete p_timer_info;
#ifdef _DEBUG
	_tprintf(TEXT("thread end here\n")); 
#endif  
	return 0;   
}

bool start_timer(int timer_id, int seconds, TIMERPROC timer_proc)
{
	timer_info *p_timer_info = new timer_info;
	p_timer_info->timer_id = timer_id;
	p_timer_info->seconds = seconds;
	p_timer_info->timer_proc = timer_proc;
	h_thread = CreateThread(NULL,0,Thread,p_timer_info,0,&thread_id);
	return true;
}

bool finish_timer(DWORD thread_id)
{
#ifdef _DEBUG
	_tprintf(TEXT("post thread quit message\n"));
#endif
	PostThreadMessage(thread_id, WM_QUIT, 0, 0);
	return true;
}

bool wait_thread()
{
	if (WAIT_TIMEOUT == WaitForSingleObject(h_thread, 2000))
	{
#ifdef _DEBUG
		_tprintf(TEXT("wait for the thread timeout\n"));
#endif
		return false;
	}
	return true;
}