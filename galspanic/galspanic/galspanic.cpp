// galspanic.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "galspanic.h"
#include <ObjIdl.h>
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")
using namespace Gdiplus;

//Region *myRegion = new Region[100];
//int myRegioni=0;
//<< :
#pragma comment(lib, "msimg32.lib") //특정 color 빼려고 포함시킴
#define MAX_LOADSTRING 100




// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
///////////back

void goBackToArea();


HBITMAP hBackImage;
BITMAP bitBack; //비트맵에 대한 정보
int RUN_FRAME_MAX = 0;
int RUN_FRAME_MIN = 0;
int curframe = RUN_FRAME_MIN;

int temp1=0, temp2=0;

void Update();
void UpdateFrame();
void UpdateFrame(HWND hWnd);
void DrawRectText(HDC hdc);
RECT rectView;
HBITMAP hDoubleBufferImage;
void DrawBitmapDoubleBuffering(HWND hWnd, HDC hdc);
void CreateBitmap();
//>>:GDI+
ULONG_PTR g_GdiToken;
void Gdi_Init();
void Gdi_Draw(HDC hdc);
void Gdi_End();
void OnGdi_Paint(HDC hdc);
void DeleteBitmap();
void DrawBack(HDC hdc);
//<<

//////8way
HBITMAP h8wayImage;
BITMAP bit8way;
const int SPRITE_SIZE_X2 = 63;
const int SPRITE_SIZE_Y2 = 80;
POINT ptAni2;
static int bit8wayi = 0;

//<< :

//>>:blackImage
VOID CALLBACK AniProc(HWND hWnd, UINT uMsg, UINT idEvenet, DWORD dwTime);

//>>:Character POINt class

class myPoint
{
	POINT pt;
public:
	POINT GetPoint() { return pt; }
	void SetPoint(POINT pt_) { pt = pt_; }
};
myPoint *mp = new myPoint[1000];
int mpi = 0;
POINT orgPt = { 0,0 };
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
    LoadStringW(hInstance, IDC_GALSPANIC, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GALSPANIC));

    MSG msg;

    //// 기본 메시지 루프입니다:
    //while (GetMessage(&msg, nullptr, 0, 0))
    //{
    //    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    //    {
    //        TranslateMessage(&msg);
    //        DispatchMessage(&msg);
    //    }
    //}
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GALSPANIC));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GALSPANIC);
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
	//static myPoint *mp = new myPoint[];



    switch (message)
    {
	case WM_CREATE:
	{
		SetTimer(hWnd, 123, 100, AniProc);
		GetClientRect(hWnd, &rectView);
		CreateBitmap();
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

			DrawBitmapDoubleBuffering(hWnd, hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		DeleteBitmap();
		KillTimer(hWnd, 123);
		delete[] mp;

		//delete[] myRegion;
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


void CreateBitmap()
{

	//>>:back image
	hBackImage = (HBITMAP)LoadImage(NULL, TEXT("images/unseen.bmp")
		, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	GetObject(hBackImage, sizeof(BITMAP), &bitBack);
	//<<:
		//8way
	h8wayImage = (HBITMAP)LoadImage(NULL, TEXT("images/8wayCharacter.bmp")
		, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	GetObject(h8wayImage, sizeof(BITMAP), &bit8way);
	RUN_FRAME_MAX = bit8way.bmWidth / SPRITE_SIZE_X2 - 1;
	RUN_FRAME_MIN = 2;
	curframe = RUN_FRAME_MIN;

	ptAni2.x = rectView.right / 2 ;
	ptAni2.y = rectView.bottom / 2;
}

void DrawBitmap(HWND hWnd, HDC hdc)
{
	HDC hMemDC; //더블버퍼링과 관련있음 바로그리는게 아니라 그 이면에 같은 크기의 메모리영역을 만드는거
	HBITMAP hOldBitmap;
	int bx, by;
	//back
	//{
	//	hMemDC = CreateCompatibleDC(hdc);
	//	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBackImage);
	//	bx = bitBack.bmWidth;
	//	by = bitBack.bmHeight;

	//	StretchBlt(hdc, rectView.right/2, rectView.left/2, bx*3,by*3, hMemDC, 0, 0,
	//		bx, by, SRCCOPY);

	//	SelectObject(hMemDC, hOldBitmap);
	//	DeleteDC(hMemDC);
	//}
	////8way
	//{
	//	hMemDC = CreateCompatibleDC(hdc);
	//	hOldBitmap = (HBITMAP)SelectObject(hMemDC, h8wayImage);
	//	//bx = bit8way.bmWidth / 8;
	//	//by = bit8way.bmHeight / 8;
	//	bx = bit8way.bmWidth / 8;
	//	by = bit8way.bmHeight / 8;

	//	int xStart = curframe * bx;
	//	int yStart = by * bit8wayi;// curframe*by;


	//	//안씀BitBlt(hdc, 100, 200, bx, by, hMemDC, 0, 0, SRCCOPY);
	//	TransparentBlt(hdc, ptAni2.x, ptAni2.y, bx, by, hMemDC, xStart, yStart,
	//		bx, by, RGB(34, 34, 34));

	//	SelectObject(hMemDC, hOldBitmap);
	//	DeleteDC(hMemDC);
	//}
}
void DeleteBitmap()
{
	DeleteObject(hBackImage);
	DeleteObject(h8wayImage);
}
void UpdateFrame(HWND hWnd)
{

	curframe++;
	if (curframe > RUN_FRAME_MAX)
		curframe = RUN_FRAME_MIN;
}
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

		StretchBlt(hMemDC, 0, 0,bx*6, by*5, hMemDC2, 0, 0, bx, by, SRCCOPY);

		SelectObject(hMemDC2, hOldBitmap2);
		DeleteDC(hMemDC2);
	}

	Gdi_Draw(hMemDC);

	//8way
	{
		hMemDC2 = CreateCompatibleDC(hMemDC);
		hOldBitmap2 = (HBITMAP)SelectObject(hMemDC2, h8wayImage);

		bx = bit8way.bmWidth / 8;
		by = bit8way.bmHeight / 8;

		int xStart = curframe * bx;
		int yStart = by * bit8wayi;// curframe*by;


		TransparentBlt(hMemDC, ptAni2.x, ptAni2.y, bx/2, by/2, hMemDC2, xStart, yStart,
			bx, by, RGB(34, 34, 34));

		SelectObject(hMemDC2, hOldBitmap2);
		DeleteDC(hMemDC2);
	}


	

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
	SolidBrush brushBlack(Color(150, 0, 0, 0));
	SolidBrush brushTransparent(Color(0, 0, 0, 0));
	FontFamily fontFamily(L"Times New Roman");
	Font font(&fontFamily, 24, FontStyleRegular, UnitPixel);
	PointF pointF(10.0f, 20.0f);
	Pen pen(Color(255, 051, 255, 255));
	pen.SetWidth(5);

	Rect Original = { rectView.left, rectView.top, rectView.right, rectView.bottom };
	Rect base = { rectView.right / 2, rectView.bottom / 2, 100, 100 };
	int rectright = rectView.right / 2;
	int rectleft = rectView.bottom / 2;


	//HRGN CreateRectRgn(rectright, rectleft, 100, 100);
	//HRGN reg1 = CreateRectRgn(1, 2, 3, 4);
	
	Region region(Original);
	Region VisibleArea(base);

	GraphicsPath mypath;


	GraphicsPath path;
	static Region newregion(&path);
	//path.AddLine(50, 150, 150, 180);
	//path.AddLine(150, 180, 200, 240);
	//graphics.DrawPath(&pen, &path);

	//if (VisibleArea.IsVisible(ptAni2.x, ptAni2.y, 0) == 0)
	//{
	//	goBackToArea();
	//}
	static int check;
	 if (mpi - 2 >= 0) {
		 //mypath.AddLine(mp[mpi-1].GetPoint().x, mp[mpi - 1].GetPoint().y,
		 //mp[mpi].GetPoint().x, mp[mpi].GetPoint().y); //mypath에 새좌표 line 추가
		 for (int i = 0; i < mpi - 2; i++)
		 {
   			 mypath.AddLine(mp[i].GetPoint().x, mp[i].GetPoint().y,
				 mp[i + 1].GetPoint().x, mp[i + 1].GetPoint().y);
		 }
	 }

	 path.AddPath(&mypath, true);
	
	// myRegion[myRegioni](Rect(0, 0, 0, 0));

		 if (VisibleArea.IsVisible(ptAni2.x, ptAni2.y, 0) == 1)
		 {
  			 newregion.Complement(&mypath);
			 /*myRegion[myRegioni].Complement(&mypath);
			 myRegioni++;*/
			 check=0;
		 }
		 
	/*	 if(myRegioni>=1)
		 VisibleArea.Union(&myRegion[myRegioni - 1]);*/
		 
		 VisibleArea.Union(&newregion);
	
	//mypath.AddLine(mp[mpi - 2].GetPoint().x, mp[mpi - 2].GetPoint().y,
	//	mp[mpi - 1].GetPoint().x, mp[mpi - 1].GetPoint().y);


	


	graphics.DrawPath(&pen, &path);


	region.Xor(&VisibleArea);

	GraphicsPath Path_test;

	graphics.FillRegion(&brushBlack, &region);

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
void UpdateFrame()
{
	
	if (GetAsyncKeyState(VK_SPACE) & 0x8001)
	{		
   		if (mpi == 0) //처음의 위치 기억
		{
			orgPt.x= ptAni2.x;
			orgPt.y= ptAni2.y;
		}

		mp[mpi].SetPoint({ ptAni2.x, ptAni2.y }); //이동중일 때 위치값 받기
		
		//if(mpi-1>=0)
		//mypath.AddLine(mp[mpi-1].GetPoint().x, mp[mpi - 1].GetPoint().y,
		//	mp[mpi].GetPoint().x, mp[mpi].GetPoint().y); //mypath에 새좌표 line 추가
		mpi++;
	

		//if ()
		//{
		//	if (mp[mpi].GetPoint().x == orgPt.x &&mp[mpi].GetPoint().y == orgPt.y)//처음으로 돌아오면

		//	{
		//	}
		//}
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8001)	//asyncstate
	{
		ptAni2.x += 6;
		bit8wayi = 2;
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8001) //s눌린게 001 안눌린게 000
	{
		ptAni2.x -= 6;
		bit8wayi = 6;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8001)
	{
		ptAni2.y -= 6;
		bit8wayi = 4;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8001)
	{
		ptAni2.y += 6;
		bit8wayi = 0;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8001 && GetAsyncKeyState(VK_LEFT) & 0x8001)
	{
		ptAni2.y -= 2;
		ptAni2.x += 2;
		bit8wayi = 7;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8001 && GetAsyncKeyState(VK_RIGHT) & 0x8001)
	{
		ptAni2.y -= 2;
		ptAni2.x -= 2;
		bit8wayi = 1;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8001 && GetAsyncKeyState(VK_LEFT) & 0x8001)
	{
		ptAni2.y += 2;
		ptAni2.x += 2;
		bit8wayi = 5;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8001 && GetAsyncKeyState(VK_RIGHT) & 0x8001)
	{
		ptAni2.y += 2;
		ptAni2.x -= 2;
		bit8wayi = 3;
	}
}
void DrawBack(HDC hdc)
{}
void goBackToArea()
{
	if (GetAsyncKeyState(VK_RIGHT) & 0x8001)	//asyncstate
	{
		ptAni2.x -= 6;
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8001) //s눌린게 001 안눌린게 000
	{
		ptAni2.x += 6;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8001)
	{
		ptAni2.y += 6;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8001)
	{
		ptAni2.y -= 6;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8001 && GetAsyncKeyState(VK_LEFT) & 0x8001)
	{
		ptAni2.y += 2;
		ptAni2.x -= 2;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8001 && GetAsyncKeyState(VK_RIGHT) & 0x8001)
	{
		ptAni2.y += 2;
		ptAni2.x += 2;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8001 && GetAsyncKeyState(VK_LEFT) & 0x8001)
	{
		ptAni2.y -= 2;
		ptAni2.x -= 2;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8001 && GetAsyncKeyState(VK_RIGHT) & 0x8001)
	{
		ptAni2.y -= 2;
		ptAni2.x += 2;
	}
}