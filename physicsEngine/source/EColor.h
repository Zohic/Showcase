#pragma once

struct EColor
{
	unsigned int data;// like r0x00 g0x00 b0x00 a0x00

	// but in directx it is getting like argb so its sets and gets other bytes

	EColor();
	EColor(unsigned char r, unsigned char g, unsigned char b);
	EColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	void SetR(unsigned char v);
	void SetG(unsigned char v);
	void SetB(unsigned char v);
	void SetA(unsigned char v);
	void Set(unsigned char r, unsigned char g, unsigned char b);
	void Set(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	unsigned char gR();
	unsigned char gG();
	unsigned char gB();
	unsigned char gA();
	
};




