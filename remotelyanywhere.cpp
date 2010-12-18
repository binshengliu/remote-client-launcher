#include "stdafx.h"
#include "remotelyanywhere.h"
#include "common.h"
#include "timer.h"
#include <atlbase.h>
#include <mshtml.h>
#include <oleacc.h>
#pragma comment ( lib, "oleacc" )
bool rmtaw_finding;
HWND find_ie_server(int seconds)
{
	if (!wait_thread())
		return 0;
	start_timer(RMTAW_TIMER_ID, seconds, rmtaw_timer_proc);
	HWND h_ie_server = 0;
	rmtaw_finding = true;
	while (rmtaw_finding)
	{
		EnumWindows(EnumWindowsProc, (LPARAM)&h_ie_server);
		if (h_ie_server) {
			rmtaw_finish_timer();
		}
	}
	return h_ie_server;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	TCHAR	buf[100];

	::GetClassName( hwnd, (LPTSTR)&buf, 100 );
	if ( _tcscmp( buf, _T("IEFrame") ) == 0 )
	{
		HWND h_ie = hwnd;
		HWND h_ie_server = NULL;
		bool found = false;
		HWND h_prev_tab = NULL;
		while (!found) {
			HWND h_tab = FindWindowEx(h_ie, h_prev_tab, _T("Frame Tab"), NULL);
			if (!h_tab) {
				break;
			}
			h_prev_tab = h_tab;
			HWND h_tab_wnd = FindWindowEx(h_tab, NULL, _T("TabWindowClass"), _T("RemotelyAnywhere - Windows Internet Explorer"));
			if (!h_tab_wnd) {
				continue;
			}
			HWND h_shell = FindWindowEx(h_tab_wnd, NULL, _T("Shell DocObject View"), NULL);
			h_ie_server = FindWindowEx(h_shell, NULL, _T("Internet Explorer_Server"), NULL);
		}
		*(HWND*)lParam = h_ie_server;
		return FALSE;
	}
	else
		return TRUE;
};

bool rmtaw_connect( CString url, CString username, CString password, int seconds )
{
	HWND h_ie = FindWindow(_T("IEFrame"), NULL);

	ShellExecute(NULL, _T("open"), _T("iexplore"), url, NULL, SW_SHOWMAXIMIZED); 
#ifdef _DEBUG
	_tprintf(_T("wait for ie\n"));
#endif

	HWND h_ie_server = find_ie_server(seconds);
	if (!h_ie_server) {
#ifdef _DEBUG
		_tprintf(_T("find ie server failed\n"));
#endif
		return false;
	}
	while (!IsWindowVisible(h_ie_server));

	HRESULT hr;
	hr = CoInitialize(NULL);
	if (FAILED(hr)) {
#ifdef _DEBUG
		_tprintf(_T("CoInitialize failed\n"));
#endif
		return false;
	}

	UINT nMsg = ::RegisterWindowMessage( _T("WM_HTML_GETOBJECT") );
	LRESULT lRes;
	::SendMessageTimeout( h_ie_server, nMsg, 0L, 0L, SMTO_ABORTIFHUNG, 3000, (DWORD*) &lRes );

	CComPtr < IHTMLDocument3 > spDoc;
	hr = ::ObjectFromLresult ( lRes, IID_IHTMLDocument3, 0 , (LPVOID *) &spDoc );
	if (FAILED(hr) || !spDoc) {
#ifdef _DEBUG
		_tprintf(_T("get IHTMLDocument3 failed\n"));
#endif
		return false;
	}

	//Sleep(2000);
	CComPtr <IHTMLElement>   pUsr;
	pUsr = get_element_by_id(spDoc, RMTAW_USR, RMTAW_FIND_ELEM_TIMEOUT);
	if (!pUsr) {
#ifdef _DEBUG
		_tprintf(_T("getElementById failed\n"));
#endif
		return false;
	}
	hr = pUsr->put_innerText(CComBSTR(username));
	if (FAILED(hr)) {
#ifdef _DEBUG
		_tprintf(_T("put_innerText failed\n"));
#endif
		return false;
	}

	CComPtr <IHTMLElement>   pPwd;
	hr = spDoc->getElementById(CComBSTR(RMTAW_PWD), &pPwd);
	if (FAILED(hr) || !pPwd) {
#ifdef _DEBUG
		_tprintf(_T("getElementById failed\n"));
#endif
		return false;
	}
	pPwd->put_innerText(CComBSTR(password));
	if (FAILED(hr)) {
#ifdef _DEBUG
		_tprintf(_T("put_innerText failed\n"));
#endif
		return false;
	}

	CComPtr <IHTMLElement>   pLogin;
	hr = spDoc->getElementById(CComBSTR(RMTAW_LOGIN), &pLogin);
	if (FAILED(hr) ||  !pLogin) {
#ifdef _DEBUG
		_tprintf(_T("getElementById failed"));
#endif
		return false;
	}
	pLogin->click();
	if (FAILED(hr)) {
#ifdef _DEBUG
		_tprintf(_T("click failed"));
#endif
		return false;
	}
	return true;
}

bool rmtaw_construct_url( const CMapStringToString &parameters, CString &url )
{
	CString ip;
	url.Empty();
	url = _T("http://");
	if (parameters.Lookup(IP_STRING, ip))
		url += ip;
	else
		return false;
	return true;
}

bool rmtaw_resolve_connect_info( const CMapStringToString &parameters, CString &username, CString &password )
{
	if (!parameters.Lookup(PASSWORD_STRING, password))
		return false;
	if (!parameters.Lookup(USERNAME_STRING, username))
		return false;
	return true;
}
VOID CALLBACK rmtaw_timer_proc( HWND hwnd, UINT u_msg, UINT_PTR id_event, DWORD dw_time )
{
#ifdef _DEBUG
	_tprintf(TEXT("timer timeout; ID:%d\n"), id_event);
#endif
	rmtaw_finish_timer();
}

bool rmtaw_finish_timer()
{
	rmtaw_finding = false;
	return finish_timer(thread_id);
}

CComPtr<IHTMLElement> get_element_by_id( CComPtr < IHTMLDocument3 > spDoc, CString id, int timeout)
{
	CComPtr<IHTMLElement> p_elem;
	if (!wait_thread())
		return 0;
	start_timer(RMTAW_TIMER_ID, timeout, rmtaw_timer_proc);
	rmtaw_finding = true;
	while (rmtaw_finding)
	{
		HRESULT hr = spDoc->getElementById(CComBSTR(id), &p_elem);
		if (SUCCEEDED(hr) && p_elem) {
			rmtaw_finish_timer();
		}
	}

	return p_elem;
}
