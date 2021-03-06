// Api.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include "resource.h"
#include "framework.h"
#include "Api.h"
#include <TCHAR.H>
#include <algorithm>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>
#define MAX_LOADSTRING 100
#include <list>
#include <commdlg.h>



// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
struct Stars
{
	POINT star;
	int angle;
	int r;
};


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
void DrawRectText(HDC hdc, RECT rc, LPWSTR str);
void DrawStar(HDC hdc, POINT star, int r);
void DrawRotatedStar(HDC hdc, POINT star, int r, double angle);
Stars RandomlyRotatedStars(HDC hdc, int howmanyStars, POINT leftTop, POINT rightBottom, int maxSize, int minSize, Stars *stars);
void DrawRandomlyRotatedStars(HDC hdc, int  howmanyStars, Stars  *stars);

float GetDegree(float deg);
float GetDegree(float a, float b);

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

class CShape
{
protected:
	POINT PT;
	POINT MovingPT;
	int Speed = 24;
	float Dir;
	int R = 45;
public:
	//	CShape();
	//	~CShape();
	POINT GetPOINT() const { return PT; }
	void SetPOINT(POINT p) { PT = p; }
	POINT GetMPT() const { return MovingPT; }
	void setMPT(POINT p) { MovingPT = p; }
	int GetSpeed() const { return Speed; }
	float GetDir() const { return Dir; }
	void SetDir() { Dir = (rand() % 360) * M_PI / 180; }
	void SetDir(float Dir_) { Dir = Dir_; }
	int GetR() const { return R; }
	void ChangeDir() { Dir = -Dir; }
	bool CollisionCircle(CShape *other);

	virtual	void update() {}
	virtual bool CollisionWall(RECT wRect) = 0;
	virtual void DrawMe(HDC hdc) = 0;
};

class CCircle :public CShape
{
	//CCircle() : R(R) {}
public:
	CCircle() : CShape() {}
	void DrawMe(HDC hdc)
	{
		DrawCircle(hdc, GetPOINT().x, GetPOINT().y, GetR());
	}
	void update() { PT.x += cos(Dir) * Speed; PT.y +=sin(Dir)* Speed; }
	bool CollisionWall(RECT wRect) { if (PT.x + R >= wRect.right || PT.x - R <= wRect.left
		|| PT.y + R >= wRect.bottom || PT.y - R <= wRect.top) return true; }
	//bool CollisionObject()
};

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

void OutFromFile(TCHAR filename[], HWND hWnd)
{
	FILE *fPtr;
	HDC hdc;
	int line;
	TCHAR buffer[500];
	line = 0;
	hdc = GetDC(hWnd);
#ifdef _UNICODE
	_tfopen_s(&fPtr, filename, _T("r, ccs = UNICODE"));
#else
	_tfopen_s(&fPtr, filename, _T("r"));
#endif
	while (_fgetts(buffer, 100, fPtr) != NULL)
	{
		if (buffer[_tcslen(buffer) - 1] == _T('\n'))
			buffer[_tcslen(buffer) - 1] = NULL;
		TextOut(hdc, 0, line * 20, buffer, _tcslen(buffer));
		line++;
	}
	fclose(fPtr);
	ReleaseDC(hWnd, hdc);
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//typedef struct {
	//	DWORD lStructSize;
	//	HWND hwndOwner;
	//	HWND hInstance;
	//	COLORREF rgbResult;
	//	COLORREF *lpCustColors;
	//	DWORD Flags;
	//	LPARAM lCustData;
	//	LPCCHOOKPROC lpfnHook;
	//	LPCTSTR lpTemplateName;
	//}CHOOSECOLOR, *LPCHOOSECOLOR;

	OPENFILENAME OFN;
	TCHAR str2[100], lpstrFile[100] = _T("");
	TCHAR filter[] = _T("Every File(*.*)\0 * .*\0Text File\0 * .txt;*.doc\0");
	HDC hdc;
	CHOOSECOLOR COLOR;
	static COLORREF tmp[16], color;
	HBRUSH hBrush, OldBrush;
	int ii;

//	static bool bKeyDown = false;
	HPEN hPen, oldPen;
	HBRUSH hBrush1,oldBrush;

	static TCHAR str[100];
	static int count = 0;
	static int yPos=0;
	static SIZE size;
	int howmanyStars = 95;
	static struct Stars *stars = new Stars[howmanyStars];
	static POINT ptCircle;
	static POINT ptCircleTimer;
	static POINT ptCircleClick;
	int moveptCircleSpeed = 40;
	static BOOL bSelected = false;
	static POINT ptCirclemMove;
	
	static int j = -1;

	static	CShape *circles[50];

	




    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
			case ID_FILEOPEN:
				memset(&OFN, 0, sizeof(OPENFILENAME));
				OFN.lStructSize = sizeof(OPENFILENAME);
				OFN.hwndOwner = hWnd;
				OFN.lpstrFilter = filter;
				OFN.lpstrFile = lpstrFile;
				OFN.nMaxFile = 100;
				OFN.lpstrInitialDir = _T(".");
				if (GetOpenFileName(&OFN) != 0)
				{
					_stprintf_s(str2, _T("%s 파일을 열겠습니까?"), OFN.lpstrFile);
					MessageBox(hWnd, str2, _T("열기 선택"), MB_OK);
					OutFromFile(OFN.lpstrFile, hWnd);
				}
				break;
			case ID_COLORDLG:
				for (ii = 0; ii < 16; ii++)
					tmp[ii] = RGB(rand() % 256, rand() % 256, rand() % 256);
				memset(&COLOR, 0, sizeof(CHOOSECOLOR));
				COLOR.lStructSize = sizeof(CHOOSECOLOR);
				COLOR.hwndOwner = hWnd;
				COLOR.lpCustColors = tmp;
				COLOR.Flags = CC_FULLOPEN;
				if (ChooseColor(&COLOR) != 0)
				{

					color = COLOR.rgbResult;
					InvalidateRgn(hWnd, NULL, TRUE);
				}

            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case ID_FUNC1:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case ID_FUNC2:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case ID_FUNC3:
			{
				int ans = MessageBox(hWnd, _T("기능 선택 확인버튼"), _T("기능선택"), MB_YESNOCANCEL);
				switch (ans)
				{
				case IDYES:
					MessageBox(hWnd, _T("YES버튼 선택"), _T("버튼확인"), MB_OK);
					break;
				case IDNO:
					MessageBox(hWnd, _T("NO 버튼 선택"), _T("버튼확인"), MB_OK);
					break;
				case IDCANCEL:
					MessageBox(hWnd, _T("CANCEL 버튼 선택"), _T("버튼확인"), MB_OK);
					break;
				default:
					break;
				}
			}
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }

        }
        break;

	case WM_CREATE :
	{
		CreateCaret(hWnd, NULL, 5, 15);
		ShowCaret(hWnd);

		//SetTimer(hWnd, 1, 100, NULL); //타이머설정 아이디, 간격ms, 함수
		//SetTimer(hWnd, 2, 100, NULL);
		SetTimer(hWnd, 3, 50, NULL);
	}
		break;
	case WM_SIZE : // wParam, IParam(HIWORD: H, LOWORD:W)창 사이즈 변경값 입력시
	{
		int breakpoint = 999; 
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
	case WM_KEYDOWN:
	{
		hdc = GetDC(hWnd);

		if (wParam == VK_RIGHT)
		{
			hBrush = CreateSolidBrush(RGB(255, 100, 100));
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
			DrawRectangle(hdc, 1250, 350, 100, 200);
			SelectObject(hdc, oldBrush);
			DeleteObject(hBrush);

			ptCircle.x += moveptCircleSpeed;

		}
		else if (wParam == VK_LEFT)
		{
			hBrush = CreateSolidBrush(RGB(255, 100, 100));
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
			DrawRectangle(hdc, 1050, 350, 100, 200);
			SelectObject(hdc, oldBrush);
			DeleteObject(hBrush);

			ptCircle.x -= moveptCircleSpeed;
	
		}
		else if (wParam == VK_UP)
		{
			hBrush = CreateSolidBrush(RGB(255, 100, 100));
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
			DrawRectangle(hdc, 1150, 150, 100, 200);
			SelectObject(hdc, oldBrush);
			DeleteObject(hBrush);

			ptCircle.y -= moveptCircleSpeed;

		}
		else if(wParam == VK_DOWN)
		{
			hBrush = CreateSolidBrush(RGB(255, 100, 100));
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
			DrawRectangle(hdc, 1150, 550, 100, 200);
			SelectObject(hdc, oldBrush);
			DeleteObject(hBrush);

			ptCircle.y += moveptCircleSpeed;
		
		}
		//else if(wParam == )
		InvalidateRect(hWnd, NULL, TRUE);
		ReleaseDC(hWnd, hdc);
	}
	
	break;
	case WM_KEYUP:
	{
		hdc = GetDC(hWnd);
		if (wParam == VK_RIGHT)
		{
			DrawRectangle(hdc, 1250, 350, 100, 200);
			TextOut(hdc, 1250, 350, _T("오른쪽"), _tcslen(_T("오른쪽")));
		}
		else if (wParam == VK_LEFT)
		{
			DrawRectangle(hdc, 1050, 350, 100, 200);
			TextOut(hdc, 1050, 350, _T("왼쪽"), _tcslen(_T("왼쪽")));
		}
		else if (wParam == VK_UP)
		{
			DrawRectangle(hdc, 1150, 150, 100, 200);
			TextOut(hdc, 1150, 150, _T("위쪽"), _tcslen(_T("위쪽")));
		}
		else if (wParam == VK_DOWN)
		{
			DrawRectangle(hdc, 1150, 550, 100, 200);
			TextOut(hdc, 1150, 550, _T("아래쪽"), _tcslen(_T("아래쪽")));
		}
		ReleaseDC(hWnd, hdc);
	}
	break;
	case WM_TIMER :
		switch(wParam)
	{
		case 1:
		{

		}
		break;
		case 2:
		{
			ptCircleTimer.x += 40;
			ptCircleTimer.y += 40;
		}
		default:
			break;
		case 3:
		{
			if (j >= 0) {
				for (int i = 0; i <= j; i++)
				{
					//circles[j]->SetPOINT({ circles[j]->GetPOINT().x + circles[j]->GetMPT().x,
					//circles[j]->GetPOINT().y + circles[j]->GetMPT().y });
					circles[i]->update();
				}
			}
		}
		break;
	}
		InvalidateRect(hWnd, NULL, true);
		break;

	case WM_LBUTTONDOWN:
	{
		j++;
	//	hdc = GetDC(hWnd);
		POINT p;
		if (j >= 0)
		{
			
			circles[j] = new CCircle();
			circles[j]->SetDir();
			p.x = cos(circles[j]->GetDir()) * circles[j]->GetSpeed();
			p.y = sin(circles[j]->GetDir()) * circles[j]->GetSpeed();

			circles[j]->SetPOINT({ LOWORD(lParam), HIWORD(lParam) });

			circles[j]->setMPT(p);

		}
		int breakpoint = 999;

	//	bSelected = TRUE;
	}
	InvalidateRect(hWnd, NULL, true);
	//ReleaseDC(hWnd, hdc);
	break;
	case WM_LBUTTONUP:
	{
		//bSelected = FALSE;
	}
	break;
	case WM_RBUTTONDOWN:
	{
	}
	break;
	case WM_RBUTTONUP:
	{

	}
	break;
	case WM_MOUSEMOVE :
	{
		//if (bSelected)
		//{
		//	ptCirclemMove.x = LOWORD(lParam);
		//	ptCirclemMove.y = HIWORD(lParam);
		//	InvalidateRect(hWnd, NULL, true);
		//}
	}
		break;
		
    case WM_PAINT:
	
	{
		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
		//HDC hdc2 = GetDC(hWnd);
		//ReleaseDC(hWnd, hdc2);
	//	TextOut(hdc, 100, 100, _T("hello 안녕"), _tcslen(_T("hello 안녕")));

		//RECT rect;
		//rect.left = 200;
		//rect.top = 100;
		//rect.right = 300;
		//rect.bottom = 200;


	//	DrawText(hdc, _T("hello 안녕"), _tcslen(_T("hello 안녕")), &rect, DT_SINGLELINE
	//	|DT_CENTER |DT_VCENTER);
		//TextOut(hdc, 400, 400, _T("hello 안녕"), _tcslen(_T("hello 안녕")));

		//if(bKeyDown)
	//	TextOut(hdc, 0, yPos,str, _tcslen(str));

		//RECT rc = { 0, 0, 400, 400 };
		//DrawText(hdc, str, _tcslen(str), &rc, DT_TOP | DT_LEFT);

		//GetTextExtentPoint(hdc, str, _tcslen(str), &size);
	//	TextOut(hdc, 0, 0, str, _tcslen(str));
	//SetCaretPos(size.cx, 0);
		/*
	격자 그리기 함수 DrawGrid() 구현
	시작위치, 끝위치LeftTop, RightBottom .. RECTm POINT
	DrawGrid(시작위치, 끝위치, 간격)
	DrawGrid(중심위치, 간격, 선의 개수)
	*/
		RECT wRect;
		GetClientRect(hWnd, &wRect);

		POINT Gridpt = { 700, 400 };
		DrawGrid(hdc, Gridpt.x, Gridpt.y, 10, 20, 100, 100);


		hPen = CreatePen(PS_SOLID, 1, RGB(255, 234, 050));
		oldPen = (HPEN)SelectObject(hdc, hPen);
		hBrush = CreateSolidBrush(RGB(255, 234, 050));
		oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		RandomlyRotatedStars(hdc, howmanyStars, { 0,0 }, { 1700,900 }, 28, 5, stars);
		DrawRandomlyRotatedStars(hdc, howmanyStars, stars);
		SelectObject(hdc, oldBrush);
		DeleteObject(hBrush);
		SelectObject(hdc, oldPen);
		DeleteObject(hPen);


		//hPen = CreatePen(PS_SOLID, 1, RGB(102, 153, 062));
		//oldPen = (HPEN)SelectObject(hdc, hPen);
		//hBrush = CreateSolidBrush(RGB(102, 153, 062));
		//oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		//POINT Rectanglept_ground = { 850, 800 };
		//int RectangleWidth_ground = 1700;
		//int RectangleHeight_ground = 300;
		//DrawRectangle(hdc, Rectanglept_ground.x, Rectanglept_ground.y, RectangleWidth_ground, RectangleHeight_ground);
		//SelectObject(hdc, oldBrush);
		//DeleteObject(hBrush);
		//SelectObject(hdc, oldPen);
		//DeleteObject(hPen);


		//hPen = CreatePen(PS_SOLID, 1, RGB(051, 204, 051));
		//oldPen = (HPEN)SelectObject(hdc, hPen);
		//hBrush = CreateSolidBrush(RGB(051, 204, 051));
		//oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		//POINT Rectanglept = { 300, 530 };
		//int RectangleWidth = 40;
		//int RectangleHeight = 280;
		//DrawRectangle(hdc, Rectanglept.x, Rectanglept.y, RectangleWidth, RectangleHeight); SelectObject(hdc, oldBrush);
		//SelectObject(hdc, oldBrush);
		//DeleteObject(hBrush);
		//SelectObject(hdc, oldPen);
		//DeleteObject(hPen);



		////TextOut(hdc, Rectanglept.x, Rectanglept.y, _T("*"), _tcslen(_T("*")));
		///*
		//DrawCircle이라는 함수 중심값, 반지름 받으면 만드는거
		//*/
		//POINT Circlept = { 300, 300 };
		//int CircleR = 100;
		////DrawCircle(hdc, Circlept.x, Circlept.y, CircleR);
		////TextOut(hdc, Circlept.x, Circlept.y, _T("*"), _tcslen(_T("*")));


		//hPen = CreatePen(PS_SOLID, 1, RGB(255, 204, 000));
		//oldPen = (HPEN)SelectObject(hdc, hPen);
		//hBrush = CreateSolidBrush(RGB(255, 204, 051));
		//oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		//SunFlower(hdc, Circlept.x, Circlept.y, CircleR, 14);
		//SelectObject(hdc, oldBrush);
		//DeleteObject(hBrush);
		//SelectObject(hdc, oldPen);
		//DeleteObject(hPen);


		//hPen = CreatePen(PS_SOLID, 1, RGB(102, 051, 000));
		//oldPen = (HPEN)SelectObject(hdc, hPen);
		//hBrush = CreateSolidBrush(RGB(153, 102, 000));
		//oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		//DrawCircle(hdc, Circlept.x, Circlept.y, CircleR); DeleteObject(hBrush);
		//SelectObject(hdc, oldBrush);
		//DeleteObject(hBrush);
		//SelectObject(hdc, oldPen);
		//DeleteObject(hPen);

		//POINT Starpt = { 1000, 500 };
		//int StarR = 200;
		//DrawStar(hdc, Starpt, StarR);

		//DrawRotatedStars(hdc, 45, { 0,0 }, { 1500,130 }, 65, 5);

		//DrawRectangle(hdc, 1250, 350, 100, 200);
		//DrawRectangle(hdc, 1050, 350, 100, 200);
		//DrawRectangle(hdc, 1150, 150, 100, 200);
		//DrawRectangle(hdc, 1150, 550, 100, 200);
		//TextOut(hdc, 1250, 350, _T("오른쪽"), _tcslen(_T("오른쪽")));
		//TextOut(hdc, 1150, 150, _T("위쪽"), _tcslen(_T("위쪽")));
		//TextOut(hdc, 1050, 350, _T("왼쪽"), _tcslen(_T("왼쪽")));
		//TextOut(hdc, 1150, 550, _T("아래쪽"), _tcslen(_T("아래쪽")));


		//SetTextColor(hdc, RGB(205, 122, 140));
		//RECT rc_text;
		//rc_text.left = 220;
		//rc_text.top = 280;
		//rc_text.right = 380;
		//rc_text.bottom = 320;
		//DrawRectText(hdc, rc_text, str);

		//POINT CircleMove{500, 500};
		//int CircleMoveR = 100;
		//	if (ptCircle.x + CircleMove.x + CircleMoveR > wRect.right )ptCircle.x -= moveptCircleSpeed;
		//	if( ptCircle.x + CircleMove.x - CircleMoveR < wRect.left)ptCircle.x += moveptCircleSpeed;
		//	if (ptCircle.y + CircleMove.y + CircleMoveR > wRect.bottom)ptCircle.y -= moveptCircleSpeed;
		//	if(ptCircle.y + CircleMove.y - CircleMoveR < wRect.top)ptCircle.y += moveptCircleSpeed;
		//	
		//	DrawCircle(hdc, ptCircle.x + CircleMove.x, ptCircle.y + CircleMove.y,CircleMoveR);

		//	POINT CircleTimer1{ 1000, 300 };
		//	int CircleTimer1R = 75;
		//	if (ptCircleTimer.x + CircleTimer1.x + CircleTimer1R > wRect.right)ptCircleTimer.x -= moveptCircleSpeed;
		//	DrawCircle(hdc, ptCircleTimer.x+CircleTimer1.x, CircleTimer1.y, CircleTimer1R);
		//	POINT CircleTimer2{ 1000, 300 };
		//	int CircleTimer2R = 75;
		//	if (ptCircleTimer.y + CircleTimer2.y + CircleTimer2R > wRect.bottom)ptCircleTimer.y -= moveptCircleSpeed;
		//	DrawCircle(hdc, CircleTimer2.x,ptCircleTimer.y+ CircleTimer2.y, CircleTimer2R);

			//hPen = CreatePen(PS_SOLID, 1, RGB(255, 128, 100));
			//oldPen = (HPEN)SelectObject(hdc, hPen);
			//POINT CircleClick{ 400,200 };
			//int CircleClickR = 70;
			//DrawCircle(hdc, ptCircleClick.x, ptCircleClick.y, CircleClickR);
			//SelectObject(hdc, oldPen);
			//DeleteObject(hPen);

			//int CirclemMoveR = 100;
			//DrawCircle(hdc, ptCirclemMove.x, ptCirclemMove.y, CirclemMoveR);

//클라이언트 영역에 마우스 클릭하면 해당 위치에 원을 생성
//임의 방향으로 이동/ 클라이언트 외곽에서 반사\/
//1번 부딪히면 합체 2번 부딪히면 쪼개지게
//랜덤으로 원 별 사각형이 그려지게
//서로 충돌할때 상성고려해서 같으면 팅기고 다르면 그 관계정해진대로 ㄱㄱ
		if (j >= 0) {
			for (int i = 0; i <= j; i++)
			{
				if (circles[i]->CollisionWall(wRect) == 1)
				{
					if (circles[i]->GetPOINT().y + circles[i]->GetR() >= wRect.bottom ||
						circles[i]->GetPOINT().y - circles[i]->GetR() <= wRect.top)
						circles[i]->SetDir(-circles[i]->GetDir());
					else
						circles[i]->SetDir(-circles[i]->GetDir()-M_PI);
				}
				for (int k = 0; k <= j; k++)
				{
					if (k != i) {
						if (circles[i]->CollisionCircle(circles[k]) == 1)
						{
							circles[i]->SetDir();
							circles[k]->SetDir();
							//circles[i]->SetDir(GetDegree(circles[i]->GetDir(), circles[k]->GetDir()));
							//circles[k]->SetDir(GetDegree(circles[k]->GetDir(), circles[i]->GetDir()));
						/*circles[i]->SetDir(circles[i]->GetDir() - abs(circles[i]->GetDir() - GetDegree(circles[k]->GetDir() + M_PI)) * 2);
							circles[k]->SetDir(circles[k]->GetDir() - abs(circles[k]->GetDir() - GetDegree(circles[i]->GetDir() + M_PI)) * 2);*/
						}
					}
				}
				DrawCircle(hdc, circles[i]->GetPOINT().x, circles[i]->GetPOINT().y, circles[i]->GetR());
			}

		}


		hBrush = CreateSolidBrush(color);
		OldBrush = (HBRUSH)SelectObject(hdc, hBrush);
		Ellipse(hdc, 10, 10, 200, 200);
		SelectObject(hdc, OldBrush);
		DeleteObject(hBrush);

			EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY: //메모리 해제, 파일 종료 시키는 곳
        PostQuitMessage(0);
		HideCaret(hWnd);
		DestroyCaret();
		PostQuitMessage(0);
		delete[] stars;


		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		KillTimer(hWnd, 3);
		
		for (int i = 0; i <= j; i++)
		{
			delete circles[i];
		}
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

	//DrawCircle(hdc, x, y, r);
	double smallR = r * cos((M_PI - theta) / 2) / (1 - cos((M_PI - theta) / 2)); //제1코사인법칙 유도
	POINT Circlept = { x, y + r+round(smallR)};

	DrawCircle(hdc, Circlept.x, Circlept.y, round(smallR));

	for (int i = 0; i < num - 1; i++)
	{
		Circlept = myRotate({ Circlept.x, Circlept.y }, { x, y }, theta);
		//TextOut(hdc, Circlept.x, Circlept.y, _T("*"), _tcslen(_T("*")));
		DrawCircle(hdc, Circlept.x, Circlept.y, round(smallR));
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
void DrawRectText(HDC hdc, RECT rc, LPWSTR str)
{
	SIZE size;
	DrawRectangle(hdc, (rc.left + rc.right) / 2, (rc.top + rc.bottom) / 2, rc.right - rc.left+5, rc.top - rc.bottom-5);
			DrawText(hdc, str, _tcslen(str), &rc, DT_TOP | DT_LEFT);
			GetTextExtentPoint(hdc, str, _tcslen(str), &size);
		//TextOut(hdc, rc.left, rc.top, str, _tcslen(str));
			double y_size = size.cy;
			
		SetCaretPos(size.cx+rc.left, rc.top);
	}
void DrawRotatedStar(HDC hdc, POINT star, int r, double angle)
{
	double rot_theta = angle * M_PI / 180;
	double theta = 360 / 5 * M_PI / 180;
	POINT p1 = { star.x, star.y - r };
	POINT p2 = myRotate(p1, star, theta);
	POINT p3 = myRotate(p2, star, theta);
	POINT p4 = myRotate(p3, star, theta);
	POINT p5 = myRotate(p4, star, theta);

	double a = double((p3.x - p4.x) / 2)*(p2.y - p1.y) / (p3.y - p1.y);
	POINT poly1;
	poly1.y = p2.y;
	poly1.x = star.x + a;
	poly1 = myRotate(poly1, star, rot_theta);
	POINT poly2 = myRotate(poly1, star, theta);
	POINT poly3 = myRotate(poly2, star, theta);
	POINT poly4 = myRotate(poly3, star, theta);
	POINT poly5 = myRotate(poly4, star, theta);

	p1 = myRotate({ star.x, star.y - r },star,rot_theta);
	p2 = myRotate(p1, star, theta);
	p3 = myRotate(p2, star, theta);
	p4 = myRotate(p3, star, theta);
	p5 = myRotate(p4, star, theta);
	POINT starPt[20] = { p1,poly1, p2,poly2, p3,poly3, p4,poly4, p5,poly5 };
	Polygon(hdc, starPt, 10);
}
void DrawStar(HDC hdc, POINT star, int r)
{
	
	double theta =  360/5*M_PI / 180;
	POINT p1 = { star.x, star.y - r };
	POINT p2 = myRotate(p1, star, theta);
	POINT p3 = myRotate(p2, star, theta);
	POINT p4 = myRotate(p3, star, theta);
	POINT p5 = myRotate(p4, star, theta);

	double a = double((p3.x - p4.x)/2)*(p2.y - p1.y) / (p3.y - p1.y);
	POINT poly1;
	poly1.y = p2.y;
	poly1.x = star.x + a;
	POINT poly2 = myRotate(poly1, star, theta);
	POINT poly3 = myRotate(poly2, star, theta);
	POINT poly4 = myRotate(poly3, star, theta);
	POINT poly5 = myRotate(poly4, star, theta);

	POINT starPt[20] = {p1,poly1, p2,poly2, p3,poly3, p4,poly4, p5,poly5};
	Polygon(hdc, starPt, 10);
}
Stars RandomlyRotatedStars(HDC hdc, int howmanyStars, POINT leftTop, POINT rightBottom, int maxSize, int minSize, Stars *stars)
{
	static int cnt = 0;

	//stars->star = new POINT[howmanyStars];
	//stars->angle = new int[howmanyStars];
	//stars->r = new int[howmanyStars];
	if (cnt == 0)
	{
		srand((unsigned)time(0));

		for (int i = 0; i < howmanyStars; i++)
		{
			stars[i].star.x = rand() % (rightBottom.x - leftTop.x);
			stars[i].star.y = rand() % (rightBottom.y - leftTop.y);
			stars[i].r = rand() % (maxSize - minSize) + minSize;
			stars[i].angle = rand() % 360;
			
		}
		cnt++;
	}

	return *stars;

}
void DrawRandomlyRotatedStars(HDC hdc,int  howmanyStars,Stars  *stars)
{
	for (int i = 0; i < howmanyStars; i++)
	{
		DrawRotatedStar(hdc, stars[i].star, stars[i].r, stars[i].angle);
	}
}
bool CShape:: CollisionCircle( CShape *other)
{
	float a = this->PT.x - other->PT.x;
	float b = this->PT.y - other->PT.y;
	
	float length = sqrt(a*a + b * b);

	if (length <= (this->R + other->R))
		return TRUE;
}
float GetDegree(float deg)
{
	if (deg < 0)
		deg += 2*M_PI;
	if (deg > 2*M_PI)
		deg -= 2*M_PI;

	return deg;
}
float GetDegree(float a, float b)
{
	if (a > b + M_PI)
		return (a - (a - GetDegree(b + M_PI)));
	else
		return (GetDegree(b + M_PI) - (GetDegree(b + M_PI) - a));

}