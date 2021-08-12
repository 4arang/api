//#include <Windows.h>
#include "CCore.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	if (!CCore::GetInst()->Init(hInstance)) //초기화 실패한경우
	{
		CCore::DestroyInst(); //코어 메모리 해제
		return 0;
	}

	int iRev = CCore::GetInst()->Run(); //런끝나고 리턴벨류 받아오기
	CCore::DestroyInst();
	return iRev;
}

