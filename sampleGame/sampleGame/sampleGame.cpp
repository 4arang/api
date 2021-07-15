// sampleGame.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "sampleGame.h"
#include <list>

using namespace std;

#define MAX_LOADSTRING 100

typedef struct _tagRectangle
{
	float l, t, r, b;
}RECTANGLE, *PRECTANGLE;


//플레이어 총알
list<RECTANGLE> g_PlayerBulletList;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.


//시간을 구하기 위한 변수들
LARGE_INTEGER g_tSecond;
LARGE_INTEGER g_tTime;
float g_fDeltaTime;



//for peek message//
HWND g_hWnd;
bool g_bLoop = TRUE;
RECTANGLE g_tPlayerRC = { 100, 100, 200, 200 };


HDC g_hDC;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void Run();

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
    LoadStringW(hInstance, IDC_SAMPLEGAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }
	
	//화면용 dc 생성
	g_hDC = GetDC(g_hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SAMPLEGAME));

    MSG msg;

	QueryPerformanceFrequency(&g_tSecond);
	QueryPerformanceCounter(&g_tTime);
	
    // 기본 메시지 루프입니다: 게임은 peekmessage에 만들어야 window deadtime 이용
	//메시지 없어도 바로바져나온다?
    while (g_bLoop)
    {
		//Peek은 메시지가 메세지큐에 없어도 바로 빠져나온다
		 // 메세지가 있읅셩우 true 없으 경우 fals 메시지 없는시간이 ㅁ윈도우의 데드타임이다.
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

		//윈도우 데드타임일 경우
		else
		{
			Run();
		}
    }

	ReleaseDC(g_hWnd, g_hDC);

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SAMPLEGAME));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = NULL;// MAKEINTRESOURCEW(IDC_SAMPLEGAME);
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

   g_hWnd = hWnd;
   //실제 윈도우 타이틀바나 메뉴를 포함한 윈도우의 크기를 구해준다.
   RECT rc = { 0,0,800,600 };
   AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

   //위에서 구해준 크기로 윈도우 클라이언트 영역의 크기를 원하는 크기로 맞춰줘야 한다.
   SetWindowPos(hWnd, HWND_TOPMOST, 400, 400, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE |
	   SWP_NOZORDER);
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
    switch (message)
    {
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
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
			TextOut(hdc, 100, 100, TEXT("win32ㅁㄴㅇ"), 8);
			TextOut(hdc, 100, 120, L"win32ㅁㄴㅇ", 8);
			
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		g_bLoop = FALSE;
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



void Run()
{
	RECT rcWindow;
	GetWindowRect(g_hWnd, &rcWindow);

	//DeltaTime 구하기
	LARGE_INTEGER tTime;
	QueryPerformanceCounter(&tTime);
	g_fDeltaTime = (tTime.QuadPart - g_tTime.QuadPart) /
		(float)g_tSecond.QuadPart;
	g_tTime = tTime;


	static float fTimeSacle = 1.f; //만가지고 전체를 control 슬로우 빠른재생 멈추기


	//플레이어 초당 이동속도 : 300
	float fSpeed = 300 * g_fDeltaTime;


	if (GetAsyncKeyState('D') & 0x8000)
	{
		g_tPlayerRC.l += fSpeed;
		g_tPlayerRC.r += fSpeed;
	}	
	if (GetAsyncKeyState('A') & 0x8000)
	{
		g_tPlayerRC.l -= fSpeed;
		g_tPlayerRC.r -= fSpeed;
	}
	if (GetAsyncKeyState('W') & 0x8000)
	{
		g_tPlayerRC.t -= fSpeed;
		g_tPlayerRC.b -= fSpeed;
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		g_tPlayerRC.t += fSpeed;
		g_tPlayerRC.b += fSpeed;
	}
	if (g_tPlayerRC.l < 0)
	{
		g_tPlayerRC.l = 0;
		g_tPlayerRC.r = 100;
	}
	if (g_tPlayerRC.t < 0)
	{
		g_tPlayerRC.t = 0;
		g_tPlayerRC.b = 100;
	}
	if (g_tPlayerRC.r > 800)
	{
		g_tPlayerRC.l = 700;
		g_tPlayerRC.r = 800;
	}
	if (g_tPlayerRC.b > 600)
	{
		g_tPlayerRC.t = 500;
		g_tPlayerRC.b = 600;
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		RECTANGLE rcBullet;
		rcBullet.l = g_tPlayerRC.r;
		rcBullet.r = g_tPlayerRC.r + 50.f;
		rcBullet.t = (g_tPlayerRC.t + g_tPlayerRC.b) / 2.f - 25.f;
		rcBullet.b = rcBullet.t + 50.f;
	
		list<RECTANGLE>::iterator iter;
		list<RECTANGLE>::iterator iterEnd = g_PlayerBulletList.end();

		for (iter = g_PlayerBulletList.begin(); iter != iterEnd; ++iter)
		{
			Rectangle(g_hDC, (*iter).l, (*iter).t, (*iter).r, (*iter).b);
		}
	}
	Rectangle(g_hDC, g_tPlayerRC.l, g_tPlayerRC.t, g_tPlayerRC.r, g_tPlayerRC.b);
}