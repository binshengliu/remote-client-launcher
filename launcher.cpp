// launcher.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "launcher.h"
#include "radmin.h"
#include "teamviewer.h"
#include "ttvnc.h"
#include "mstsc.h"
#include "remotelyanywhere.h"
#include "common.h"
#include "DESDecryption.h"
#include <iostream>
#include <string.h>
#include <tchar.h>
using namespace std;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define KEY "oaycjmf3"

// The one and only application object

CWinApp theApp;
void str2byte( DWORD _dwLen, LPSTR _pStr, LPBYTE _pByte);

CMapStringToString parameters;
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
#ifdef _DEBUG
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
#endif
		nRetCode = 1;
	}
	else
	{
		if (argc != 2)
		{
			nRetCode = 1;
			return nRetCode;
		}
#ifdef _DEBUG
		_tprintf(_T("%s\n"), argv[1]);
#endif
		int begin_pos = CString(PREFIX_STRING).GetLength();
		TCHAR *t = &argv[1][begin_pos];
		DWORD len = _tcslen(&argv[1][begin_pos]);
		DWORD bytes = len * sizeof(TCHAR);

		char *tc = 0;
#ifdef UNICODE
		bytes /= 2;
		tc = new char[bytes + 1];
		wcstombs_s(0, tc, bytes + 1, t, bytes * 2);
#endif
		DWORD content_len = bytes / 2;
		BYTE *buf = new BYTE[content_len];
		str2byte(bytes, tc, buf);
		const BYTE key[] = KEY;
		if (des_decrypt(key, buf, &content_len))
			buf[content_len] = '\0';
		else
			return 1;
#ifdef DEBUG
		printf("%s\n", buf);
#endif
		CString ip, port, username, password;
		CString id, teamviewer_assistant_type;
		CString code, ttvnc_assistant_mode;
		CString cmd;
		CString rmtaw_url;
		
		CString url(buf);
		delete [] buf;
		delete [] tc;
		if (url[url.GetLength() - 1] == '/')
		{
			url = url.Left(url.GetLength() - 1);
		}

		process_parameters(url, 0, parameters);

		CString str_time;
		if (parameters.Lookup(TIME_STRING, str_time)) {
			time_t tim = _tstol(str_time);
			if (!is_time_valid(tim)) {
				AfxMessageBox(_T("链接已失效"));
				return -1;
			}
		} else {
			AfxMessageBox(_T("无法验证时间"));
			return -1;
		}

		CString str_type;
		TCHAR type = 0;
		if (parameters.Lookup(TYPE_STRING, str_type))
			type = str_type[0];

		switch (type)
		{
		case RADMIN_NO_CH:
			radmin_construct_command(parameters, cmd);
			radmin_resolve_login_info(parameters, ip, username, password);
			radmin_login(cmd, ip, username, password, RADMIN_LOGIN_RETRY);
			break;
		case MSTSC_NO_CH:
			mstsc_construct_command(parameters, cmd);
			mstsc_resolve_connect_info(parameters, ip, username, password);
			mstsc_connect(cmd, ip, username, password, MSTSC_CONNECT_RETRY);
			break;
		case TTVNC_NO_CH:
			ttvnc_construct_command(parameters, cmd);
			ttvnc_resolve_assist_info(parameters, code, ttvnc_assistant_mode);
			ttvnc_assist(cmd, code, ttvnc_assistant_mode, TTVNC_LOGIN_RETRY);
			break;
		case TEAMVIEWER_NO_CH:
			teamviewer_construct_command(parameters, cmd);
			teamviewer_resolve_connect_info(parameters, id, password, teamviewer_assistant_type);
			teamviewer_connect(cmd, id, password, TEAMVIEWER_CONNECT_RETRY);
			break;
		case RMTAW_NO_CH:
			rmtaw_construct_url(parameters, rmtaw_url);
			rmtaw_resolve_connect_info(parameters, username, password);
			rmtaw_connect(rmtaw_url, username, password, RMTAW_CONNECT_RETRY);
			break;
		}
#ifdef _DEBUG
		getchar();
#endif
	}
	return nRetCode;
}
