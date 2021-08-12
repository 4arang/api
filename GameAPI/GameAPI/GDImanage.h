#pragma once
#include "framework.h"
#include <gdiplus.h>
#include <ObjIdl.h>
using namespace std;
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")
class GDImanage
{
	ULONG_PTR g_GdiToken;
public:
	void Update();
	void GDI_Init();
	void GDI_Draw(HDC hdc);
	void GDI_End();
};

