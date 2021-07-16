#include <iostream>
#include <process.h> 
#include <Windows.h>
// createthread 윈도우api용? 학습용? thread가 어떤거다 확인하는용도
// beginthread 주로 사용하는용
//afxbeginthread mfc용

using namespace std;

int Value = 0;
int ValuePlusOne = 0;
int Cnt = 0;
BOOL Check = 0;
CRITICAL_SECTION cs;

DWORD WINAPI ThFunc1(LPVOID lpParam);
DWORD WINAPI ThFunc2(LPVOID lpParam);
unsigned __stdcall ThFunc3(LPVOID lpParam);
unsigned __stdcall ThFunc4(LPVOID lpParam);

int main()
{
	InitializeCriticalSection(&cs); //cs 쓸거야 


	DWORD dwThID1, dwThID2;
	HANDLE hThreads[2];
	unsigned long ulStackSize = 0; //인자 필요해서 넣은거 의미 x
	dwThID1 = 0;
	dwThID2 = 0;
	hThreads[0] = NULL;
	hThreads[1] = NULL;

	//hThreads[0] = CreateThread(NULL, ulStackSize, ThFunc1, NULL, CREATE_SUSPENDED, &dwThID1);
	//hThreads[1] = CreateThread(NULL, ulStackSize, ThFunc2, NULL, CREATE_SUSPENDED, &dwThID2);
	hThreads[0] = (HANDLE)_beginthreadex(NULL, 0, (unsigned(__stdcall *)(void*))ThFunc3,
		NULL, 0, (unsigned*)&dwThID1);
	hThreads[1] = (HANDLE)_beginthreadex(NULL, 0, (unsigned(__stdcall *)(void*))ThFunc4,
		NULL, 0, (unsigned*)&dwThID2);

	if (hThreads[0] == 0 || hThreads[1] == 0)
		exit(1);


	ResumeThread(hThreads[0]);
	ResumeThread(hThreads[1]);

	//ExitThread(dwThID1);
	//DWORD dwExitCode = 0;
	//GetExitCodeThread(hThreads[0], &dwExitCode);
	//if (dwExitCode == STILL_ACTIVE)
	//{
	//	CloseHandle(hThreads[0]);
	//	TerminateThread(hThreads[0], dwExitCode);
	//}

	WaitForMultipleObjects(2, hThreads, TRUE, INFINITE); //thread 정상종료될때까지 기다리는거
	//이코드에서는 작용할 틈을 안줌
	CloseHandle(hThreads[0]);
	CloseHandle(hThreads[1]);


	DeleteCriticalSection(&cs);
}

DWORD WINAPI ThFunc1(LPVOID lpParam)
{
	while (1)
	
	{
		EnterCriticalSection(&cs); //내부에 while문있어야 완벽하게 해결 가능 for exception발생

		Value = rand() % 1000;
		ValuePlusOne = Value + 1;

		LeaveCriticalSection(&cs);
	}
	return 0; //있긴하지만 실질적으로 무한루프
}
DWORD WINAPI ThFunc2(LPVOID lpParam)
{
	while (1)
	{
		EnterCriticalSection(&cs);
		if ((Value + 1) != ValuePlusOne)
			cout << ++Cnt << " -> " << Value << " : " << ValuePlusOne << endl;
		LeaveCriticalSection(&cs);
	}
	return 0;
}

unsigned __stdcall ThFunc3(LPVOID lpParam)
{
	while (1)

	{
		EnterCriticalSection(&cs);

		Value = rand() % 1000;
		ValuePlusOne = Value + 1;

		LeaveCriticalSection(&cs);
	}
	return 0; //있긴하지만 실질적으로 무한루프
}
unsigned __stdcall ThFunc4(LPVOID lpParam)
{
	while (1)
	{
		EnterCriticalSection(&cs);
		if ((Value + 1) != ValuePlusOne)
			cout << ++Cnt << " -> " << Value << " : " << ValuePlusOne << endl;
		LeaveCriticalSection(&cs);
	}
	return 0;
}