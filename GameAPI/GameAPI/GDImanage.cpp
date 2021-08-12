#include "GDImanage.h"



void GDImanage::Update()
{
}

void GDImanage::GDI_Init()
{
	GdiplusStartupInput gpsi;
	GdiplusStartup(&g_GdiToken, &gpsi, NULL);
}

void GDImanage::GDI_Draw(HDC hdc)
{
}

void GDImanage::GDI_End()
{
	GdiplusShutdown(g_GdiToken);
}
