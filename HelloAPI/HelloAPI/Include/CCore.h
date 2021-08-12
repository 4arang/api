#pragma once

#include "Game.h"

class CCore
{
private:
	static CCore* m_pInst;
public:
	static CCore* GetInst()
	{
		if (!m_pInst)
			m_pInst = new CCore;
		return m_pInst;
	}
	static void DestroyInst()
	{
		SAFE_DELETE(m_pInst);
	}
private:
	CCore();
	~CCore();
private:
	HINSTANCE m_hInst;
	HWND m_hWnd;
	RESOLUTION m_tRS;
	static bool m_bLoop;
public:
	bool Init(HINSTANCE hInst);
	int Run();
private:
	ATOM MyRegisterClass();
	BOOL Create();
public:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};