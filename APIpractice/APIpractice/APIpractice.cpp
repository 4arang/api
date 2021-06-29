// APIpractice.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "Resource.h"
#include "framework.h"
#include "APIpractice.h"
#include <tchar.h>
#include <algorithm>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <commdlg.h>
#define MAX_LOADSTRING 100

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
class CShape
{
protected:
	POINT PT;
	POINT MovingPT;
	int Speed = 7;
	int R = 30;
	double Dir;
public:
	//CShape();
	//~CShape();
	POINT GetPOINT() const { return PT; }
	int GetR() const { return R; }
	void SetPOINT(POINT p) { PT = p; }
	void SetDir(double theta) { Dir = theta; }
	void SetR(int r) { R = r; }
	void SetSpeed(int speed) { Speed = speed; }
	virtual	void update() = 0 {}
	virtual void Show(HDC &hdc) = 0 {}
	//virtual bool CollisionWall(RECT wRect) = 0;
	//virtual void DrawMe(HDC hdc) = 0;
	virtual bool CollisionEnemy(CShape* circle, CShape* MP) = 0;
};
class CMissile : public CShape
{
public:
	CMissile() : CShape() {}
	void update();
	void Show(HDC &hdc);
	bool CollisionEnemy(CShape* circle, CShape* MP);
};
class CEnemy :public CShape
{
	//CCircle() : R(R) {}
public:
	CEnemy() : CShape() {}
	void update() { PT.y += Speed; }
	void Show(HDC &hdc) { Ellipse(hdc, PT.x - R, PT.y - R, PT.x + R, PT.y + R); }
	bool CollisionEnemy(CShape* circle, CShape* MP);
};

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void DrawGrid(HDC hdc, int x, int y, int r, int c, int w, int h);
POINT myRotate(POINT Opoint, POINT Hpoint, double theta); //회전행렬
void DrawCircle(HDC hdc, int x, int y, int r);
void DrawRectangle(HDC hdc, int x, int y, int w, int h);
void DrawStar(HDC hdc, POINT star, int r);
void DrawRotatedStar(HDC hdc, POINT star, int r, double angle);
Stars RandomlyRotatedStars(HDC hdc, int howmanyStars, POINT leftTop, POINT rightBottom, int maxSize, int minSize, Stars *stars);
void DrawRandomlyRotatedStars(HDC hdc, int  howmanyStars, Stars  *stars);
//float GetDegree(float deg);
//float GetDegree(float a, float b);
void DrawBunker(HDC hdc, double x, double y, int r, double theta);
void DrawBunkerGun(HDC hdc, POINT OP, POINT BP, double theta);
void DrawMissile(HDC hdc, POINT MP, int r, double theta);



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
    LoadStringW(hInstance, IDC_APIPRACTICE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_APIPRACTICE));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APIPRACTICE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_APIPRACTICE);
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
	HDC hdc;
	RECT wRect;
	GetClientRect(hWnd, &wRect);
	HPEN hPen, oldPen;
	HBRUSH hBrush, oldBrush;
	int howmanyStars = 15;
	static struct Stars *stars = new Stars[howmanyStars];
	int BunkerR = wRect.right/15;
	POINT BunkerPT = { wRect.right / 2, wRect.bottom - wRect.bottom/10 };
	static CShape *circles[11];
	static int circlei=0;
	static CShape *missiles[20];
	static int missilei = 0;


	static double BunkerGunAngle=0;




    switch (message)
    {
	case WM_CREATE :
	{
		SetTimer(hWnd, 1, 1000, NULL);
		SetTimer(hWnd, 2, 100, NULL);
	}
	break;
	case WM_TIMER :
		switch (wParam)
		{
		case 1:
		{

			if (circles[circlei] != NULL)
				circles[circlei]=NULL;
			circles[circlei] = new CEnemy();
			circles[circlei]->SetPOINT({ rand() % wRect.right, 50 });
			circlei++;
			if (circlei >= 11)
			{
				circlei = 0;
			}

		}
		break;
		case 2:
		{
			for (int i = 0; i < 11; i++)
			{
				if (circles[i] != NULL)
					circles[i]->update();
			}
			for(int i=0; i<20; i++)
			{
				if (missiles[i] != NULL)
				missiles[i]->update();
			}
		}
		break;
		default: break;
		}
		InvalidateRect(hWnd, NULL, true);
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
	case WM_KEYDOWN:
	{
		hdc = GetDC(hWnd);
		if (wParam == VK_LEFT )
		{
			if(BunkerGunAngle>-90)
			BunkerGunAngle -= 2.5;
		}
		if (wParam == VK_RIGHT)
		{
			if (BunkerGunAngle < 90)
			BunkerGunAngle += 2.5;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		ReleaseDC(hWnd, hdc);
	}
	break;
	case WM_KEYUP:
	{
		//hdc = GetDC(hWnd);
		if (wParam == VK_SPACE)
		{
			if (missiles[missilei] != NULL)
			{
				missiles[missilei]=NULL;
				
			}
			missiles[missilei] = new CMissile();
		
			POINT MP = { BunkerPT.x, BunkerPT.y - BunkerR*5/3 };
			POINT newMP1 = myRotate(MP, BunkerPT, BunkerGunAngle*M_PI / 180);
			double theta;
			if (BunkerGunAngle > 0) theta = M_PI / 2 - BunkerGunAngle*M_PI/180;
			else theta = -M_PI / 2 - BunkerGunAngle*M_PI/180;
			missiles[missilei]->SetPOINT({ newMP1.x,newMP1.y });
			missiles[missilei]->SetDir(theta);
			missiles[missilei]->SetR(BunkerR/7);
			missiles[missilei]->SetSpeed(15);
			missilei++;
			if (missilei >= 20)
			{
				missilei = 0;
			}
		}

		//	DrawMissile(hdc, BunkerPT, BunkerR, BunkerGunAngle);
	}
		//InvalidateRect(hWnd, NULL, TRUE);
		//ReleaseDC(hWnd, hdc);
	
	break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...


			//////////////////////////////////배경///////////////////////////////
			hPen = CreatePen(PS_SOLID, 1, RGB(255, 234, 050));
			oldPen = (HPEN)SelectObject(hdc, hPen);
			hBrush = CreateSolidBrush(RGB(255, 234, 050));
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
			RandomlyRotatedStars(hdc, howmanyStars, { wRect.left,wRect.top }, { wRect.right,wRect.bottom }, 28, 5, stars);
			DrawRandomlyRotatedStars(hdc, howmanyStars, stars);
			SelectObject(hdc, oldBrush);
			DeleteObject(hBrush);
			SelectObject(hdc, oldPen);
			DeleteObject(hPen);

			hBrush = CreateSolidBrush(RGB(102, 102, 204));
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
			Pie(hdc, -wRect.right/2,wRect.top-wRect.right/2,wRect.right+wRect.right/2,wRect.top+wRect.bottom/15,
				wRect.left,0,wRect.right,0);
			SelectObject(hdc, oldBrush);
			DeleteObject(hBrush);
			///////////////////////////////////////////////////////////////////


			hBrush = CreateSolidBrush(RGB(102, 204, 102));
			oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
			DrawBunker(hdc, BunkerPT.x, BunkerPT.y, BunkerR, BunkerGunAngle);
			SelectObject(hdc, oldBrush);
			DeleteObject(hBrush);
			


			for (int i = 0; i < 11; i++)
			{
				if (circles[i] != NULL)
					circles[i]->Show(hdc);
			}
				for(int i = 0; i<20; i++)
			{
				if (missiles[i] != NULL)
					missiles[i]->Show(hdc);
			}
			

			
			
		

			
			int Life_width = wRect.right/4;
			int Life_height = wRect.bottom/10;
			POINT Life_1 = { wRect.left + Life_width / 2, wRect.bottom - Life_height / 2 };
			POINT Life_2 = { Life_1.x + Life_width, wRect.bottom - Life_height / 2 };
			POINT Life_3 = { Life_2.x + Life_width, wRect.bottom - Life_height / 2 };
			POINT Life_4 = { Life_3.x + Life_width, wRect.bottom - Life_height / 2 };
			DrawRectangle(hdc, Life_1.x, Life_1.y, Life_width, Life_height); 
			DrawRectangle(hdc, Life_2.x, Life_2.y, Life_width, Life_height);
			DrawRectangle(hdc, Life_3.x, Life_3.y, Life_width, Life_height);
			DrawRectangle(hdc, Life_4.x, Life_4.y, Life_width, Life_height);
			

			for (int i = 0; i < 20; i++)
			{
				if (missiles[i] != NULL)
				{
					if (missiles[i]->GetPOINT().y <= wRect.top)  missiles[i]=NULL;
					for (int k = 0; k < 11; k++)
					{
						if (circles[k] != NULL) {
							if (missiles[i]->CollisionEnemy(circles[k], missiles[i]) == 1)
							{
								missiles[i] = NULL;
								circles[k] = NULL;
								break;
							}
						}
					}
				}
			}

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);

		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		delete[] stars;
		for (int i = 0; i < 11; i++)
		{
			delete circles[i];
		}
		for (int i = 0; i < 20; i++)
		{
			delete missiles[i];
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
POINT myRotate(POINT Opoint, POINT Hpoint, double theta) //회전행렬
{
	POINT rpoint;
	double x = cos(theta);
	rpoint.x = cos(theta)*(Opoint.x - Hpoint.x) - sin(theta)*(Opoint.y - Hpoint.y) + Hpoint.x;
	rpoint.y = sin(theta)*(Opoint.x - Hpoint.x) + cos(theta)*(Opoint.y - Hpoint.y) + Hpoint.y;
	return rpoint;
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

	p1 = myRotate({ star.x, star.y - r }, star, rot_theta);
	p2 = myRotate(p1, star, theta);
	p3 = myRotate(p2, star, theta);
	p4 = myRotate(p3, star, theta);
	p5 = myRotate(p4, star, theta);
	POINT starPt[20] = { p1,poly1, p2,poly2, p3,poly3, p4,poly4, p5,poly5 };
	Polygon(hdc, starPt, 10);
}
void DrawStar(HDC hdc, POINT star, int r)
{

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
	POINT poly2 = myRotate(poly1, star, theta);
	POINT poly3 = myRotate(poly2, star, theta);
	POINT poly4 = myRotate(poly3, star, theta);
	POINT poly5 = myRotate(poly4, star, theta);

	POINT starPt[20] = { p1,poly1, p2,poly2, p3,poly3, p4,poly4, p5,poly5 };
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
void DrawRandomlyRotatedStars(HDC hdc, int  howmanyStars, Stars  *stars)
{
	for (int i = 0; i < howmanyStars; i++)
	{
		DrawRotatedStar(hdc, stars[i].star, stars[i].r, stars[i].angle);
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
void DrawBunker(HDC hdc, double x, double y, int r, double theta)
{
	theta *= M_PI / 180;
	POINT OP = { x, y - r * 5/3 };
	POINT BP = { x, y };
	DrawBunkerGun(hdc, OP, BP, theta);
	Pie(hdc, x - r, y - r, x + r, y + r, x + r, y, x - r, y);
}
void DrawBunkerGun(HDC hdc, POINT OP, POINT BP,  double theta)
{

	POINT TP = myRotate(OP, BP, theta);
	
		MoveToEx(hdc, BP.x, BP.y, NULL);
		LineTo(hdc, TP.x, TP.y);
}
void DrawMissile(HDC hdc, POINT MP, int r, double theta)
{
	theta *= M_PI / 180;
	POINT BP = MP;
	MP = { BP.x, BP.y - r * 5/3-r/7 };
	POINT newMP1 = myRotate(MP, BP, theta);
	POINT newMP2 = myRotate({ MP.x, MP.y + r/7 }, BP, theta);
	MoveToEx(hdc, newMP1.x, newMP1.y, NULL);
	LineTo(hdc, newMP2.x, newMP2.y);
}
void CMissile ::Show(HDC &hdc)
{

	POINT newMP2 = {PT.x-R*cos(Dir), PT.y+R*sin(Dir)};
	MoveToEx(hdc, PT.x, PT.y, NULL);
	LineTo(hdc, newMP2.x, newMP2.y); 

}
void CMissile::update()
{
	if (Dir > 0) {
		PT.x += cos(Dir) * Speed;
		PT.y -= sin(Dir) * Speed;
	}
	else {
		PT.x -= cos(Dir) * Speed;
		PT.y += sin(Dir) * Speed;
	}
}
bool CMissile::CollisionEnemy(CShape* circle, CShape* MP)
{
	if (sqrt((circle->GetPOINT().x - MP->GetPOINT().x)*(circle->GetPOINT().x - MP->GetPOINT().x) +
		(circle->GetPOINT().y - MP->GetPOINT().y)*(circle->GetPOINT().y - MP->GetPOINT().y))
		<= circle->GetR())
		return true;
}
bool CEnemy::CollisionEnemy(CShape* circle, CShape* MP)
{
	return true;
}