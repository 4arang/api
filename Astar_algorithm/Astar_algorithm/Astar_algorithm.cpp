// Astar_algorithm.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "Astar_algorithm.h"

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


//>>>>>>>>>>>>>>>
#define n_cost 10 //normal cost
#define nd_cost 14 // normal digonal cost
#define GridWidth 50
#define NodeOver 9999999
void DrawGrid(HDC hdc, int x, int y, int r, int c, int w, int h);
void DrawGrid2(HDC hdc, int x, int y, int w);
int GetG(POINT S, POINT O); //S시작점 O대상점
int GetH(POINT D, POINT O); //D도착점 O대상점 
int GetF(int x, int y); //x G, y H
void a_star(POINT S, POINT D); //경로찾기
int map[50][30][3] = { 0 };
int res(int x, int y);
int meetGH = 0;
POINT Mid;
void GetCost(POINT S, POINT D);
void SetH(int nx, int ny, int ox, int oy);
int SetG(int nx, int ny, int ox, int oy);

//<<<<<<<<<<<<<<<<



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
    LoadStringW(hInstance, IDC_ASTARALGORITHM, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ASTARALGORITHM));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ASTARALGORITHM));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ASTARALGORITHM);
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
	HBRUSH hBrush, OldBrush;
	RECT wRect;
	GetClientRect(hWnd, &wRect);
	RECT Obstacle1 = { 500,500,1000,600 };


	static POINT Clicked = { -999,-999 };
	static int ClickedCheck = 1;
	static POINT SP = { -999, -999 }; //start point
	static POINT DP = { -999, -999 }; //end point
    switch (message)
    {
	case WM_CREATE:
	{
		
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


		DrawGrid2(hdc, wRect.right, wRect.bottom, GridWidth);

	///////////////// 장애물1 //////////////////
		hBrush = CreateSolidBrush(RGB(0, 0, 0));
		OldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		Rectangle(hdc, Obstacle1.left, Obstacle1.top, Obstacle1.right, Obstacle1.bottom);
		SelectObject(hdc, OldBrush);
		DeleteObject(hBrush);
	/////////////////////////////////////////////

		/////////////// Start point //////////////////
		hBrush = CreateSolidBrush(RGB(255, 234, 050));
		OldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		Rectangle(hdc, SP.x, SP.y, SP.x + GridWidth, SP.y + GridWidth);
		SelectObject(hdc, OldBrush);
		DeleteObject(hBrush);
		////////////////////////////////////////

		////////////// Destination Point ///////////////////////
		hBrush = CreateSolidBrush(RGB(128, 234, 050));
		OldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		Rectangle(hdc, DP.x, DP.y, DP.x + GridWidth, DP.y + GridWidth);
		SelectObject(hdc, OldBrush);
		DeleteObject(hBrush);
		////////////////////////////////////////////////////

		///////////PT////////
		hBrush = CreateSolidBrush(RGB(050, 234, 234));
		OldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		Rectangle(hdc, Mid.x*50, Mid.y*50, Mid.x*50 + GridWidth, Mid.y*50 + GridWidth);
		SelectObject(hdc, OldBrush);
		DeleteObject(hBrush);
		///////////////////////
		
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_LBUTTONDOWN:
	{

		POINT p = { LOWORD(lParam), HIWORD(lParam) };
		Clicked = { p.x - p.x%GridWidth, p.y - p.y%GridWidth };

		if (ClickedCheck == 1) //처음눌릴때 SP
		{
			SP = Clicked;
			DP = { -999, -999 };
			ClickedCheck = 0;
		}
		else //다음눌릴때 DP
		{
			DP = Clicked;
			ClickedCheck = 1;
			a_star(SP, DP);
		}

		InvalidateRect(hWnd, NULL, TRUE);
	
	}
	break;
    case WM_DESTROY:
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
	for (int i = 0; i < r + 1; i++)
	{
		MoveToEx(hdc, x - c_ * w, y - r_ * h + i * h, NULL);
		LineTo(hdc, x + c / 2 * w, y - r_ * h + i * h);
	}
	for (int i = 0; i < c + 1; i++)
	{
		MoveToEx(hdc, x - c_ * w + i * w, y - r_ * h, NULL);
		LineTo(hdc, x - c_ * w + i * w, y + r / 2 * h);
	}
}
void DrawGrid2(HDC hdc,int x, int y, int w)
{
	for (int i = 0; i < x; i++)
	{
		MoveToEx(hdc,i* w,0, NULL);
		LineTo(hdc, i*w, y);
	}
	for (int i = 0; i < y; i++)
	{
		MoveToEx(hdc, 0,i*w, NULL);
		LineTo(hdc, x, i*w);
	}
}
int GetG(POINT S, POINT O)
{
	if ((S.x == O.x || S.y == O.y)) //수평 or 수직이면 cost=10
		return n_cost;
	else //대각이면 cost=14
		return nd_cost; 
}

int GetH(POINT D, POINT O)
{
	if ((D.x == O.x || D.y == O.y)) //수평 or 수직이면 cost=10
		return n_cost;
	else //대각이면 cost=14
		return nd_cost;
}

int GetF(int x, int y)
{
	return x+y;
}

int res(int x, int y) //작거나 같으면 y 리턴
{
	return x >= y ? y : x;
}

void a_star(POINT S, POINT D)
{

	GetCost(S, D);
	for (int i = 1; i < 10; i++)
		{
			if (meetGH == 1) break;
			GetCost({ S.x - GridWidth * i ,S.y - GridWidth * i },
				{ D.x - GridWidth * i ,D.y - GridWidth * i });
			GetCost({ S.x ,S.y - GridWidth * i },
				{ D.x ,D.y - GridWidth * i });
			GetCost({ S.x + GridWidth * i ,S.y - GridWidth * i },
				{ D.x + GridWidth * i ,D.y - GridWidth * i });
			GetCost({ S.x - GridWidth * i ,S.y },
				{ D.x - GridWidth * i ,D.y  });
			GetCost({ S.x + GridWidth * i ,S.y },
				{ D.x + GridWidth * i ,D.y });
			GetCost({ S.x - GridWidth * i ,S.y + GridWidth * i },
				{ D.x - GridWidth * i ,D.y + GridWidth * i });
			GetCost({ S.x  ,S.y + GridWidth * i },
				{ D.x - GridWidth * i ,D.y + GridWidth * i });
			GetCost({ S.x + GridWidth * i ,S.y + GridWidth * i },
				{ D.x - GridWidth * i ,D.y + GridWidth * i });
		}
	

}
int SetG(int nx, int ny, int ox, int oy)
{
	if (nx >= 0 && ny >= 0) //map 최소좌표 (0,0)
	{
		if (map[nx][ny][2] != NodeOver)  //갈 수 있는지역이고
		{
			if(map[nx][ny][0]==0)//G값이 없다면
			map[nx][ny][0] = map[ox][oy][0] + GetG({ ox, oy }, { nx, ny }); //기존 G값받아서 더하기
			else //G값이 있었다면
			{
				if(map[nx][ny][0]> map[ox][oy][0] + GetG({ ox, oy }, { nx, ny }))//새로운값보다 클때만 집어넣기
					map[nx][ny][0] = map[ox][oy][0] + GetG({ ox, oy }, { nx, ny });
			}
		}
	}
	if (map[nx][ny][1] != NULL) //근데 H값이 있었다면
	{
		meetGH = 1;
		map[nx][ny][2] = GetF(map[nx][ny][0], map[nx][ny][1]);
		return map[nx][ny][2];
	}
	else
		return 0;
}
void SetH(int nx, int ny, int ox, int oy)
{
	if (nx >= 0 && ny >= 0) //map 최소좌표 (0,0)
	{
		if (map[nx][ny][2] != NodeOver)  //갈 수 있는지역이고
		{
			if (map[nx][ny][1] == 0)//H값이 없다면
				map[nx][ny][1] = map[ox][oy][1] + GetH({ ox, oy }, { nx, ny }); //기존 H값받아서 더하기
			else //H값이 있었다면
			{
				if (map[nx][ny][1] > map[ox][oy][1] + GetH({ ox, oy }, { nx, ny }))//새로운값보다 클때만 집어넣기
					map[nx][ny][1] = map[ox][oy][1] + GetH({ ox, oy }, { nx, ny });
			}
		}
	}
}

void GetCost(POINT S, POINT D)
{

	static int cost;
	int temp;
	int s_x, s_y, d_x, d_y; //S좌표, D좌표를 map배열로 변환
	s_x = (S.x) / 50;			// 1 2 3
	s_y = (S.y) / 50;			// 4 o 5
	d_x = (D.x) / 50;			// 6 7 8
	d_y = (D.y) / 50;

	/////>> G cost for 8way 구하면서 더 작은f값 찾게되면 그 좌표 반환
	cost = SetG(s_x - 1, s_y - 1, s_x, s_y);
	if (cost != 0) Mid = { s_x - 1 ,s_y - 1 };
	temp = cost;
	cost = SetG(s_x, s_y - 1, s_x, s_y);
	if (cost!=0 && cost<temp) Mid = {s_x, s_y - 1 };
	temp = cost;
	cost = SetG(s_x + 1, s_y - 1, s_x, s_y);
	if (cost != 0 && cost < temp) Mid = {s_x + 1, s_y - 1 };
	temp = cost;
	cost = SetG(s_x - 1, s_y, s_x, s_y);
	if (cost != 0 && cost < temp) Mid = {s_x - 1, s_y };
	temp = cost;
	cost = SetG(s_x + 1, s_y, s_x, s_y);
	if (cost != 0 && cost < temp) Mid = {s_x + 1, s_y };
	temp = cost;
	cost = SetG(s_x - 1, s_y + 1, s_x, s_y);
	if (cost != 0 && cost < temp) Mid = {s_x - 1, s_y + 1 };
	temp = cost;
	cost = SetG(s_x, s_y + 1, s_x, s_y);
	if (cost != 0 && cost < temp) Mid = {s_x, s_y + 1 };
	temp = cost;
	cost = SetG(s_x + 1, s_y + 1, s_x, s_y);
	if (cost != 0 && cost < temp) Mid = {s_x + 1, s_y + 1 };

	/////>> H cost for 8way
	SetH(d_x - 1, d_y - 1, d_x, d_y);
	SetH(d_x, d_y - 1, d_x, d_y);
	SetH(d_x + 1, d_y - 1,d_x, d_y);
	SetH(d_x - 1, d_y, d_x, d_y);
	SetH(d_x + 1, d_y, d_x, d_y);
	SetH(d_x - 1, d_y + 1, d_x, d_y);
	SetH(d_x, d_y + 1, d_x, d_y);
	SetH(d_x + 1, d_y + 1, d_x, d_y);

}