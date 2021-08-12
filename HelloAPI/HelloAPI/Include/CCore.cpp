#include "CCore.h"
#include "Scene\CSceneManager.h"
CCore* CCore::m_pInst = NULL; //static ������� ��� ���� ���� ����
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

	//�ػ� ����
	m_tRS.iW = 1280;
	m_tRS.iH = 720;

	Create();

	return true;
}

int CCore::Run()
{
	MSG msg;

	// �⺻ �޽��� �����Դϴ�: ������ peekmessage�� ������ window deadtime �̿�
	//�޽��� ��� �ٷι������´�?
	while (m_bLoop)
	{
		//Peek�� �޽����� �޼���ť�� ��� �ٷ� �������´�
		 // �޼����� �֟��Ϳ� true ���� ��� fals �޽��� ���½ð��� ���������� ����Ÿ���̴�.
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//������ ����Ÿ���� ���
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

	if (!m_hWnd) //������ ���� ���н�
	{
		return FALSE;
	}


	RECT rc = { 0,0,m_tRS.iW,m_tRS.iH };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	//������ ������ ũ��� ������ Ŭ���̾�Ʈ ������ ũ�⸦ ���ϴ� ũ��� ������� �Ѵ�.
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
			// �޴� ������ ���� �м��մϴ�:
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
			// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�...
			TextOut(hdc, 100, 100, TEXT("win32������"), 8);
			TextOut(hdc, 100, 120, L"win32������", 8);

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
