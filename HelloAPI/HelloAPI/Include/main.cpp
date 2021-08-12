//#include <Windows.h>
#include "CCore.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	if (!CCore::GetInst()->Init(hInstance)) //�ʱ�ȭ �����Ѱ��
	{
		CCore::DestroyInst(); //�ھ� �޸� ����
		return 0;
	}

	int iRev = CCore::GetInst()->Run(); //�������� ���Ϻ��� �޾ƿ���
	CCore::DestroyInst();
	return iRev;
}

