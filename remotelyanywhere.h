#pragma once
#include <atlbase.h>
#include <mshtml.h>
#include <oleacc.h>
#define RMTAW_NO 4
#define RMTAW_NO_CH _T('4')
#define RMTAW_TIMER_ID 104
#define RMTAW_CONNECT_RETRY 10
#define RMTAW_FIND_ELEM_TIMEOUT 10
#define RMTAW_USR _T("usr")
#define RMTAW_PWD _T("pwd")
#define RMTAW_LOGIN _T("login")

CComPtr<IHTMLElement> get_element_by_id(CComPtr < IHTMLDocument3 > spDoc, CString id, int timeout);
HWND find_ie_server(int seconds);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);		
bool rmtaw_construct_url( const CMapStringToString &parameters, CString &url);
bool rmtaw_resolve_connect_info( const CMapStringToString &parameters, CString &username, CString &password);
bool rmtaw_connect(CString url, CString username, CString password, int seconds);
VOID CALLBACK rmtaw_timer_proc(HWND hwnd, UINT u_msg, UINT_PTR id_event, DWORD dw_time);
bool rmtaw_finish_timer();
