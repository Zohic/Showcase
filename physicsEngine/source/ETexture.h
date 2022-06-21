#pragma once
#include "EColor.h"
#include <memory>
#include <sstream>

struct ETexture
{
	ETexture();
	ETexture(int iwidth, int iheight);
	void Des();
	

	EColor* buffer;
	unsigned int width;
	unsigned int height;
	unsigned int pitch;//like data's RowPitch, but in units(pixels)
	
	void Clear(EColor clr);
	void CopyFrom(ETexture source);
	void Project(unsigned int dataPitch, unsigned char* const data) const;
	void PutPixel(int ix, int iy, EColor clr);
	void PutPixelAlpha(int ix, int iy, EColor clr);
	EColor GetPixel(int ix, int iy);
	static ETexture FromFile(const std::wstring name);
	int LoadFromFile(const std::wstring name);

};