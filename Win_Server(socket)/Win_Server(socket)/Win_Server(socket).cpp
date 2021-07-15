// Win_Server(socket).cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "Win_Server(socket).h"
#include <stdio.h>
//>>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")


#include <list>
#include <vector>

using namespace std;
//<<
#define MAX_LOADSTRING 100

#define WM_ASYNC WM_USER+1

WSADATA wsadata;
SOCKET s, cs;
SOCKADDR_IN addr = { 0 }, c_addr;
int size, msgLen;
char buffer[100];
TCHAR messages[256];
TCHAR mymsg[256];
// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK CONTROL_Proc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);

list <SOCKET> socketList;


int InitServer(HWND hWnd);
int CloseServer();
SOCKET AcceptSocket(HWND hWnd, SOCKET s, SOCKADDR_IN& c_addr);
void SendMessageToClient(char* buffer);
void ReadMessage(TCHAR* msg, char* buffer);
void CloseClient(SOCKET s);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	//return WinServer();
	
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINSERVERSOCKET, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINSERVERSOCKET));

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

	return (int)msg.wParam;
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

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINSERVERSOCKET));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINSERVERSOCKET);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
		case IDC_BUTTON_SERVER_START:
			return InitServer(hDlg);
			break;
		case IDC_BUTTON_SEND:
			GetDlgItemText(hDlg, IDC_EDIT_INPUT, mymsg, 100);
			SetDlgItemText(hDlg, IDC_EDIT_OUTPUT, mymsg);
			break;
		case IDCLOSE:
			EndDialog(hDlg, 0);
			DestroyWindow(hDlg);
			hDlg = NULL;
			return 0;
		}
		break;
	case WM_ASYNC:
		switch (lParam)
		{
		case FD_ACCEPT:
			cs = AcceptSocket(hDlg, s, c_addr);
			break;
		case FD_READ:
			ReadMessage(messages, buffer);

			if (_tcscmp(messages, _T("")))
			{
				SetDlgItemText(hDlg, IDC_EDIT_OUTPUT, messages);
			}
			break;
		case FD_CLOSE:
			CloseClient(wParam);
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
	static TCHAR msg[200];

	switch (message)
	{
	case WM_CREATE :

		//return InitServer(hWnd);
		break;
	case WM_ASYNC :
		switch (lParam)
		{
		case FD_ACCEPT:
			cs = AcceptSocket(hWnd, s, c_addr);
			break;
		case FD_READ:
			ReadMessage(messages, buffer);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case FD_CLOSE:
			CloseClient(wParam);
			break;
		}
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
		case ID_CHAT :
			DialogBox(hInst, MAKEINTRESOURCE(IDD_SERVER_DIALOG), hWnd, CONTROL_Proc);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
		if(_tcscmp(messages, _T(""))) TextOut(hdc, 0, 30, messages, (int)_tcslen(messages));

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		CloseServer();
		closesocket(s);
		WSACleanup();
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

int InitServer(HWND hWnd)
{
	WSAStartup(MAKEWORD(2, 2 ), &wsadata);
	s = socket(AF_INET, SOCK_STREAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_port = 20;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	bind(s, (LPSOCKADDR)&addr, sizeof(addr));
	WSAAsyncSelect(s, hWnd, WM_ASYNC, FD_ACCEPT);
	if (listen(s, 5) == -1)return 0;
	return 1;
}

int CloseServer()
{

	closesocket(s);
	WSACleanup();
	return 0;
}

SOCKET AcceptSocket(HWND hWnd, SOCKET s, SOCKADDR_IN & c_addr)
{
	SOCKET cs;
	int size;
	size = sizeof(c_addr);
	cs = accept(s, (LPSOCKADDR)&c_addr, &size);
	WSAAsyncSelect(cs, hWnd, WM_ASYNC, FD_READ|FD_CLOSE);

	socketList.push_back(cs);

	return cs;
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

void CloseClient(SOCKET socket)
{
	for (list<SOCKET>::iterator it = socketList.begin();
		it != socketList.end(); it++)
	{
		SOCKET cs = (*it);
		if (cs == socket)
		{
			closesocket(cs);
			it = socketList.erase(it);
			break;
		}
	}
}

int WinServer()
{
	//WSADATA wsadata;
	//SOCKET s;
	//SOCKADDR_IN addr = { 0 };
	//TCHAR message[256];


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

	if (bind(s, (LPSOCKADDR)&addr, sizeof(addr)))
	{
		MessageBox(NULL, _T("binding failed"), _T("Error"), MB_OK);
		return 0;
	}
	else
	{
		MessageBox(NULL, _T("binding Success"), _T("Success"), MB_OK);
	}
	if (listen(s, 5) == -1)
	{
		MessageBox(NULL, _T("listen failed"), _T("Error"), MB_OK);
		return 0;
	}
	else
	{
		MessageBox(NULL, _T("listen Success"), _T("Success"), MB_OK);
	}

	do
	{
		SOCKADDR_IN c_addr;
		int size = sizeof(c_addr);
		SOCKET cs = accept(s, (LPSOCKADDR)&c_addr, &size);

		char buffer[100];
		int msgLen;
		msgLen = recv(cs, buffer, 100, 0);
		buffer[msgLen] = NULL; //문자열의 끝
#ifdef _UNICODE
		TCHAR wBuffer[100];
		msgLen = MultiByteToWideChar(CP_ACP, 0, buffer, strlen(buffer), NULL, NULL);//얼마만큼 변환
		MultiByteToWideChar(CP_ACP, 0, buffer, strlen(buffer), wBuffer, msgLen); //실제변환
		wBuffer[msgLen] = NULL;

		//출력용문자열
		_stprintf_s(messages, _T("클라이언트 메시지: %s, 서버 종료할ㄲ?"), wBuffer);
#else
		sprintf_s(messages, _T("클라이언트 메시지: %s, 서버 종료핡?"), buffer);
#endif
	} while (MessageBox(NULL, _T("클라이언트 접속 확인. 서버 종료할꺼?"),
		_T("Server Message"), MB_YESNO) == IDNO);

	closesocket(s);
		WSACleanup();
		return 1;
}
