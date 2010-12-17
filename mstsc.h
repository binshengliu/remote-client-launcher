#pragma once
#define MSTSC_NO 1
#define MSTSC_NO_CH TEXT('1')
#define MSTSC_TIMER_ID 101
#define MSTSC_WARNING_TITLE TEXT("远程桌面连接")
#define MSTSC_REMOTE_TITLE TEXT("远程桌面连接")
#define MSTSC_REMOTE_SUC_TITLE TEXT("远程桌面")
#define MSTSC_CONNECT_BTN_TITLE TEXT("连接(&N)")
#define MSTSC_WARNING_DISABLE TEXT("&Don't ask me again for connections to this computer")
#define MSTSC_YES_BUTTON TEXT("&Yes")

#define MSTSC_NORMAL_WINDOW 0
#define MSTSC_WARNING_WINDOW 1
#define MSTSC_ERROR 2

#define MSTSC_CONNECT_RETRY 10

#define MSTSC_TIMER_ID 101

#define MSTSC_AUTHENTICATION_RETRY 10
bool mstsc_construct_command(const CMapStringToString &parameters, CString &cmd);
bool mstsc_resolve_connect_info( const CMapStringToString &parameters, CString &ip, CString &username, CString &password );
bool mstsc_connect( CString cmd, CString ip, CString username, CString password, int seconds);
bool mstsc_find_main_window(int seconds, HWND &h_main);
//void mstsc_connect(CString ip, CString port, CString username, CString password, int seconds);
int mstsc_probe_result(CString ip, int seconds, HWND &h_wnd);
VOID CALLBACK mstsc_timer_proc(HWND hwnd, UINT u_msg, UINT_PTR id_event, DWORD dw_time);
bool mstsc_finish_timer();
