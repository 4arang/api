#include "CCore.h"
#include "Scene\CSceneManager.h"
CCore* CCore::m_pInst = NULL; //static 멤버변수 사용 위한 선언 따로
bool CCore::m_bLoop = true;

CCore::CCore()
{
}

CCore::~CCore()
{

}

bool CCore::Init(HINSTANCE hInst)
{
	m_hInst = hInst;
	MyRegisterClass();

	//해상도 설정
	m_tRS.iW = 1280;
	m_tRS.iH = 720;

	Create();

	return true;
}

int CCore::Run()
{
	MSG msg;

	// 기본 메시지 루프입니다: 게임은 peekmessage에 만들어야 window deadtime 이용
	//메시지 없어도 바로바져나온다?
	while (m_bLoop)
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
		}
	}

	return (int)msg.wParam;
	return 0;
}

BOOL CCore::Create()
{

	m_hWnd = CreateWindowW(L"MyGAME",L"MyGAME", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, m_hInst, nullptr);

	if (!m_hWnd) //윈도우 생성 실패시
	{
		return FALSE;
	}


	RECT rc = { 0,0,m_tRS.iW,m_tRS.iH };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	//위에서 구해준 크기로 윈도우 클라이언트 영역의 크기를 원하는 크기로 맞춰줘야 한다.
	SetWindowPos(m_hWnd, HWND_TOPMOST, 400, 400, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE |
		SWP_NOZORDER);
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return TRUE;
}

LRESULT CCore::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
		switch (message)
		{
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// 메뉴 선택을 구문 분석합니다:
			switch (wmId)
			{

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
			m_bLoop = false;
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	
}

ATOM CCore::MyRegisterClass()
{
	
		WNDCLASSEXW wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = CCore::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = m_hInst;
		wcex.hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_ICON1));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;// MAKEINTRESOURCEW(IDC_SAMPLEGAME);
		wcex.lpszClassName = L"MyGAME";
		wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

		return RegisterClassExW(&wcex);
}
