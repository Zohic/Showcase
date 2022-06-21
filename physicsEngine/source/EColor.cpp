#include "EColor.h"
EColor::EColor()
{
	//  (255)1 0 0 0
	//       a r g b
	data = 0xFF000000;
}
EColor::EColor(unsigned char r, unsigned char g, unsigned char b)
{
	// not standart order because directx getting colors diffrent way
	//         alpha        red        green   blue
	data = (0xFF << 24) | (r << 16) | (g << 8) | b;
}
EColor::EColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	data = (a << 24) | (r << 16) | (g << 8) | b;
}
void EColor::SetR(unsigned char v)
{
	data = (data & 0xFF00FFFF) | (v << 16);
}
void EColor::SetG(unsigned char v)
{
	data = (data & 0xFFFF00FF) | (v << 8);
}
void EColor::SetB(unsigned char v)
{
	data = (data & 0xFFFFFF00) | (v);
}
void EColor::SetA(unsigned char v)
{
	data = (data & 0x00FFFFFF) | (v << 24);
}
void EColor::Set(unsigned char r, unsigned char g, unsigned char b)
{
	data = (0xFF << 24) | (r << 16) | (g << 8) | b;
}
void EColor::Set(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	data = (a << 24) | (r << 16) | (g << 8) | b;
}
unsigned char EColor::gR()
{
	return (data >> 16) & (0xFF);
}
unsigned char EColor::gG()
{
	return (data >> 8) & (0xFF);
}
unsigned char EColor::gB()
{
	return (data) & (0xFF);
}
unsigned char EColor::gA()
{
	return (data >> 24) & (0xFF);
}
