#include "stdafx.h"
#include "teamviewer.h"
#include "launcher.h"
#include "common.h"
#include "timer.h"
bool teamviewer_finding;
bool teamviewer_construct_command( const CMapStringToString &parameters, CString &cmd )
{
	CString progname;
	cmd.Empty();
	if (parameters.Lookup(PROGNAME_STRING, progname))
		cmd = progname;
	else
		return false;

	return true;
}

bool teamviewer_resolve_connect_info( const CMapStringToString &parameters, CString &id, CString &password, CString &assistant_type )
{
	if (!parameters.Lookup(ID_STRING, id))
		return false;
	if (!parameters.Lookup(PASSWORD_STRING, password))
		return false;
	parameters.Lookup(ASSISTANT_TYPE_STRING, assistant_type);
	return true;
}

bool teamviewer_find_main_window( int seconds, HWND &h_wnd )
{
	bool found = false;
	HWND wnd = 0;
	teamviewer_finding = true;
	if (!wait_thread())
	{
		return false;
	}
	start_timer(TEAMVIEWER_TIMER_ID, seconds, teamviewer_timer_proc);
#ifdef _DEBUG
	_tprintf(TEXT("wait for the main window\n"));
#endif

	while (teamviewer_finding)
	{
		if ((found = find_window(TEAMVIEWER_TITLE, 1, wnd)))
		{
#ifdef _DEBUG
			_tprintf(TEXT("main window found\n"));
#endif
			teamviewer_finish_timer();
		}
	}
	h_wnd = wnd;
	return found;
}

bool teamviewer_wait_initialization( HWND h_wnd, int seconds )
{
	bool result = false;
	teamviewer_finding = true;
	if (!wait_thread())
	{
		return false;
	}
	start_timer(TEAMVIEWER_TIMER_ID, seconds, teamviewer_timer_proc);
#ifdef _DEBUG
	_tprintf(TEXT("wait for teamviewer initialize\n"));
#endif

	while (teamviewer_finding)
	{
		HWND h_child = FindWindowEx(h_wnd, NULL, TEXT("Edit"), TEXT(""));
		if (!h_child)
		{
			result = true;
#ifdef _DEBUG
			_tprintf(TEXT("initialize signal found\n"));
#endif
			teamviewer_finish_timer();
		}
	}
	return result;
}

bool teamviewer_connect( CString cmd, CString id, CString password, int seconds )
{
	if (!create_process(cmd))
	{
		//MessageBox(0, TEXT("启动程序失败"), TEXT("启动程序失败"), MB_OK);
		return false;
	}
	Sleep(1000);
	HWND h_wnd = 0;

	if (!teamviewer_find_main_window(seconds, h_wnd))
	{
		//MessageBox(0, TEXT("连接超时"), TEXT("连接超时"), MB_OK);
		return false;
	}
	//if (!teamviewer_wait_initialization(h_wnd, TEAMVIEWER_INITIALIZATION_RETRY))
	//{
	//}

	HWND h_child_id_combo = ::FindWindowEx(h_wnd, NULL, _T("ComboBox"), NULL);
	HWND h_child_id_edit = ::FindWindowEx(h_child_id_combo, NULL, _T("Edit"), NULL);
	HWND h_child_ok = ::FindWindowEx(h_wnd, h_child_id_combo, _T("Button"), TEAMVIEWER_BUTTON_CONNECT);

	//id.Insert(3, ' ');
	//id.Insert(7, ' ');
	fill_text(h_child_id_edit, id);
	Sleep(100);
	click_button(h_child_ok);
	HWND h_result = 0;
	if (teamviewer_probe_result(TEAMVIEWER_AUTHENTICATION_RETRY, h_result))
	{
		//正常
		return teamviewer_authenticate(h_result, password);
	}
	else
	{
		//错误
		return false;
	}
}

bool teamviewer_probe_result(int seconds, HWND &h_wnd)
{
	bool found = false;
	bool result = false;
	HWND wnd = 0;
	teamviewer_finding = true;
	if (!wait_thread())
	{
		return false;
	}
	start_timer(TEAMVIEWER_TIMER_ID, seconds, teamviewer_timer_proc);
#ifdef _DEBUG
	_tprintf(TEXT("wait for the password window or error window\n"));
#endif

	while (teamviewer_finding)
	{
		if ((found = find_window(TEAMVIEWER_ERROR_TITLE, 1, wnd)))
		{
#ifdef _DEBUG
			_tprintf(TEXT("error window found\n"));
#endif
			result = false;
			teamviewer_finish_timer();
		}
		else if ((found = find_window(TEAMVIEWER_PASSWORD_TITLE, 1, wnd)))
		{
#ifdef _DEBUG
			_tprintf(TEXT("password window found\n"));
#endif
			result = true;
			teamviewer_finish_timer();
		}
		else if (found = 
			(find_window(TEAMVIEWER_TITLE, 1, wnd) && FindWindowEx(wnd, 0, TEXT("Static"), TEAMVIEWER_INVALID_ID_MESSAGE)))
		{
#ifdef _DEBUG
			_tprintf(TEXT("invalid id window found\n"));
#endif
			result = true;
			teamviewer_finish_timer();
		}
	}
	h_wnd = wnd;
	return result;

}

bool teamviewer_authenticate( HWND h_password, CString password )
{
	ASSERT(h_password);
	HWND h_wnd = h_password;

	HWND h_child_password_edit = ::FindWindowEx(h_wnd, NULL, _T("Edit"), NULL);

	fill_text(h_child_password_edit, password);
	//Sleep(15000);
	HWND h_child_login_button = ::FindWindowEx(h_wnd, NULL, _T("Button"), TEAMVIEWER_BUTTON_LOGIN);
#ifdef _DEBUG
	//_tprintf(TEXT("%d\n"), h_child_login_button);
#endif
	click_button(h_child_login_button);
	return true;
}

VOID CALLBACK teamviewer_timer_proc( HWND hwnd, UINT u_msg, UINT_PTR id_event, DWORD dw_time )
{
#ifdef _DEBUG
	_tprintf(TEXT("timeout; ID:%d\n"), id_event);
#endif
	teamviewer_finish_timer();
}

bool teamviewer_finish_timer()
{
	teamviewer_finding = false;
	return finish_timer(thread_id);
}