#include "stdafx.h"
#include "ttvnc.h"
#include "common.h"
#include "timer.h"
bool ttvnc_finding;

bool ttvnc_construct_command( const CMapStringToString &parameters, CString &cmd )
{
	CString progname;
	cmd.Empty();
	if (parameters.Lookup(PROGNAME_STRING, progname))
		cmd = progname;
	else
		return false;
	return true;
}

bool ttvnc_resolve_assist_info( const CMapStringToString &parameters, CString &code, CString &assistant_mode )
{
	if (!parameters.Lookup(CODE_STRING, code))
		return false;
	if (!parameters.Lookup(ASSISTANT_MODE_STRING, assistant_mode))
		return false;
	return true;
}

bool ttvnc_find_window( int seconds, HWND &h_wnd )
{
	bool found = false;
	CString ttvnc_titles[] =
	{
		TTVNC_TITLE0,
		TTVNC_TITLE1,
	};

	HWND wnd = 0;
	ttvnc_finding = true;
	if (!wait_thread())
	{
		return false;
	}
	start_timer(TTVNC_TIMER_ID, seconds, ttvnc_timer_proc);
	while (ttvnc_finding)
	{
		for (int i = 0; i < TTVNC_TITLE_COUNT; ++i)
		{
			if ((found = find_window(ttvnc_titles[i], 1, wnd)))
			{
				ttvnc_finish_timer();
				break;
			}
		}
	}
	h_wnd = wnd;
	return found;
}

bool ttvnc_assist( CString cmd, CString code, CString assist_type, int seconds )
{
	if (!create_process(cmd))
	{
		//MessageBox(0, TEXT("启动程序失败"), TEXT("启动程序失败"), MB_OK);
		return false;
	}
	Sleep(1000);
	HWND h_wnd = 0;

	if (!ttvnc_find_window(seconds, h_wnd))
	{
		//MessageBox(0, TEXT("连接超时"), TEXT("连接超时"), MB_OK);
		return false;
	}
	Sleep(100);
	HWND h_child_code_edit = ::FindWindowEx(h_wnd, NULL, _T("Edit"), 0);
	HWND h_child_assist_button = ::FindWindowEx(h_wnd, h_child_code_edit, TEXT("Button"), TTVNC_BUTTON_ASSIST);
	switch (assist_type[0])
	{
	case TTVNC_ASSISTMODE_ASSIST_CHAR:
		break;
	case TTVNC_ASSISTANTMODE_ASSISTED_CHAR:
		h_child_assist_button = ::FindWindowEx(h_wnd, h_child_code_edit, TEXT("Button"), TTVNC_BUTTON_ASSISTED);
		break;
	}
	fill_text(h_child_code_edit, code);
	Sleep(100);
	click_button(h_child_assist_button);
	return true;
}

VOID CALLBACK ttvnc_timer_proc( HWND hwnd, UINT u_msg, UINT_PTR id_event, DWORD dw_time )
{
#ifdef _DEBUG
	_tprintf(TEXT("timeout; ID:%d\n"), id_event);
#endif
	ttvnc_finish_timer();
}

bool ttvnc_finish_timer()
{
	ttvnc_finding = false;
	return finish_timer(thread_id);
}