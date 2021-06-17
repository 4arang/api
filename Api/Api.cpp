// Api.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "Api.h"
#include <TCHAR.H>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>
#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void DrawGrid(HDC hdc, int x, int y, int r, int c, int w, int h);
void DrawCircle(HDC hdc, int x, int y, int r);
void DrawRectangle(HDC hdc, int x, int y, int w, int h);
void SunFlower(HDC hdc, int x, int y, int r, int num);
POINT myRotate(POINT Opoint, POINT Hpoint, double theta); //회전행렬


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
    LoadStringW(hInstance, IDC_API, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_API));
	
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_API));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_API);
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
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	static bool bKeyDown = false;
	static TCHAR str[100];
	static int count = 0;
	static int yPos=0;
	static SIZE size;
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
	case WM_CREATE :
	{
		CreateCaret(hWnd, NULL, 5, 15);
		ShowCaret(hWnd);
	}
		break;
	case WM_CHAR :
	{
		
			//bKeyDown = true;
			if (wParam == VK_BACK)
			{
				if (count > 0)
					str[--count] = NULL;
				else
					str[count] = NULL;
			}
			//else if (wParam == VK_RETURN)
			//{

			//	yPos += 20;
			//}
			else
			{
				if (count < 99)
				{
					str[count++] = wParam;
					str[count] = NULL;
				}
			}
			InvalidateRect(hWnd, NULL, true);
		}
	
	break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
			//HDC hdc2 = GetDC(hWnd);
			//ReleaseDC(hWnd, hdc2);
			TextOut(hdc, 100, 100, _T("hello 안녕"), _tcslen(_T("hello 안녕")));
			RECT rect;
			rect.left = 200;
			rect.top = 100;
			rect.right = 300;
			rect.bottom = 200;
			SetTextColor(hdc, RGB(205, 122, 140));
			DrawText(hdc, _T("hello 안녕"), _tcslen(_T("hello 안녕")), &rect, DT_SINGLELINE
			|DT_CENTER |DT_VCENTER);
			TextOut(hdc, 400, 400, _T("hello 안녕"), _tcslen(_T("hello 안녕")));
			
			//if(bKeyDown)
		//	TextOut(hdc, 0, yPos,str, _tcslen(str));
			RECT rc = { 0, 0, 400, 400 };
			DrawText(hdc, str, _tcslen(str), &rc, DT_TOP | DT_LEFT);
           
			GetTextExtentPoint(hdc, str, _tcslen(str), &size);
			TextOut(hdc, 0, 0, str, _tcslen(str));
			SetCaretPos(size.cx, 0);
			/*
		격자 그리기 함수 DrawGrid() 구현
		시작위치, 끝위치LeftTop, RightBottom .. RECTm POINT
		DrawGrid(시작위치, 끝위치, 간격)
		DrawGrid(중심위치, 간격, 선의 개수)
		*/
			POINT Gridpt = {800, 500};
			TextOut(hdc, Gridpt.x, Gridpt.y, _T("*"), _tcslen(_T("*")));
			DrawGrid(hdc, Gridpt.x, Gridpt.y, 14, 12, 20, 10);


			/*
			DrawCircle이라는 함수 중심값, 반지름 받으면 만드는거
			*/
			POINT Circlept = { 300, 300 };
			int CircleR = 100;
			DrawCircle(hdc, Circlept.x, Circlept.y, CircleR);
			TextOut(hdc, Circlept.x, Circlept.y, _T("*"), _tcslen(_T("*")));
			SunFlower(hdc, Circlept.x, Circlept.y, CircleR, 99);


			POINT Rectanglept = { 500, 200 };
			int RectangleWidth = 200;
			int RectangleHeight = 50;
			DrawRectangle(hdc, Rectanglept.x, Rectanglept.y, RectangleWidth, RectangleHeight);
			TextOut(hdc, Rectanglept.x, Rectanglept.y, _T("*"), _tcslen(_T("*")));
	
			EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY: //메모리 해제, 파일 종료 시키는 곳
        PostQuitMessage(0);
		HideCaret(hWnd);
		DestroyCaret();
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
void DrawGrid(HDC hdc, int x, int y, int r, int c, int w, int h)//중심점, 행, 열, 셀너비, 셀높이
{

	int c_ = c % 2 == 1 ? c / 2 + 1 : c / 2;
	int r_ = r % 2 == 1 ? r / 2 + 1 : r / 2;
	for (int i = 0; i < r+1; i++)
	{
		MoveToEx(hdc, x-c_*w,y-r_*h+i*h,NULL);
		LineTo(hdc, x + c/2 * w, y - r_ * h+i*h);
	}
	for (int i = 0; i < c + 1; i++)
	{
		MoveToEx(hdc, x - c_ * w+i*w, y - r_ * h, NULL);
		LineTo(hdc, x - c_ * w+i*w, y + r/2 * h);
	}
}
void DrawCircle(HDC hdc, int x, int y, int r)
{
	Ellipse(hdc, x - r, y - r, x + r, y + r);
}
void DrawRectangle(HDC hdc, int x, int y, int w, int h)
{
	
	int w_ = w % 2 == 1 ? w / 2 + 1 : w / 2;
	int h_ = h % 2 == 1 ? h / 2 + 1 : h / 2;
	Rectangle(hdc, x - w_, y - h_, x + w_, y + h_);
}
void SunFlower(HDC hdc, int x, int y, int r, int num)
{
	double angle = (double)360 / num;
	double theta = angle *  M_PI/ 180;


	double smallR = r * cos((M_PI - theta) / 2) / (1 - cos((M_PI - theta) / 2)); //제1코사인법칙 유도
	POINT Circlept = { x, y + r+(int)smallR};
	DrawCircle(hdc, Circlept.x, Circlept.y, (int)smallR);

	for (int i = 0; i < num - 1; i++)
	{
		Circlept = myRotate({ Circlept.x, Circlept.y }, { x, y }, theta);
		//TextOut(hdc, Circlept.x, Circlept.y, _T("*"), _tcslen(_T("*")));
		DrawCircle(hdc, Circlept.x, Circlept.y, (int)smallR);
	}
}
POINT myRotate(POINT Opoint, POINT Hpoint, double theta) //회전행렬
{
	POINT rpoint;
	double x = cos(theta);
	rpoint.x = cos(theta)*(Opoint.x-Hpoint.x) - sin(theta)*(Opoint.y-Hpoint.y)+Hpoint.x;
	rpoint.y = sin(theta)*(Opoint.x - Hpoint.x) + cos(theta)*(Opoint.y - Hpoint.y)+Hpoint.y;
	return rpoint;
}