#include "stdafx.h"
#include "radmin.h"
#include "launcher.h"
#include "common.h"
#include "timer.h"
#include <stdio.h>
bool radmin_finding;
//根据解析完的参数，构造命令行，如下
//"Radmin.exe /connect:219.216.101.200"
bool radmin_construct_command(const CMapStringToString &parameters, CString &cmd)
{
	CString progname, ip, port, username, password;
	cmd.Empty();
	if (parameters.Lookup(PROGNAME_STRING, progname))
		cmd = progname;
	else
		return false;
	if (parameters.Lookup(IP_STRING, ip) && ip.GetLength() > 0)
	{
		cmd += " /connect:";
		cmd += ip;
		if (parameters.Lookup(PORT_STRING, port) && port.GetLength() > 0)
		{
			cmd += ":";
			cmd += port;
		}
	}

	return true;
}

//解析登录信息，即ip，用户名，密码
bool radmin_resolve_login_info( const CMapStringToString &parameters, CString &ip, CString &username, CString &password )
{
	if (!parameters.Lookup(IP_STRING, ip))
		return false;
	if (!parameters.Lookup(PASSWORD_STRING, password))
		return false;
	parameters.Lookup(USERNAME_STRING, username);
	return true;
}

//根据窗口名，查找radmin窗口
//设置超时等待，如果超时，则返回false
bool radmin_find_window(CString ip, int seconds, HWND &h_wnd, bool &zh)
{
	bool found = false;
	CString radmin_titles[] =
	{
		RADMIN_TITLE_ZH0,
		RADMIN_TITLE_ZH1,
		RADMIN_TITLE_EN0,
		RADMIN_TITLE_EN1,
	};

	for (int i = 0; i < RADMIN_TITLE_COUNT_ZH + RADMIN_TITLE_COUNT_EN; ++i)
		radmin_titles[i] += ip;

	HWND wnd = 0;
	radmin_finding = true;
	if (!wait_thread())
	{
		return false;
	}
	start_timer(RADMIN_TIMER_ID, seconds, radmin_timer_proc);
#ifdef _DEBUG
	_tprintf(TEXT("wait for the window\n"));
#endif
	
	while (radmin_finding)
		for (int i = 0; i < RADMIN_TITLE_COUNT_ZH + RADMIN_TITLE_COUNT_EN; ++i)
			if ((found = find_window(radmin_titles[i], 1, wnd)))
			{
#ifdef _DEBUG
				_tprintf(TEXT("window found\n"));
#endif
				if (i < RADMIN_TITLE_COUNT_ZH)
					zh = true;
				else
					zh = false;
				radmin_finish_timer();
				break;
			}
	h_wnd = wnd;
	return found;
}

//主要的登录动作
//首先创建进程，然后查找主窗口，然后查找用户名和密码输入框，如果没有用户名输入框，则只输密码
bool radmin_login(CString cmd, CString ip, CString username, CString password, int seconds)
{
	if (!create_process(cmd))
	{
		//MessageBox(0, TEXT("启动程序失败"), TEXT("启动程序失败"), MB_OK);
		return false;
	}
	//Sleep(1000);
	HWND h_wnd = 0;
	bool zh = true;

	if (!radmin_find_window(ip, seconds, h_wnd, zh))
	{
		//MessageBox(0, TEXT("连接超时"), TEXT("连接超时"), MB_OK);
		return false;
	}

	while (!IsWindowVisible(h_wnd));
	ShowWindow(h_wnd, SW_HIDE);

	HWND h_child_username_edit = ::FindWindowEx(h_wnd, NULL, _T("Edit"), 0);
	HWND h_child_password_edit = ::FindWindowEx(h_wnd, h_child_username_edit, _T("Edit"), 0);
	if (!h_child_password_edit)
	{
		h_child_password_edit = h_child_username_edit;
		h_child_username_edit = NULL;
	}
	CString button_ok;
	if (zh)
		button_ok = RADMIN_BUTTON_OK_ZN;
	else
		button_ok = RADMIN_BUTTON_OK_EN;
	HWND h_child_ok_button = ::FindWindowEx(h_wnd, NULL, _T("Button"), button_ok);

	fill_text(h_child_username_edit, username);

	fill_text(h_child_password_edit, password);

	Sleep(100);
	click_button(h_child_ok_button);
	return true;
}

//超时定时器，如果超时，则让线程退出
VOID CALLBACK radmin_timer_proc( HWND hwnd, UINT u_msg, UINT_PTR id_event, DWORD dw_time )
{
#ifdef _DEBUG
	_tprintf(TEXT("timeout; ID:%d\n"), id_event);
#endif
	radmin_finish_timer();
}

bool radmin_finish_timer()
{
	radmin_finding = false;
	return finish_timer(thread_id);
}