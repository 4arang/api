// Win_Client(Socket).cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "Win_Client(Socket).h"
#include <stdio.h>
//>>
#include <list>
#include <vector>

using namespace std;
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

//<<
#define WM_ASYNC WM_USER+1
list <SOCKET> socketList;
#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
void ReadMessage(TCHAR * msg, char * buffer);
WSADATA wsadata;
SOCKET s;
SOCKADDR_IN addr = { 0 };
TCHAR msg[200], str[100], mymsg[200];
int msgLen;
char buffer[100];
int cnt=0;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK CONTROL_Proc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
int initClient(HWND hWnd);
void SendMessageToClient(char* buffer);
void CloseClient();
int Win_Client_init();
int Win_Client();
void 	SendMessageToServer();
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
// return Win_Client();
    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINCLIENTSOCKET, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINCLIENTSOCKET));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINCLIENTSOCKET));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINCLIENTSOCKET);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
BOOL CALLBACK CONTROL_Proc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static int selection2;

	switch (iMsg)
	{
	case WM_INITDIALOG:
		//hList = GetDlgItem(hDlg, IDC)
		return 1;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_CLIENT_START :
			return initClient(hDlg);
			break;
		case ID_BUTTON_SEND:
			GetDlgItemText(hDlg, IDC_EDIT_INPUT, mymsg, 100);
			SetDlgItemText(hDlg, IDC_EDIT_OUTPUT, mymsg);
			break;
		case IDCLOSE:
			EndDialog(hDlg, 0);
			DestroyWindow(hDlg);
			hDlg = NULL;
			return 0;
		
		break;
		case WM_ASYNC:
		{
			switch (lParam)
			{
			case FD_READ:
				msgLen = recv(s, buffer, 100, 0);
				buffer[msgLen] = NULL;
#ifdef _UNICODE
				msgLen = MultiByteToWideChar(CP_ACP, 0, buffer, strlen(buffer), NULL, NULL);
				MultiByteToWideChar(CP_ACP, 0, buffer, strlen(buffer), msg, msgLen);
				msg[msgLen] = NULL;



#else
				strcpy(msg, buffer);
#endif

				ReadMessage(msg, buffer);

				if (_tcscmp(msg, _T("")))
				{
					SetDlgItemText(hDlg, IDC_EDIT_OUTPUT, msg);
				}
				break;
			}
		}
		break;
		case WM_CHAR:
			if (wParam == VK_RETURN)
			{
				if (s == INVALID_SOCKET) return 0;
#ifdef _UNICODE
				msgLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
				WideCharToMultiByte(CP_ACP, 0, str, -1, buffer, msgLen, NULL, NULL);
#else
				strcpy(bufferm str);
				msgLen = strlen(buffer);
#endif
				send(s, (LPSTR)buffer, msgLen + 1, 0);
				cnt = 0;
				return 0;
			}
			str[cnt++] = wParam;
			str[cnt] = NULL;
			InvalidateRgn(hDlg, NULL, true);

		case FD_CLOSE:
			CloseClient();
			break;
		}
		break;
	default:
		break;
	}
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{


    switch (message)
    {
	case WM_CREATE:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_CLIENT_DIALOG), hWnd, CONTROL_Proc);
		//return initClient(hWnd);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_ASYNC:
	{
		switch (lParam)
		{
		case FD_READ:
			msgLen = recv(s, buffer, 100, 0);
			buffer[msgLen] = NULL;
#ifdef _UNICODE
			msgLen = MultiByteToWideChar(CP_ACP, 0, buffer, strlen(buffer), NULL, NULL);
			MultiByteToWideChar(CP_ACP, 0, buffer, strlen(buffer), msg, msgLen);
			msg[msgLen] = NULL;

			

#else
			strcpy(msg, buffer);
#endif

			InvalidateRgn(hWnd, NULL, TRUE);
		}
	}
	break;
	case WM_CHAR:
		if (wParam == VK_RETURN)
		{
			if (s == INVALID_SOCKET) return 0;
#ifdef _UNICODE
			msgLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_ACP, 0, str, -1, buffer, msgLen, NULL, NULL);
#else
			strcpy(bufferm str);
			msgLen = strlen(buffer);
#endif
			send(s, (LPSTR)buffer, msgLen + 1, 0);
			cnt = 0;
			return 0;
		}
		str[cnt++] = wParam;
		str[cnt] = NULL;
		InvalidateRgn(hWnd, NULL, true);
			break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
			TextOut(hdc, 0, 0, str, _tcslen(str));
			if(_tcscmp(msg, _T(""))) TextOut(hdc, 0, 30, msg, (int)_tcslen(msg));

            EndPaint(hWnd, &ps);
        }
        break;
	case WM_KEYDOWN:
	//	SendMessageToServer();
		break;
    case WM_DESTROY:
		CloseClient();

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

int Win_Client()
{
	//WSADATA wsadata;
	//SOCKET s;
	//SOCKADDR_IN addr = { 0 };

	WSAStartup(MAKEWORD(2, 2), &wsadata);
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		MessageBox(NULL, _T("socket failed"), _T("Error"), MB_OK);
		return 0;
	}
	else
	{
		MessageBox(NULL, _T("socket success"), _T("SuccesS"), MB_OK);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = 20;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //loczl

	
	if (connect(s, (LPSOCKADDR)&addr, sizeof(addr))==SOCKET_ERROR)
	{
		MessageBox(NULL, _T("connect failed"), _T("Error"), MB_OK);
		return 0;
	}
	else
	{
		MessageBox(NULL, _T("connect success"), _T("SuccesS"), MB_OK);
	}

	send(s, "Hello Server", 12,0);

	closesocket(s);
	WSACleanup();
	return 1;

	return 0;
}

int initClient(HWND hWnd)
{
	//WSADATA wsadata;
//SOCKET s;
//SOCKADDR_IN addr = { 0 };

	WSAStartup(MAKEWORD(2, 2), &wsadata);
	s = socket(AF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = 20;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //loczl

	WSAAsyncSelect(s, hWnd, WM_ASYNC, FD_READ); //read발생하면 async로 보내주세여


	if (connect(s, (LPSOCKADDR)&addr, sizeof(addr)) == -1)
		return 0;


	return 1;
}
void CloseClient()
{

	closesocket(s);
	WSACleanup();
}
int Win_Client_init()
{
	//WSADATA wsadata;
	//SOCKET s;
	//SOCKADDR_IN addr = { 0 };

	WSAStartup(MAKEWORD(2, 2), &wsadata);
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		MessageBox(NULL, _T("socket failed"), _T("Error"), MB_OK);
		return 0;
	}
	else
	{
		MessageBox(NULL, _T("socket success"), _T("SuccesS"), MB_OK);
	}

	addr.sin_family = AF_INET;
	addr.sin_port = 20;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //loczl


	if (connect(s, (LPSOCKADDR)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		MessageBox(NULL, _T("connect failed"), _T("Error"), MB_OK);
		return 0;
	}
	else
	{
		MessageBox(NULL, _T("connect success"), _T("SuccesS"), MB_OK);
	}

	return 0;
}
void 	SendMessageToServer()
{
	send(s, "Hello Server", 12, 0);
}
void ReadMessage(TCHAR * msg, char * buffer)
{
	for (list<SOCKET>::iterator it = socketList.begin();
		it != socketList.end(); it++)
	{
		SOCKET cs = (*it);
		int msgLen = recv(cs, buffer, 100, 0);
		if (msgLen > 0)
		{
			buffer[msgLen] = NULL;

#ifdef _UNICODE
			msgLen = MultiByteToWideChar(CP_ACP, 0, buffer, strlen(buffer), NULL, NULL);
			MultiByteToWideChar(CP_ACP, 0, buffer, strlen(buffer), msg, msgLen);
			msg[msgLen] = NULL;
#else
			strcpy(msg, buffer);
#endif _UNICODE

			SendMessageToClient(buffer);
		}
	}
}
void SendMessageToClient(char * buffer)
{
	for (list<SOCKET>::iterator it = socketList.begin();
		it != socketList.end(); it++)
	{
		SOCKET cs = (*it);
		send(cs, (LPSTR)buffer, strlen(buffer) + 1, 0);
	}
}