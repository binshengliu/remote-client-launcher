#pragma once
#define TEAMVIEWER_NO 3
#define TEAMVIEWER_NO_CH TEXT('3')
#define	TEAMVIEWER_TITLE TEXT("TeamViewer")
#define TEAMVIEWER_PASSWORD_TITLE TEXT("TeamViewer 认证")
#define TEAMVIEWER_ERROR_TITLE TEXT("错误")
#define TEAMVIEWER_INVALID_ID_MESSAGE TEXT("您输入的 ID 无效.请检查您伙伴的 ID.")
#define TEAMVIEWER_ERROR_BUTTON_OK TEXT("确认")
#define TEAMVIEWER_BUTTON_CONNECT TEXT("连接至伙伴")
#define TEAMVIEWER_BUTTON_LOGIN TEXT("登录")
#define TEAMVIEWER_CONNECT_RETRY 10
#define TEAMVIEWER_AUTHENTICATION_RETRY 60
#define TEAMVIEWER_INITIALIZATION_RETRY 10

#define TEAMVIEWER_TIMER_ID 103

bool teamviewer_construct_command( const CMapStringToString &parameters, CString &cmd);
bool teamviewer_resolve_connect_info( const CMapStringToString &parameters, CString &id, CString &password, CString &assistant_type);
bool teamviewer_find_main_window(int seconds, HWND &h_wnd);
bool teamviewer_wait_initialization(HWND h_wnd, int seconds);
bool teamviewer_connect(CString cmd, CString id, CString password, int seconds);
bool teamviewer_probe_result(int seconds, HWND &h_wnd);
bool teamviewer_authenticate(HWND h_password, CString password);
VOID CALLBACK teamviewer_timer_proc(HWND hwnd, UINT u_msg, UINT_PTR id_event, DWORD dw_time);
bool teamviewer_finish_timer();
