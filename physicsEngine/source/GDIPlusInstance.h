#pragma once
#include <Windows.h>

struct  GDIPlusInstance
{
public:
	GDIPlusInstance();
	~GDIPlusInstance();
private:
	static ULONG_PTR token;
	static int refCount;
};
