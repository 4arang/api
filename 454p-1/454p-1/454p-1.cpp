// 454p-1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "454p-1.h"
#include <process.h> 
#include <Windows.h>
#include <time.h>

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
int flag = 1;
int j = -1; //circle 개수
CRITICAL_SECTION cs;
HWND hThread;
RECT rectView;
// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void DrawCircle(HDC hdc, int x, int y, int r);
unsigned __stdcall ThFunc3(LPVOID lpParam);
unsigned __stdcall ThFunc4(LPVOID lpParam);



class CCircle
{
	POINT pt = { 0,0 };
	int R=30;
	int ColorR = 0;
	int ColorG = 0;
	int ColorB = 0;
public:
	void update();

	int GetColorR() const { return ColorR; }
	int GetColorG() const { return ColorG; }
	int GetColorB() const { return ColorB; }
	POINT GetPOINT() const { return pt; }
	int GetR() const { return R; }

	void SetPOINT(POINT p) { pt = p; }
	void SetColors(int R, int G, int B) { ColorR = R, ColorG = G, ColorB = B; }
};
CCircle *circles[100];

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY454P1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY454P1));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY454P1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MY454P1);
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
   hThread = hWnd;
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	DWORD dwThID1, dwThID2;
	HANDLE hThreads[2];
	unsigned long ulStackSize = 0; //인자 필요해서 넣은거 의미 x

    switch (message)
    {
	case WM_CREATE:
	{
		GetClientRect(hWnd, &rectView);
		InitializeCriticalSection(&cs); //cs 쓸거야
		dwThID1 = 0;
		dwThID2 = 0;
		hThreads[0] = NULL;
		hThreads[1] = NULL;

		hThreads[0] = (HANDLE)_beginthreadex(NULL, 0, (unsigned(__stdcall *)(void*))ThFunc3,
			NULL, 0, (unsigned*)&dwThID1);
		hThreads[1] = (HANDLE)_beginthreadex(NULL, 0, (unsigned(__stdcall *)(void*))ThFunc4,
			NULL, 0, (unsigned*)&dwThID2);
		SetTimer(hWnd, 1, 100, NULL);
		

	}
		break;
	case WM_TIMER:
	{
		switch (wParam)
		{
		case 1:
			InvalidateRect(hWnd, NULL, false);
			break;
		default:
			break;
		}
	}
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

	case WM_LBUTTONDOWN:
	{
		j++;
		//	hdc = GetDC(hWnd);
		if (hThreads[0] == 0 || hThreads[1] == 0)
			exit(1);

		if (j >= 0)
		{
			circles[j] = new CCircle();
			circles[j]->SetPOINT({ rand() % rectView.right, rand() % rectView.bottom });
			circles[j]->SetColors(rand() % 256, rand() % 256, rand() % 256);

			ResumeThread(hThreads[1]);

			WaitForMultipleObjects(2, hThreads, TRUE, INFINITE); //thread 정상종료될때까지 기다리는거
		}
		
	}
	break;
	case WM_RBUTTONDOWN:
	{
		flag = 0;
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
		HBRUSH hBrush, OldBrush;




		if (j >= 0)
		{
			for (int i = 0; i <= j; i++)
			{
				int R = circles[i]->GetColorR();
				int G = circles[i]->GetColorG();
				int B = circles[i]->GetColorB();
				hBrush = CreateSolidBrush(RGB(R, G, B));
				OldBrush = (HBRUSH)SelectObject(hdc, hBrush);
				int X = circles[i]->GetPOINT().x;
				int Y = circles[i]->GetPOINT().y;
				DrawCircle(hdc, X, Y, circles[i]->GetR());
				SelectObject(hdc, OldBrush);
				DeleteObject(hBrush);
			}
		}




		EndPaint(hWnd, &ps);
	}
	break;
    case WM_DESTROY:
		for (int i = 0; i <= j; i++)
		{
			delete circles[i];
		}

	
		//CloseHandle(hThreads[1]);


		DeleteCriticalSection(&cs);
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
void DrawCircle(HDC hdc, int x, int y, int r)
{
	Ellipse(hdc, x - r, y - r, x + r, y + r);
}

unsigned __stdcall ThFunc3(LPVOID lpParam) //원 생성
{
	int i = 1;
	while (flag)
	{
		EnterCriticalSection(&cs);
		while (i--)
		{
			if (j >= 0)
			{
				circles[j]->SetPOINT({ rand() % rectView.right, rand() % rectView.bottom });
			}
		}
		LeaveCriticalSection(&cs);
	}
	return 0; 
}
unsigned __stdcall ThFunc4(LPVOID lpParam) //색 변경
{

	while (flag)
	{
		EnterCriticalSection(&cs);
		for (int i = 0; i < j; i++)
		{
			circles[i]->SetColors(rand() % 256, rand() % 256, rand() % 256);
		}
		Sleep(1000);
		LeaveCriticalSection(&cs);
	
	}

	return 0;
}