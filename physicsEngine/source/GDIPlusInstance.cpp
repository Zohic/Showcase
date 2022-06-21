#include "GDIPlusInstance.h"
#include <gdiplus.h>

ULONG_PTR GDIPlusInstance::token = 0;
int GDIPlusInstance::refCount = 0;

GDIPlusInstance::GDIPlusInstance()
{
	if (refCount++ == 0)
	{
		Gdiplus::GdiplusStartupInput input;
		input.GdiplusVersion = 1;
		input.DebugEventCallback = nullptr;
		input.SuppressBackgroundThread = false;
		Gdiplus::GdiplusStartup(&token, &input, nullptr);
	}
}

GDIPlusInstance::~GDIPlusInstance()
{
	if (--refCount == 0)
	{
		Gdiplus::GdiplusShutdown(token);
	}
}
