#pragma once
struct timer_info
{
	int timer_id;
	int seconds;
	TIMERPROC timer_proc;
};

DWORD CALLBACK Thread(PVOID   pvoid);
bool start_timer(int timer_id, int seconds, TIMERPROC timer_proc);
bool finish_timer(DWORD thread_id);
bool wait_thread();

extern DWORD thread_id;
extern HANDLE h_thread;
