// APIpr2.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
/*
문.8방향 캐릭터 이미지를 이용해서키 누른 방향으로 캐릭터 이동하도록
애니메이션 프로그램을 작성하라.

*/
/*
다음주까지 갈스패닉 구현
*/
#include "framework.h"
#include "APIpr2.h"
//>> : GDI+
#include <ObjIdl.h>
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")
using namespace Gdiplus;

//<< :
#pragma comment(lib, "msimg32.lib") //특정 color 빼려고 포함시킴

#define MAX_LOADSTRING 100

///////////back
HBITMAP hBackImage;
BITMAP bitBack; //비트맵에 대한 정보
//transparent
HBITMAP hTransparentImage;
BITMAP bitTransparent;
//animation
HBITMAP hAniImage;
BITMAP bitAni;
const int SPRITE_SIZE_X = 57;
const int SPRITE_SIZE_Y = 52;



POINT ptAni;


// : run
int RUN_FRAME_MAX = 0;
int RUN_FRAME_MIN = 0;
int curframe = RUN_FRAME_MIN;

void CreateBitmap();
void DrawBitmap(HWND hWnd, HDC hdc);
void DeleteBitmap();

void Update();
void UpdateFrame();
void UpdateFrame(HWND hWnd);
//////////
//Text
void DrawRectText(HDC hdc);
RECT rectView;
//////8way
HBITMAP h8wayImage;
BITMAP bit8way;
const int SPRITE_SIZE_X2 = 63;
const int SPRITE_SIZE_Y2 = 80;
POINT ptAni2;
static int bit8wayi = 0;

//<< :

//>> : double buffering
HBITMAP hDoubleBufferImage;

void DrawBitmapDoubleBuffering(HWND hWnd, HDC hdc);

HBITMAP hReverseImage;
//<< :

//>>:GDI+
ULONG_PTR g_GdiToken;
void Gdi_Init();
void Gdi_Draw(HDC hdc);
void Gdi_End();
void OnGdi_Paint(HDC hdc);

//<<



// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
    LoadStringW(hInstance, IDC_APIPR2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_APIPR2));

    MSG msg;

    // 기본 메시지 루프입니다:
 /*   while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }*/
	Gdi_Init();
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			Update();
			//Render();
		}
	}
	Gdi_End();
    return (int) msg.wParam;
}



void CreateBitmap()
{

	//>>:back image
	hBackImage = (HBITMAP)LoadImage(NULL, TEXT("images/수지.bmp")
		, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	GetObject(hBackImage, sizeof(BITMAP), &bitBack);
	//<<:

	//>>:tarnsparent image
	hTransparentImage = (HBITMAP)LoadImage(NULL, TEXT("images/sigong.bmp")
		, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	GetObject(hTransparentImage, sizeof(BITMAP), &bitTransparent);
	//<<

	//>>:animatioan
	hAniImage = (HBITMAP)LoadImage(NULL, TEXT("images/zero_run.bmp")
		, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	GetObject(hAniImage, sizeof(BITMAP), &bitAni);

	RUN_FRAME_MAX = bitAni.bmWidth / SPRITE_SIZE_X - 1;
	RUN_FRAME_MIN = 2;
	curframe = RUN_FRAME_MIN;
	//<<
	ptAni.x = 200;
	ptAni.y = 400;

	//8way
	h8wayImage = (HBITMAP)LoadImage(NULL, TEXT("images/8wayCharacter.bmp")
		, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	GetObject(h8wayImage, sizeof(BITMAP), &bit8way);
	RUN_FRAME_MAX = bit8way.bmWidth / SPRITE_SIZE_X2 - 1;
	RUN_FRAME_MIN = 2;
	curframe = RUN_FRAME_MIN;

	ptAni2.x = 400;
	ptAni2.y = 400;
}

void DrawBitmap(HWND hWnd, HDC hdc)
{
	HDC hMemDC; //더블버퍼링과 관련있음 바로그리는게 아니라 그 이면에 같은 크기의 메모리영역을 만드는거
	HBITMAP hOldBitmap;
	int bx, by;
	//back
	{
		hMemDC = CreateCompatibleDC(hdc);
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBackImage);
		bx = bitBack.bmWidth;
		by = bitBack.bmHeight;

		BitBlt(hdc, 0, 0, bx, by, hMemDC, 0, 0, SRCCOPY);

		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
	}
	//sigong
	{
		hMemDC = CreateCompatibleDC(hdc);
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hTransparentImage);
		bx = bitTransparent.bmWidth;
		by = bitTransparent.bmHeight;

		BitBlt(hdc, 100, 200, bx, by, hMemDC, 0, 0, SRCCOPY);
		TransparentBlt(hdc, 200, 200, bx, by, hMemDC, 0, 0,
			bx, by, RGB(255, 0, 255));

		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
	}
	//ani
	{
		hMemDC = CreateCompatibleDC(hdc);
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hAniImage);
		bx = bitAni.bmWidth/16;
		by = bitAni.bmHeight/2;

		int xStart = curframe * bx;
		int yStart = 0;


		//안씀BitBlt(hdc, 100, 200, bx, by, hMemDC, 0, 0, SRCCOPY);
		TransparentBlt(hdc, ptAni.x,ptAni.y, bx, by, hMemDC, xStart, yStart,
			bx, by, RGB(255, 0, 255));

		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
	}
	//8way
	{
		hMemDC = CreateCompatibleDC(hdc);
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, h8wayImage);
		//bx = bit8way.bmWidth / 8;
		//by = bit8way.bmHeight / 8;
			bx = bit8way.bmWidth / 8;
		by = bit8way.bmHeight / 8;

		int xStart = curframe * bx;
		int yStart = by*bit8wayi;// curframe*by;


		//안씀BitBlt(hdc, 100, 200, bx, by, hMemDC, 0, 0, SRCCOPY);
		TransparentBlt(hdc, ptAni2.x, ptAni2.y, bx, by, hMemDC, xStart, yStart,
			bx, by, RGB(34, 34, 34));

		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
	}
}


//oldTime = newTime;
//UpdateFrame();
//}
void DeleteBitmap()
{
	DeleteObject(hBackImage);
	DeleteObject(hTransparentImage);
	DeleteObject(hAniImage);
	DeleteObject(h8wayImage);
}

void UpdateFrame(HWND hWnd)
{

	curframe++;
	if (curframe > RUN_FRAME_MAX)
		curframe = RUN_FRAME_MIN;
}
void UpdateFrame()
{
	if (GetKeyState(VK_RIGHT) & 0x8000)	//asyncstate
		{
		ptAni.x += 10;
		ptAni2.x += 10;
		bit8wayi = 2;
		}
	if (GetKeyState(VK_LEFT) & 0x8000)
	{
		ptAni.x -= 10;
		ptAni2.x -= 10;
		bit8wayi = 6;
	}
	if (GetKeyState(VK_UP) & 0x8000)
	{
		ptAni.y -= 10;
		ptAni2.y -= 10;
		bit8wayi = 4;
	}
	if (GetKeyState(VK_DOWN) & 0x8000)
	{
		ptAni.y += 10;
		ptAni2.y += 10;
		bit8wayi = 0;
	}
	if (GetKeyState(VK_DOWN) & 0x8000 && GetKeyState(VK_LEFT) & 0x8000)
	{
		bit8wayi = 7;
	}
	if (GetKeyState(VK_DOWN) & 0x8000 && GetKeyState(VK_RIGHT) & 0x8000)
	{
		bit8wayi = 1;
	}
	if (GetKeyState(VK_UP) & 0x8000 && GetKeyState(VK_LEFT) & 0x8000)
	{

		bit8wayi = 5;
	}
	if (GetKeyState(VK_UP) & 0x8000 && GetKeyState(VK_RIGHT) & 0x8000)
	{

		bit8wayi = 3;
	}
}
VOID CALLBACK AniProc(HWND hWnd, UINT uMsg, UINT idEvenet, DWORD dwTime);





void DrawBitmapDoubleBuffering(HWND hWnd, HDC hdc)
{
	HDC hMemDC;
	HBITMAP hOldBitmap;
	int bx, by;

	HDC hMemDC2;
	HBITMAP hOldBitmap2;

	hMemDC = CreateCompatibleDC(hdc); //206p
	if (!hMemDC)
	{
		MessageBox(hWnd, _T("CreateCompatibleDC failed"), _T("Error"), MB_OK);
		return;
	}
	if (hDoubleBufferImage == NULL) //아무것도없는상태여야 작업
	{
		hDoubleBufferImage = CreateCompatibleBitmap(hdc,
			rectView.right, rectView.bottom);//이미지파일을 가상의메모리로 만듬
	}
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hDoubleBufferImage);
	//>>:back
	{ //전에 쓴거 그대로가져온거
		hMemDC2 = CreateCompatibleDC(hMemDC);
		hOldBitmap2 = (HBITMAP)SelectObject(hMemDC2, hBackImage);
		bx = bitBack.bmWidth;
		by = bitBack.bmHeight;

		BitBlt(hMemDC, 0, 0, bx, by, hMemDC2, 0, 0, SRCCOPY);
		SelectObject(hMemDC2, hOldBitmap2);
		DeleteDC(hMemDC2);
	}
	//>>:sigong
	{ //전에 쓴거 그대로가져온거
		hMemDC2 = CreateCompatibleDC(hMemDC);
		hOldBitmap2 = (HBITMAP)SelectObject(hMemDC2, hTransparentImage);
		bx = bitTransparent.bmWidth;
		by = bitTransparent.bmHeight;

		BitBlt(hMemDC, 100, 200, bx, by, hMemDC2, 0, 0, SRCCOPY);
		TransparentBlt(hMemDC, 200, 200, bx, by, hMemDC2, 0, 0,
			bx, by, RGB(255, 0, 255));

		SelectObject(hMemDC2, hOldBitmap2);
		DeleteDC(hMemDC2);
	}
	//>>:animation
	{
		hMemDC2 = CreateCompatibleDC(hMemDC);
		hOldBitmap2 = (HBITMAP)SelectObject(hMemDC2, hAniImage);

		//좌우반전용
		HDC hReverseMemDC;
		HBITMAP hOldBitmap3;
		hReverseMemDC = CreateCompatibleDC(hMemDC);
		if (!hReverseImage)
		{
			hReverseImage = CreateCompatibleBitmap(hMemDC2, bx, by);
		}
		hOldBitmap3 = (HBITMAP)SelectObject(hReverseMemDC, hReverseImage);
		//

		bx = bitAni.bmWidth / 16;
		by = bitAni.bmHeight / 2;

		int xStart = curframe * bx;
		int yStart = 0;
		//StretchBlt(hReverseMemDC, bx - 1, 0, -bx, by, hMemDC2, xStart, yStart,
		//	bx, by, SRCCOPY);
		//TransparentBlt(hMemDC, ptAni.x+100, ptAni.y+100, bx, by, hMemDC2, xStart-1, yStart,
		//	-bx, by, RGB(255, 0, 255)); //200, 400출발이었음

		TransparentBlt(hMemDC, ptAni.x, ptAni.y, bx, by, hMemDC2, xStart, yStart,
			bx, by, RGB(255, 0, 255)); //200, 400출발이었음

		SelectObject(hMemDC2, hOldBitmap2);
		DeleteDC(hMemDC2);
	}
	//8way
	{
		hMemDC2 = CreateCompatibleDC(hMemDC);
		hOldBitmap2 = (HBITMAP)SelectObject(hMemDC2, h8wayImage);
		//bx = bit8way.bmWidth / 8;
		//by = bit8way.bmHeight / 8;
		bx = bit8way.bmWidth / 8;
		by = bit8way.bmHeight / 8;

		int xStart = curframe * bx;
		int yStart = by * bit8wayi;// curframe*by;


		//안씀BitBlt(hdc, 100, 200, bx, by, hMemDC, 0, 0, SRCCOPY);
		TransparentBlt(hMemDC, ptAni2.x, ptAni2.y, bx*2, by*2, hMemDC2, xStart, yStart,
			bx, by, RGB(34, 34, 34));

		SelectObject(hMemDC2, hOldBitmap2);
		DeleteDC(hMemDC2);
	}

	Gdi_Draw(hMemDC);


	BitBlt(hdc, 0, 0, rectView.right, rectView.bottom, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);
	DeleteDC(hMemDC);

	
}

void Gdi_Init()
{
	GdiplusStartupInput gpsi;
	GdiplusStartup(&g_GdiToken, &gpsi, NULL);
}

void Gdi_Draw(HDC hdc)
{
	OnGdi_Paint(hdc);
}

void Gdi_End()
{
	GdiplusShutdown(g_GdiToken);
}

void OnGdi_Paint(HDC hdc)
{
	Graphics graphics(hdc); //memoryDC
	SolidBrush brush(Color(255, 255, 0, 0));
	FontFamily fontFamily(L"Times New Roman");
	Font font(&fontFamily, 24, FontStyleRegular, UnitPixel);
	PointF pointF(10.0f, 20.0f);
	graphics.DrawString(L"Hello GDI+", -1, &font, pointF, &brush);
	//<<:

	//>>;line
	Pen pen(Color(50, 0, 255, 255));
	graphics.DrawLine(&pen, 0, 0, 200, 100);
	//<<:

	//>>: image
	//Image *pImg = nullptr;
	//pImg = ((WCHAR*)L"images/sigong.png");
	Image img((WCHAR*)L"images/sigong.png");
	int w = img.GetWidth();
	int h = img.GetHeight();
	graphics.DrawImage(&img, 100, 100, w, h);

	//image rotation
	Image* pImage = nullptr;
	pImage = Image::FromFile((WCHAR*)L"images/sigong.png");
	Gdiplus::Matrix mat;
	static int rot = 0;
	int xPos = 200;
	int yPos = 100;
	mat.RotateAt(rot % 360, Gdiplus::PointF(xPos+ (float)w / 2,yPos+  (float)(h / 2)));
	rot += 2;
	graphics.SetTransform(&mat);
	graphics.DrawImage(pImage, xPos, yPos, w, h);
	mat.RotateAt(-(rot % 360), Gdiplus::PointF(xPos + (float)w / 2, yPos + (float)(h / 2)));
	rot -= 2;

	//<<
	//>> : colorKey뺴려면
	ImageAttributes imagAttr;
	imagAttr.SetColorKey(Color(240, 0, 240), Color(255, 10, 255)); //범위 지정할 수 있음
	xPos = 300;
	//mat.RotateAt(rot % 360, Gdiplus::PointF(xPos + (float)w / 2, yPos + (float)(h / 2)));
	//graphics.GetTransform(&mat);
	graphics.DrawImage(pImage, Rect(xPos , yPos ,w,h), 0, 0, w, h,
		UnitPixel, &imagAttr);

	//<<;

	//>> :반투명
	brush.SetColor(Color(50, 255, 0, 0));
	graphics.FillRectangle(&brush, 400, 250, 200, 300);
	//<<:

	//>>:
	static REAL transparency = 0.5f;
	transparency += 0.1f;
	if(transparency>1.0f) transparency=0.0f;
	ColorMatrix colorMatrix =  //5 by 5 //r g b a(transparency) default?
	{
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, transparency, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};
	imagAttr.SetColorMatrix(&colorMatrix);
	xPos = 400;

	graphics.DrawImage(pImage, Rect(xPos, yPos, w, h), 0, 0, w, h,
		UnitPixel, &imagAttr);
	//<<:

	
	//>>:
	 colorMatrix =  //5 by 5 //r g b a(transparency) default?
	{
		0.3f, 0.3f, 0.3f, 0.0f, 0.0f,
		0.6f, 0.6f, 0.6f, 0.0f, 0.0f,
		0.1f, 0.1f, 0.1f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};
	imagAttr.SetColorMatrix(&colorMatrix);
	xPos = 500;

	graphics.DrawImage(pImage, Rect(xPos, yPos, w, h), 0, 0, w, h,
		UnitPixel, &imagAttr);
	//<<:

	graphics.DrawArc(&pen, Rect(xPos, yPos, w, h), 45, 270);
	

	if (pImage)delete pImage;
}

//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
//void Render2()
//{
//	PAINTSTRUCT ps;
//	HDC hdc = BeginPaint(g_hWnd, &ps);
//	TEXTOUT(hdc, 100, 100,)
//}
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APIPR2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_APIPR2);
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
    switch (message)
    {
	case WM_CREATE:
	{
		CreateBitmap();
		//SetTimer(hWnd, 123, 100, NULL);
		SetTimer(hWnd, 123, 100, AniProc);
		GetClientRect(hWnd, &rectView);
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
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...

			//DrawBitmap(hWnd, hdc);
			//DrawRectText(hdc);
			DrawBitmapDoubleBuffering(hWnd, hdc);

            EndPaint(hWnd, &ps);
        }
        break;
	case WM_TIMER:
	{
		//UpdateFrame(hWnd);
		//InvalidateRect(hWnd, NULL, false);
	}
	break;
    case WM_DESTROY:
		DeleteBitmap();
		KillTimer(hWnd, 123);
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

VOID CALLBACK AniProc(HWND hWnd, UINT uMsg, UINT idEvenet, DWORD dwTime)
{
	curframe++;
	if (curframe > RUN_FRAME_MAX)
		curframe = RUN_FRAME_MIN;
	InvalidateRect(hWnd, NULL, false); //false true로 놓으면 해결하긴하는데 다시그려야되서 별로
}

void DrawRectText(HDC hdc)
{
	static int yPos = 0;
	TCHAR strTest[] = _T("이미지 출력");
	TextOut(hdc, 10, yPos, strTest, _tcslen(strTest));
	yPos += 5;
	if (yPos > rectView.bottom) yPos = 0;
}

void Update()
{
	DWORD newTime = GetTickCount();
	static DWORD oldTime = newTime;

	if (newTime - oldTime < 100)
		return;
	oldTime = newTime;

	UpdateFrame();
}