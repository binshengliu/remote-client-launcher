#include "stdafx.h"
#include "common.h"
#include "radmin.h"
bool find_window( CString title, int retry, HWND &h_wnd )
{
	HWND wnd = 0;
	while (retry-- > 0 && !wnd)
		wnd = ::FindWindow(_T("#32770"), title);
	h_wnd = wnd;
	return wnd ? true : false;
}
TCHAR resolve_type(CString command)
{
	CString protocol = TEXT("alert://");
	int begin_pos = protocol.GetLength();
	return command[begin_pos];
}

void test()
{
}

//name&ip=219.216.101.53&port=&username=123456&password=123456
void process_parameters( CString str, int start, CMapStringToString &parameters )
{
	int pos = start;
	int end = start + str.GetLength();
	CString tmpName;
	CString tmpValue;
	while (pos < end)
	{
		boolean noEq=false;
		int valStart = -1;
		int valEnd = -1;
		int nameStart = pos;
		int nameEnd = str.Find('=', nameStart);

		// Workaround for a&b&c encoding
		int nameEnd2 = str.Find('&', nameStart);
		if( (nameEnd2!=-1 ) && ( nameEnd==-1 || nameEnd > nameEnd2) )
		{
			noEq = true;
			nameEnd = nameEnd2;
			valStart = nameEnd;
			valEnd = nameEnd;
		}
		if (nameEnd == -1)
		{
			nameEnd = end;
		}

		if (!noEq)
		{
			valStart= (nameEnd < end) ? nameEnd+1 : end;
			valEnd=str.Find('&', valStart);
			if( valEnd== -1 ) 
				valEnd = (valStart < end) ? end : valStart;
		}

		pos = valEnd + 1;
		tmpName = str.Mid( nameStart, nameEnd-nameStart );
		tmpValue = str.Mid( valStart, valEnd-valStart );
		parameters.SetAt(tmpName, tmpValue);
	}
}
bool create_process(CString cmd)
{
	STARTUPINFO startupinfo;
	ZeroMemory(&startupinfo,sizeof(STARTUPINFO));
	startupinfo.cb=sizeof(STARTUPINFO);
	PROCESS_INFORMATION process_info;
	bool suc = ::CreateProcess(NULL, cmd.GetBuffer(), NULL,NULL,TRUE,
		NORMAL_PRIORITY_CLASS ,NULL,NULL,&startupinfo,&process_info) ? true : false;
	cmd.ReleaseBuffer();
	if (!suc)
	{
#ifdef _DEBUG
		_tprintf(TEXT("fail to create process:%s"), cmd);
#endif
	}
	return suc;
}

bool click_button( HWND h_button )
{
	bool result0 = false;
	bool result1 = false;
	if (h_button)
	{
		if (!::PostMessage(h_button, WM_LBUTTONDOWN, 0, 0))
		{
#ifdef _DEBUG
			DWORD error = GetLastError();
			_tprintf(TEXT("Error when clicking the button. Error Code: %d\n"), error);
#endif
		}
		else
			result0 = true;
		if (!::PostMessage(h_button, WM_LBUTTONUP, 0, 0))
		{
#ifdef _DEBUG
			DWORD error = GetLastError();
			_tprintf(TEXT("Error when clicking the button. Error Code: %d\n"), error);
#endif
		}
		else
			result1 = true;
	}
	return result0 && result1;
}

bool fill_text( HWND h_edit, CString text )
{
	bool result = false;
	if (h_edit)
	{
		//PostMessage中不能传递指针
		if (!::SendMessage(h_edit, WM_SETTEXT, 0, (LPARAM)text.GetBuffer()))
		{
#ifdef _DEBUG
			DWORD error = GetLastError();
			_tprintf(TEXT("Error when filling text. Error Code: %d\n"), error);
#endif
		}
		else
			result = true;
		text.ReleaseBuffer();
	}
	return result;
}