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
	//else
	//	return false;
	//if (parameters.Lookup(IP_STRING, ip) && ip.GetLength() > 0)
	//{
	//	cmd += " /v:";
	//	cmd += ip;
	//	if (parameters.Lookup(PORT_STRING, port) && port.GetLength() > 0)
	//	{
	//		cmd += ":";
	//		cmd += port;
	//	}
	//}
	return true;
}

bool mstsc_resolve_connect_info( const CMapStringToString &parameters, CString &ip, CString &username, CString &password )
{
	if (!parameters.Lookup(IP_STRING, ip))
		return false;
	if (!parameters.Lookup(PASSWORD_STRING, password))
		return false;
	if (!parameters.Lookup(USERNAME_STRING, username))
		return false;
	return true;
}

bool mstsc_connect( CString cmd, CString ip, CString username, CString password, int seconds)
{
	if (!create_process(cmd))
	{
		//MessageBox(0, TEXT("启动程序失败"), TEXT("启动程序失败"), MB_OK);
		return false;
	}
	//Sleep(500);

	HWND h_main;
	if (!mstsc_find_main_window(MSTSC_CONNECT_RETRY, h_main))
		return false;

	while (!IsWindowVisible(h_main));
	//填入IP和密码
	HWND h_option = FindWindowEx(h_main, NULL, _T("Button"), _T("选项(&O) >>"));
	printf("%d------\n", h_option);
	click_button(h_option);
	Sleep(100);
	//通过FindWindowEx一层一层地找到IP地址的输入控件
	HWND h_dlg = FindWindowEx(h_main, NULL, _T("#32770"), _T(""));
	HWND h_ipCbEx32 = FindWindowEx(h_dlg, NULL, _T("ComboBoxEx32"), NULL);
	HWND h_ipCb = FindWindowEx(h_ipCbEx32, NULL, _T("ComboBox"), NULL);
	HWND h_ipEdit = FindWindowEx(h_ipCb, NULL, _T("Edit"), NULL);
	fill_text(h_ipEdit, ip);

	HWND h_labelUsername = FindWindowEx(h_dlg, NULL, _T("Static"), _T("用户名(&U):"));
	HWND h_editUsername = FindWindowEx(h_dlg, h_labelUsername, _T("Edit"), NULL);
	HWND h_labelPwd = FindWindowEx(h_dlg, NULL, _T("Static"), _T("密码(&P):"));
	HWND h_editPwd = FindWindowEx(h_dlg, h_labelPwd, _T("Edit"), NULL);
	fill_text(h_editUsername, username);
	fill_text(h_editPwd, password);

	HWND h_btnConnect = FindWindowEx(h_main, NULL, _T("Button"), MSTSC_CONNECT_BTN_TITLE);
	click_button(h_btnConnect);

	//查找根据窗口来判断连接是否成功
	HWND h_wnd;
	int probe_result = mstsc_probe_result(ip, MSTSC_AUTHENTICATION_RETRY, h_wnd);
	if (MSTSC_WARNING_WINDOW == probe_result)
	{
		//警告窗口
		HWND h_disable_button = FindWindowEx(h_wnd, NULL, TEXT("Button"), MSTSC_WARNING_DISABLE);
		click_button(h_disable_button);
		HWND h_yes_button = FindWindowEx(h_wnd, NULL, TEXT("Button"), MSTSC_YES_BUTTON);
		click_button(h_yes_button);
	}
	else if (MSTSC_ERROR == probe_result) {//error
		return false;
	}

	if (MSTSC_NORMAL_WINDOW != probe_result) {
		probe_result = mstsc_probe_result(ip, MSTSC_AUTHENTICATION_RETRY, h_wnd);
	}

	if (MSTSC_NORMAL_WINDOW != probe_result)
		return false;

	return true;
	//连接成功，输入密码，并登陆。键盘事件的模拟
	//都只有一个子窗口，所以找到的唯一一个就是所需的子窗口
	//HWND h_sub1 = FindWindowEx(h_wnd, NULL, _T("TscShellAxHostClass"), NULL);
	//HWND h_sub2 = FindWindowEx(h_sub1, NULL, NULL, NULL);
	//HWND h_sub3 = FindWindowEx(h_sub2, NULL, _T("UIMainClass"), NULL);
	//HWND h_sub4 = FindWindowEx(h_sub3, NULL, _T("UIContainerClass"), NULL);
	//HWND h_input = FindWindowEx(h_sub4, NULL, _T("IHWindowClass"), NULL);
	//Sleep(1000);
	//while (!IsWindowVisible(h_wnd));
	//::ShowWindow(h_wnd, SW_RESTORE);  
	//::SetForegroundWindow(h_wnd);
	//for (int i = 0; i < password.GetLength(); ++i) {
	//	TCHAR ch = password.GetAt(i);
	//	SHORT comb = VkKeyScan(ch);
	//	BYTE vk = comb & 0xFF;
	//	bool shift = (comb & 0x0100) == 0x0100 ? true : false;
	//	//char key_ch = ch;
	//	if (shift) {
	//		keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), 0, 0);
	//	}
	//	keybd_event(vk, MapVirtualKey(vk, MAPVK_VK_TO_VSC), 0, 0);
	//	keybd_event(vk, MapVirtualKey(vk, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
	//	if (shift) {
	//		keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
	//	}
	//}
	//keybd_event(VK_RETURN, MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC), 0, 0);
	//keybd_event(VK_RETURN, MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC), KEYEVENTF_KEYUP, 0);
	//return true;
}

bool mstsc_find_main_window( int seconds, HWND &h_main )
{
	bool found = false;
	HWND wnd = 0;
	mstsc_finding = true;
	if (!wait_thread())
	{
		return false;
	}
	start_timer(MSTSC_TIMER_ID, seconds, mstsc_timer_proc);
#ifdef _DEBUG
	_tprintf(TEXT("wait for the main window\n"));
#endif

	while (mstsc_finding)
	{
		if ((found = find_window(MSTSC_REMOTE_TITLE, 1, wnd)))
		{
#ifdef _DEBUG
			_tprintf(TEXT("main window found\n"));
#endif
			mstsc_finish_timer();
		}
	}
	h_main = wnd;
	return found;
}

int mstsc_probe_result( CString ip, int seconds, HWND &h_wnd )
{
	bool found = false;
	int result = MSTSC_ERROR;
	HWND wnd = 0;
	mstsc_finding = true;
	CString remote_title = ip + _T(" - ") + MSTSC_REMOTE_SUC_TITLE;
	if (!wait_thread())
	{
		return result;
	}
	start_timer(MSTSC_TIMER_ID, seconds, mstsc_timer_proc);
#ifdef _DEBUG
	_tprintf(TEXT("wait for the warning window or remote window\n"));
#endif

	while (mstsc_finding)
	{
		if (NULL != (wnd = FindWindow(TEXT("TSSHELLWND"), remote_title)))
		{
			found = true;
#ifdef _DEBUG
			_tprintf(TEXT("remote window found\n"));
#endif
			result = MSTSC_NORMAL_WINDOW;
			mstsc_finish_timer();
		}
		if (found == find_window(_T("中断远程桌面连接"), 1, wnd)) {
#ifdef _DEBUG
			_tprintf(TEXT("error window found\n"));
#endif
			result = MSTSC_ERROR;
			mstsc_finish_timer();
		}
		if (!found && (found = find_window(MSTSC_WARNING_TITLE, 1, wnd)))
		{
			HWND h_label = 0;
			if (NULL != (h_label = FindWindowEx(wnd, NULL, _T("Button"), _T("&Don't ask me again for connections to this computer")))) {
#ifdef _DEBUG
				_tprintf(TEXT("warning window found\n"));
#endif
				result = MSTSC_WARNING_WINDOW;
				mstsc_finish_timer();
			} else if (NULL != (h_label = FindWindowEx(wnd, NULL, _T("Static"),
				_T("Remote Desktop can’t connect to the remote computer for one of these reasons:\n")
				_T("\n")
				_T("1) Remote access to the server is not enabled\n")
				_T("2) The remote computer is turned off\n")
				_T("3) The remote computer is not available on the network\n")
				_T("\n")
				_T("Make sure the remote computer is turned on and connected to the network, and that remote access is enabled.")))) {
#ifdef _DEBUG
					_tprintf(TEXT("error window found\n"));
#endif
					result = MSTSC_ERROR;
					mstsc_finish_timer();
			}
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