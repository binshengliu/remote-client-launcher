#include "stdafx.h"
#include "mstsc.h"
#include "common.h"
#include "timer.h"
bool mstsc_finding;
bool mstsc_construct_command(const CMapStringToString &parameters, CString &cmd)
{
	CString progname, ip, port, username, password;
	cmd.Empty();
	if (parameters.Lookup(PROGNAME_STRING, progname))
		cmd = progname;
	else
		return false;
	if (parameters.Lookup(IP_STRING, ip) && ip.GetLength() > 0)
	{
		cmd += " /v:";
		cmd += ip;
		if (parameters.Lookup(PORT_STRING, port) && port.GetLength() > 0)
		{
			cmd += ":";
			cmd += port;
		}
	}
	return true;
}

bool mstsc_resolve_connect_info( const CMapStringToString &parameters, CString &ip )
{
	if (!parameters.Lookup(IP_STRING, ip))
		return false;
	return true;
}

bool mstsc_connect( CString cmd, CString ip)
{
	if (!create_process(cmd))
	{
		//MessageBox(0, TEXT("启动程序失败"), TEXT("启动程序失败"), MB_OK);
		return false;
	}
	Sleep(1000);
	HWND h_wnd;
	if (mstsc_probe_result(ip, MSTSC_AUTHENTICATION_RETRY, h_wnd))
	{
		//正常登录
	}
	else
	{
		//警告窗口
		HWND h_disable_button = FindWindowEx(h_wnd, NULL, TEXT("Button"), MSTSC_WARNING_DISABLE);
		click_button(h_disable_button);
		HWND h_yes_button = FindWindowEx(h_wnd, NULL, TEXT("Button"), MSTSC_YES_BUTTON);
		click_button(h_yes_button);
	}
	return true;
}

bool mstsc_probe_result( CString ip, int seconds, HWND &h_wnd )
{
	bool found = false;
	bool result = false;
	HWND wnd = 0;
	mstsc_finding = true;
	CString remote_title = ip + MSTSC_REMOTE_TITLE;
	if (!wait_thread())
	{
		return false;
	}
	start_timer(MSTSC_TIMER_ID, seconds, mstsc_timer_proc);
#ifdef _DEBUG
	_tprintf(TEXT("wait for the warning window or remote window\n"));
#endif

	while (mstsc_finding)
	{
		if ((found = find_window(MSTSC_WARNING_TITLE, 1, wnd)))
		{
#ifdef _DEBUG
			_tprintf(TEXT("warning window found\n"));
#endif
			result = false;
			mstsc_finish_timer();
		}
		else if (NULL != (wnd = FindWindow(TEXT("TscShellContainerClass"), remote_title)))
		{
			found = true;
#ifdef _DEBUG
			_tprintf(TEXT("remote window found\n"));
#endif
			result = true;
			mstsc_finish_timer();
		}
	}
	h_wnd = wnd;
	return result;
}

VOID CALLBACK mstsc_timer_proc( HWND hwnd, UINT u_msg, UINT_PTR id_event, DWORD dw_time )
{
#ifdef _DEBUG
	_tprintf(TEXT("timer timeout; ID:%d\n"), id_event);
#endif
	mstsc_finish_timer();
}

bool mstsc_finish_timer()
{
	mstsc_finding = false;
	return finish_timer(thread_id);
}

//void mstsc_connect(CString ip, CString port, CString username, CString password, int seconds)
//{
//	CString data = _T("what");
//	CString title = _T("Remote Desktop Connection");
//
//	HWND hWnd = ::FindWindow(_T("#32770"), title);
//	HWND hChildComboBoxEx32 = ::FindWindowEx(hWnd, NULL, _T("ComboBoxEx32"), NULL);
//	HWND hChildComboBox = ::FindWindowEx(hChildComboBoxEx32, NULL, _T("ComboBox"), NULL);
//	HWND hChildEdit = ::FindWindowEx(hChildComboBox, NULL, _T("Edit"), NULL);
//	//HWND hChildPassword = ::FindWindowEx(hWnd, hChildUsername, _T("Edit"), _T(""));
//	//HWND hChildOK = ::FindWindowEx(hWnd, hChildUsername, _T("Button"), _T("OK"));
//	HWND hChildConnect = ::FindWindowEx(hWnd, hChildComboBoxEx32, _T("Button"), _T("Co&nnect"));
//
//	fill_text(hChildEdit, data);
//	Sleep(500);
//	click_button(hChildConnect);
//}
