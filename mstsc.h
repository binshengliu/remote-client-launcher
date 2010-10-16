#pragma once
#define MSTSC_NO 1
#define MSTSC_NO_CH TEXT('1')
#define MSTSC_TIMER_ID 101
#define MSTSC_WARNING_TITLE TEXT("Remote Desktop Connection")
#define MSTSC_REMOTE_TITLE TEXT(" - Remote Desktop Connection")
#define MSTSC_WARNING_DISABLE TEXT("&Don't ask me again for connections to this computer")
#define MSTSC_YES_BUTTON TEXT("&Yes")

#define MSTSC_AUTHENTICATION_RETRY 10
bool mstsc_construct_command(const CMapStringToString &parameters, CString &cmd);
bool mstsc_resolve_connect_info( const CMapStringToString &parameters, CString &ip);
bool mstsc_connect(CString cmd, CString ip);
//void mstsc_connect(CString ip, CString port, CString username, CString password, int seconds);
bool mstsc_probe_result(CString ip, int seconds, HWND &h_wnd);
VOID CALLBACK mstsc_timer_proc(HWND hwnd, UINT u_msg, UINT_PTR id_event, DWORD dw_time);
bool mstsc_finish_timer();
