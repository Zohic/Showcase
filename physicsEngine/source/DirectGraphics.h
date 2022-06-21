#pragma once

#include <thread>
#include <vector>

#include "specialHeader.h"

#include "ETexture.h"
#include "InputManager.h"
#include "GDIPlusInstance.h"
#include <d3d11.h>
#include "EMath.h"
#include <amp.h>



struct DirectGraphics
{
	~DirectGraphics();
	void Init(HWND hWnd, int Width, int Height);

	InputManager* inputManagerPtr;



	void ProcessFrame();
	void ClearBuffer(float red, float green, float blue);

	void DrawLine(int x1, int y1, int x2, int y2, EColor clr);
	void DrawLine(Vec2d a, Vec2d b, EColor clr);
	void DrawLine(int x1, int y1, int x2, int y2, EColor clr, ETexture target);
	void DrawVector(Vec2d beg, Vec2d vec, EColor clr = EColor(255, 255, 255));

	void DrawCircle(int x, int y, float r, EColor clr);
	void DrawCircle(Vec2d p, float r, EColor clr);
	void FillRect(int x, int y, int w, int h, EColor clr);
	void FillRect(IVec2d p, IVec2d s, EColor clr);


	bool TextureOnScreen(int dx, int dy, ETexture tex);
	bool DrawTexture(int dx, int dy, ETexture tex);
	void ScaleTexture(ETexture* tex, ETexture* res, float scaleX, float scaleY);
	void ScaleTextureAMP(ETexture* tex, ETexture* res, float scaleX, float scaleY);

	void DrawText(int dx, int dy, std::string text, EColor clr);

	int screenWidth;
	int screenHeight;


	//RowPitch is data's width in bytes
	//DepthPitch is size if data in bytes
	D3D11_MAPPED_SUBRESOURCE dxMainTextureBuffer;

	ETexture swapTexture;
	ETexture fontTexture;
	
	std::vector<Concurrency::accelerator> accelerators;
	Concurrency::accelerator mainAcc;
	

private:

	GDIPlusInstance gdiInst;//for loading images

	ID3D11Device* devicePtr = nullptr;
	IDXGISwapChain* swapChainPtr = nullptr;
	ID3D11DeviceContext* deviceContextPtr = nullptr;
	ID3D11RenderTargetView* targetViewPtr = nullptr;

	ID3D11Texture2D* dxMainTexture;

	ID3D11ShaderResourceView* dxShaderResourceView;
	ID3D11PixelShader* pixelShader;
	ID3D11VertexShader* vertexShader;
	ID3D11Buffer* vertexBuffer;
	ID3D11InputLayout* inputLayout;
	ID3D11SamplerState* samplerState;

};

namespace EPalette
{
	static const EColor
		RED{ 255,0,0 },
		GREEN{ 0,255,0 },
		BLUE{ 0,0,255 },
		BLACK{ 0,0,0 },
		WHITE{ 255,255,255 },
		YELLOW{ 255,255,0 },
		MAGENTA{ 255,0,255 },
		CYAN{ 0,255,255 };
}