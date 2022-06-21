#include <Windows.h>//for gdi plus, not special header because it is blocking gdi plus
#include "ETexture.h"

#include <gdiplus.h>
#include <sstream>

#pragma comment( lib,"gdiplus.lib" )

ETexture::ETexture()
{
	width = 640;
	height = 480;
	pitch = 640;
	buffer = new EColor[pitch * height];
}
ETexture::ETexture(int iwidth, int iheight)
{
	width = iwidth;
	height = iheight;
	pitch = iwidth;
	buffer = new EColor[pitch * height];
}

void ETexture::Des()
{
	delete [] buffer;
}

void ETexture::Clear(EColor clr)
{
	//memset(buffer, clr.data , (size_t)width * (size_t)height * sizeof(EColor));
	for (int i = 0; i < width * height; i++)
		buffer[i] = clr;
}

void ETexture::CopyFrom(ETexture source)
{
	memcpy(buffer, source.buffer, sizeof(EColor) * width*height);
}


void ETexture::Project(unsigned int dataPitch, unsigned char* const data) const
{
	for (int ny = 0; ny < height; ny++)
		memcpy(&data[dataPitch*ny], &buffer[pitch*ny], sizeof(EColor)*width);
}

void ETexture::PutPixel(int ix, int iy, EColor clr)
{
	if(ix >= 0)
	if (iy >= 0)
	if (ix < width)
	if (iy < height)
	buffer[ix + pitch * iy] = clr;
}

void ETexture::PutPixelAlpha(int ix, int iy, EColor clr)
{
	if(clr.gA() > 0)
	if (ix >= 0)
		if (iy >= 0)
			if (ix < width)
				if (iy < height)
					buffer[ix + pitch * iy] = clr;
}

EColor ETexture::GetPixel(int ix, int iy)
{
	if(ix >= 0);
	if (iy >= 0);
	if (ix < width);
	if (iy < height);
	return buffer[ix + pitch * iy];
}

ETexture ETexture::FromFile(const std::wstring name)
{
	
	Gdiplus::Bitmap bitmap(name.c_str());

	ETexture out = ETexture(bitmap.GetWidth(), bitmap.GetHeight());

	Gdiplus::Color clr;

	for (unsigned int cy = 0; cy < out.height; cy++)
	{
		for (unsigned int cx = 0; cx < out.width; cx++)
		{
			bitmap.GetPixel(cx, cy, &clr);
			out.buffer[cy * out.pitch + cx].data = clr.GetValue();
		}
	}

	return out;
}

int ETexture::LoadFromFile(const std::wstring name)
{
	Gdiplus::Bitmap bitmap(name.c_str());

	int o = bitmap.GetLastStatus();

	if (o != Gdiplus::Status::Ok)
		return o;

	width = bitmap.GetWidth();
	height = bitmap.GetHeight();
	pitch = width;
	buffer = new EColor[width * height];
	Gdiplus::Color clr;

	for (unsigned int cy = 0; cy < height; cy++)
	{
		for (unsigned int cx = 0; cx < width; cx++)
		{
			bitmap.GetPixel(cx, cy, &clr);
			buffer[cy * pitch + cx].data = clr.GetValue();
		}
	}
	return o;
}


