#pragma once
#define PREFIX_STRING TEXT("wbr://")
#define TYPE_STRING TEXT("type")
#define PROGNAME_STRING TEXT("progname")
#define IP_STRING TEXT("ip")
#define PORT_STRING TEXT("port")
#define USERNAME_STRING TEXT("username")
#define PASSWORD_STRING TEXT("password")
#define ID_STRING TEXT("id")
#define CODE_STRING TEXT("code")
#define ASSISTANT_MODE_STRING TEXT("assistant_mode")
#define ASSISTANT_TYPE_STRING TEXT("assistant_type")

bool find_window(CString title, int retry, HWND &h_wnd);
TCHAR resolve_type(CString command);
void test();
void process_parameters(CString str, int start, CMapStringToString &parameters);
bool create_process(CString cmd);
bool fill_text(HWND h_edit, CString text);
bool click_button(HWND h_button);
extern DWORD thread_id;
