#pragma once
#include "launcher.h"
#define RADMIN_NO 0
#define RADMIN_NO_CH TEXT('0')
#define RADMIN_TITLE_COUNT_ZH 2
#define RADMIN_TITLE_ZH0 TEXT("Radmin 安全性: ")
#define RADMIN_TITLE_ZH1 TEXT("Radmin 安全性：")
#define RADMIN_TITLE_COUNT_EN 2
#define RADMIN_TITLE_EN0 TEXT("Radmin security: ")
#define RADMIN_TITLE_EN1 TEXT("Radmin security:")

#define RADMIN_BUTTON_OK_ZN TEXT("确定")
#define RADMIN_BUTTON_OK_EN TEXT("OK")

#define RADMIN_LOGIN_RETRY 10

#define RADMIN_TIMER_ID 100

extern bool radmin_finding;

bool radmin_construct_command( const CMapStringToString &parameters, CString &cmd);
bool radmin_resolve_login_info( const CMapStringToString &parameters, CString &ip, CString &username, CString &password);
bool radmin_find_window(CString ip, int seconds, HWND &h_wnd, bool &zh);
bool radmin_login(CString cmd, CString ip, CString username, CString password, int seconds);
VOID CALLBACK radmin_timer_proc(HWND hwnd, UINT u_msg, UINT_PTR id_event, DWORD dw_time);
bool radmin_finish_timer();
