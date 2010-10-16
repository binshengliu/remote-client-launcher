#pragma once
#define TTVNC_NO 2
#define TTVNC_NO_CH TEXT('2')
#define TTVNC_TITLE_COUNT 2
#define TTVNC_TITLE0 TEXT("TTVNC 远程协助")
#define TTVNC_TITLE1 TEXT("TTVNC 远程协助 ")
#define TTVNC_BUTTON_ASSIST TEXT("协助")
#define TTVNC_BUTTON_ASSISTED TEXT("被协助")

#define TTVNC_ASSISTMODE_ASSIST_CHAR TEXT('0')
#define TTVNC_ASSISTANTMODE_ASSISTED_CHAR TEXT('1')

#define TTVNC_LOGIN_RETRY 10
#define TTVNC_TIMER_ID 102

bool ttvnc_construct_command( const CMapStringToString &parameters, CString &cmd);
bool ttvnc_resolve_assist_info( const CMapStringToString &parameters, CString &code, CString &assistant_mode);
bool ttvnc_find_window(int seconds, HWND &h_wnd);
bool ttvnc_assist(CString cmd, CString code, CString assist_type, int seconds);
VOID CALLBACK ttvnc_timer_proc(HWND hwnd, UINT u_msg, UINT_PTR id_event, DWORD dw_time);
bool ttvnc_finish_timer();
