// dllmain.cpp : DLL 애플리케이션의 진입점을 정의합니다.
#include "pch.h"


int refCount = 0;
int* intPtr = NULL;
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)

{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		if (refCount == 0)
		{
			intPtr = new int[100];
		}
		refCount++;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
		refCount--;
		if (refCount == 0)
			delete intPtr;
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) void dll_TextOut(HDC hdc, char* str)
{
	static TCHAR sText[128];
	wsprintf(sText, TEXT("DLL TestOut"), str);
	TextOut(hdc, 10, 10, sText, lstrlen(sText));
}