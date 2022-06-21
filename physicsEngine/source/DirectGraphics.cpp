#include "DirectGraphics.h"

namespace FramebufferShaders
{
#include "FramebufferPS.shh"
#include "FramebufferVS.shh"
}

#pragma comment(lib,"d3d11.lib")

#include "ELogic.h"
#define EEXC_FILENAME "DirectGraphics.cpp"

DirectGraphics::~DirectGraphics()
{
	if (targetViewPtr != nullptr)
		targetViewPtr->Release();

	if (deviceContextPtr != nullptr)
		deviceContextPtr->Release();

	if (swapChainPtr != nullptr)
		swapChainPtr->Release();

	if (devicePtr != nullptr)
		devicePtr->Release();

	if (dxMainTexture != nullptr)
		dxMainTexture->Release();

	if (dxShaderResourceView != nullptr)
		dxShaderResourceView->Release();

	if (pixelShader != nullptr)
		pixelShader->Release();

	if (vertexShader != nullptr)
		vertexShader->Release();

	if (vertexBuffer != nullptr)
		vertexBuffer->Release();

	if (inputLayout != nullptr)
		inputLayout->Release();

	if (samplerState != nullptr)
		samplerState->Release();
}

void DirectGraphics::Init(HWND hWnd, int Width, int Height)
{
	screenWidth = Width;
	screenHeight = Height;

	swapTexture = ETexture(screenWidth, screenHeight);

	DXGI_SWAP_CHAIN_DESC scDesc = {};
	scDesc.BufferDesc.Width = screenWidth;
	scDesc.BufferDesc.Height = screenHeight;
	scDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scDesc.BufferDesc.RefreshRate.Numerator = 1;
	scDesc.BufferDesc.RefreshRate.Denominator = 120;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = 1;
	scDesc.OutputWindow = hWnd;
	scDesc.Windowed = TRUE;

	D3D_FEATURE_LEVEL	featureLevelsRequested = D3D_FEATURE_LEVEL_9_1;
	UINT				numLevelsRequested = 1;
	D3D_FEATURE_LEVEL	featureLevelsSupported;
	HRESULT				hr;
	UINT				createFlags = 0u;
#ifdef _DEBUG
#ifdef USE_DIRECT3D_DEBUG_RUNTIME
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#endif

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createFlags,
		&featureLevelsRequested,
		numLevelsRequested,
		D3D11_SDK_VERSION,
		&scDesc,
		&swapChainPtr,
		&devicePtr,
		&featureLevelsSupported,
		&deviceContextPtr
	);

	ID3D11Resource* backBuffer = nullptr;
	swapChainPtr->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

	devicePtr->CreateRenderTargetView(backBuffer, nullptr, &targetViewPtr);
	deviceContextPtr->OMSetRenderTargets(1, &targetViewPtr, nullptr);
	//backBuffer->Release();

	D3D11_VIEWPORT vp;
	vp.Width = float(screenWidth);
	vp.Height = float(screenHeight);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	deviceContextPtr->RSSetViewports(1, &vp);

	

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = screenWidth;
	texDesc.Height = screenHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DYNAMIC;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texDesc.MiscFlags = 0;

	devicePtr->CreateTexture2D(&texDesc, nullptr, &dxMainTexture);
	

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	
	devicePtr->CreateShaderResourceView(dxMainTexture, &srvDesc, &dxShaderResourceView);

	

	devicePtr->CreatePixelShader(
		FramebufferShaders::FramebufferPSBytecode,
		sizeof(FramebufferShaders::FramebufferPSBytecode),
		nullptr,
		&pixelShader);

	

	
	devicePtr->CreateVertexShader(
		FramebufferShaders::FramebufferVSBytecode,
		sizeof(FramebufferShaders::FramebufferVSBytecode),
		nullptr,
		&vertexShader);

	struct wow
	{
		float x, y, z;
		float u, v;
	};
	
	const wow vertices[] =
	{//    x      y     z     u      v
		{ -1.0f,  1.0f, 0.5f, 0.0f, 0.0f },
		{  1.0f,  1.0f, 0.5f, 1.0f, 0.0f },
		{  1.0f, -1.0f, 0.5f, 1.0f, 1.0f },
		{ -1.0f,  1.0f, 0.5f, 0.0f, 0.0f },
		{  1.0f, -1.0f, 0.5f, 1.0f, 1.0f },
		{ -1.0f, -1.0f, 0.5f, 0.0f, 1.0f },
	};

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(wow) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;
	devicePtr->CreateBuffer(&bd, &initData, &vertexBuffer);

	

	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	// Ignore the intellisense error "namespace has no member"
	devicePtr->CreateInputLayout(ied, 2,
		FramebufferShaders::FramebufferVSBytecode,
		sizeof(FramebufferShaders::FramebufferVSBytecode),
		&inputLayout);

	

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	devicePtr->CreateSamplerState(&sampDesc, &samplerState);
	
	try
	{
		int fontOK = fontTexture.LoadFromFile(L"font.png");
		if (fontOK != 0)// 0 - ok code for gdiplus
			throw EInstance::ECrashHandler("failed to load font", EEXC_FILENAME, 210);
	}CATCHCYCLE_R

	accelerators = Concurrency::accelerator::get_all();
	mainAcc = Concurrency::accelerator::accelerator();

}



void DirectGraphics::ProcessFrame()
{

	/*deviceContextPtr->Map(dxMainTexture, 0u,
		D3D11_MAP_WRITE_DISCARD, 0u, &dxMainTextureBuffer);

	//RowPitch is data's width in bytes
	//DepthPitch is size if data in bytes

	swapTexture.Project(dxMainTextureBuffer.RowPitch,
		reinterpret_cast<unsigned char*>(dxMainTextureBuffer.pData));

	deviceContextPtr->Unmap(dxMainTexture, 0u);

	// render offscreen scene texture to back buffer
	deviceContextPtr->IASetInputLayout(inputLayout);
	deviceContextPtr->VSSetShader(vertexShader, nullptr, 0u);
	deviceContextPtr->PSSetShader(pixelShader, nullptr, 0u);
	deviceContextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	const UINT stride = sizeof(float[5]);
	const UINT offset = 0u;
	deviceContextPtr->IASetVertexBuffers(0u, 1u, &vertexBuffer, &stride, &offset);
	deviceContextPtr->PSSetShaderResources(0u, 1u, &dxShaderResourceView);
	deviceContextPtr->PSSetSamplers(0u, 1u, &samplerState);
	deviceContextPtr->Draw(6u, 0u);*/
	ID3D11Texture2D* backBuffer = NULL;

	swapChainPtr->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

	//deviceContextPtr->CopyResource(dxMainTexture, backBuffer);

	deviceContextPtr->Map(dxMainTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &dxMainTextureBuffer);

	//if (dxMainTextureBuffer.DepthPitch / dxMainTextureBuffer.RowPitch !=screenHeight || dxMainTextureBuffer.RowPitch/4 != screenWidth)
	//	this->swapTexture.Fill(EColor(255, 255, 255));

	//DrawText(40, 200, std::to_string(dxMainTextureBuffer.DepthPitch/8*2), EColor(255, 255, 255));
	//DrawText(40, 220, std::to_string(screenWidth * screenHeight), EColor(255, 255, 255));

	swapTexture.Project(dxMainTextureBuffer.RowPitch,
		reinterpret_cast<unsigned char*>(dxMainTextureBuffer.pData));


	deviceContextPtr->Unmap(dxMainTexture, 0);
	deviceContextPtr->CopyResource(backBuffer, dxMainTexture);


	/*deviceContextPtr->Map(dxMainTexture, 0u,
		D3D11_MAP_WRITE_DISCARD, 0u, &dxMainTextureBuffer);

	swapTexture.Project(dxMainTextureBuffer.RowPitch,
		reinterpret_cast<unsigned char*>(dxMainTextureBuffer.pData));

	deviceContextPtr->Unmap(dxMainTexture, 0u);
	//deviceContextPtr->Draw(6u, 0u);*/


	swapChainPtr->Present(0u,0u);

}


void DirectGraphics::ClearBuffer(float red, float green, float blue)
{
	//const float clr[] =  {red, green, blue, 1.0f};
	//deviceContextPtr->ClearRenderTargetView(targetViewPtr, clr);
}


void DirectGraphics::DrawLine(int x1, int y1, int x2, int y2, EColor clr)
{
	
	float xDif = (x2 - x1);
	float yDif = (y2 - y1);
	
	float k = yDif / xDif;
	if (fabsf(xDif) >= fabsf(yDif))
	{
		int xsign = (int)(xDif > 0) - (int)(xDif < 0);
		for (int dx = 0; dx < fabsf(xDif); dx++)
		{
			swapTexture.PutPixel(x1 + dx * xsign, y1 + dx * k * xsign, clr);
		}
	}
	else
	{
		int ysign = (int)(yDif > 0) - (int)(yDif < 0);
		for (int dy = 0; dy < fabsf(yDif); dy++)
		{
			swapTexture.PutPixel(x1 + dy * ysign / k, y1 + dy * ysign, clr);
		}
	}
	
}

void DirectGraphics::DrawLine(Vec2d a, Vec2d b, EColor clr)
{
	DrawLine(a.x, a.y, b.x, b.y, clr);
}

void DirectGraphics::DrawLine(int x1, int y1, int x2, int y2, EColor clr, ETexture target)
{

	float xDif = (x2 - x1);
	float yDif = (y2 - y1);

	float k = yDif / xDif;
	if (fabsf(xDif) >= fabsf(yDif))
	{
		int xsign = (int)(xDif > 0) - (int)(xDif < 0);
		for (int dx = 0; dx < fabsf(xDif); dx++)
		{
			target.PutPixel(x1 + dx * xsign, y1 + dx * k * xsign, clr);
		}
	}
	else
	{
		int ysign = (int)(yDif > 0) - (int)(yDif < 0);
		for (int dy = 0; dy < fabsf(yDif); dy++)
		{
			target.PutPixel(x1 + dy * ysign / k, y1 + dy * ysign, clr);
		}
	}

}

void DirectGraphics::DrawVector(Vec2d p, Vec2d v, EColor clr)
{
	DrawLine(p.x, p.y, p.x + v.x, p.y + v.y, clr);


	DrawLine(p.x + v.x, p.y + v.y,
		p.x + v.x + v.GetNormal().Normalized().x * 5,
		p.y + v.y + v.GetNormal().Normalized().y * 5, clr);

	DrawLine(p.x + v.x, p.y + v.y,
		p.x + v.x - v.GetNormal().Normalized().x * 5,
		p.y + v.y - v.GetNormal().Normalized().y * 5, clr);
}

void DirectGraphics::DrawCircle(int x, int y, float r, EColor clr)
{
	for (float dx = 0; dx < floorf(r); dx+=(1-(dx/r)*0.5f))
	{
		
		float dy = sqrtf(r * r - dx * dx);

		swapTexture.PutPixel(x + dx, y + dy, clr);
		swapTexture.PutPixel(x + dx, y - dy, clr);
		swapTexture.PutPixel(x - dx, y + dy, clr);
		swapTexture.PutPixel(x - dx, y - dy, clr);
	}
}

void DirectGraphics::DrawCircle(Vec2d p, float r, EColor clr)
{
	for (float dx = 0; dx < floorf(r); dx += (1 - (dx / r) * 0.5f))
	{

		float dy = sqrtf(r * r - dx * dx);

		swapTexture.PutPixel(p.x + dx, p.y + dy, clr);
		swapTexture.PutPixel(p.x + dx, p.y - dy, clr);
		swapTexture.PutPixel(p.x - dx, p.y + dy, clr);
		swapTexture.PutPixel(p.x - dx, p.y - dy, clr);
	}
}

void DirectGraphics::FillRect(int x, int y, int w, int h, EColor clr)
{
	
	for (int ix = x; ix < x + w; ix++)
		for (int iy = y; iy < y + h; iy++)
			swapTexture.PutPixel(ix, iy, clr);
			//swapTexture.buffer[ix + (iy) * screenWidth] = clr;


}

void DirectGraphics::FillRect(IVec2d p, IVec2d s, EColor clr)
{
	for (int ix = p.x; ix < p.x + s.x; ix++)
		for (int iy = p.y; iy < p.y + s.y; iy++)
			swapTexture.PutPixel(ix, iy, clr);
			
}

bool DirectGraphics::TextureOnScreen(int dx, int dy, ETexture tex)
{
	if (dx + (int)tex.width < 0)
		return false;
	if (dx > (int)swapTexture.width)
		return false;
	if (dy + (int)tex.height < 0)
		return false;
	if (dy > (int)swapTexture.height)
		return false;

	return true;
}

bool DirectGraphics::DrawTexture(int dx, int dy, ETexture tex)
{

	/*for (int ix = 0; ix < tex.width/2; ix++)
		for (int iy = 0; iy < tex.height/2; iy++)
			swapTexture.PutPixel(dx+ix,dy+iy,tex.GetPixel(ix,iy));*/


	if (dx + (int)tex.width < 0 || dx >(int)swapTexture.width || dy + (int)tex.height < 0 || dy >(int)swapTexture.height)
		return false;

	int newDrawX = dx, 
		newDrawY = dy;

	int newBeginX = 0, 
		newBeginY = 0;

	int newWidth  = tex.width,
		newHeight = tex.height;

	bool endXInScreen = dx + (int)tex.width  < (int)swapTexture.width;
	bool endYInScreen = dy + (int)tex.height < (int)swapTexture.height;

	//X PART
	if (dx < 0)
	{
		newDrawX = 0;

		newBeginX = -dx;

		if (endXInScreen)
			newWidth = tex.width + dx;
		else
			newWidth = swapTexture.width;

	}
	else if(!endXInScreen)
	{
		newWidth = swapTexture.width - dx;
	}
	//Y PART
	if (dy < 0)
	{
		newDrawY = 0;

		newBeginY = -dy;

		if (endYInScreen)
			newHeight = tex.height + dy;
		else
			newHeight = swapTexture.height;

	}
	else if (!endYInScreen)
	{
		newHeight = swapTexture.height - dy;
	}




	for (int iy = 0; iy < newHeight; iy++)
		memcpy(&swapTexture.buffer[newDrawX +swapTexture.width * (iy + newDrawY)], &tex.buffer[ newBeginX + (iy+newBeginY) * tex.width], sizeof(EColor) * (newWidth));
	
	return true;
}

void DirectGraphics::ScaleTextureAMP(ETexture* tex, ETexture* res, float scaleX, float scaleY)
{

	Concurrency::accelerator_view accView = mainAcc.get_default_view();

	int newWidth = (int)(tex->width * scaleX);
	int newHeight = (int)(tex->height * scaleY);

	float xMod = 1.0f / scaleX;
	float yMod = 1.0f / scaleY;


	res->Des();

	*res = ETexture(newWidth, newHeight);


	std::vector<int> pixArr;
	std::vector<int> pixArrOrig;

	std::vector<int> indexix;
	for (int i = 0; i < newWidth * newHeight; i++)
		indexix.push_back(i);
	
	for (int i = 0; i < tex->width * tex->height; i++)
		pixArrOrig.push_back(tex->buffer[i].data);

	pixArr.resize(newWidth*newHeight);




	//Concurrency::array_view<int, 1> pixls(newWidth*newHeight, pixArr);
	//Concurrency::array_view<int, 1> pixlsOrig(tex->width * tex->height, pixArrOrig);
	//Concurrency::array_view<int, 1> indx(newWidth * newHeight, indexix);
	//pixls.discard_data();

	for (int ip = 0; ip < newWidth * newHeight; ip++)
	{
		int gx = (ip % newWidth) * xMod;
		int gy = (ip / newWidth) * yMod;

		pixArr[ip] = pixArrOrig[gx + gy * newWidth];
	}
	/*
	Concurrency::parallel_for_each(accView, indx.extent, [=](Concurrency::index<1> idx) restrict(amp) {
		

			int gx = (indx[idx] % newWidth) * xMod;
			int gy = (indx[idx] / newWidth) * yMod;

			pixls[idx] = pixlsOrig[gx+gy*newWidth];

	});*/
	//pixlsOrig.synchronize();
	//pixls.synchronize();
	
	for (int i = 0; i < newWidth * newHeight; i++)
	{
		res->buffer[i].data = pixArr[i];
	}


}

void DirectGraphics::ScaleTexture(ETexture* tex, ETexture* res, float scaleX, float scaleY)
{

	int newWidth = (int)(tex->width * scaleX);
	int newHeight = (int)(tex->height * scaleY);

	float xMod = 1.0f / scaleX;
	float yMod = 1.0f / scaleY;


	res->Des();

	*res = ETexture(newWidth, newHeight);


	if (tex->width == newWidth && tex->height == newHeight)
	{
		res->CopyFrom(*tex);
		return;
	}



	if (tex->width == newWidth)
	{
		yMod = 1.0f / scaleY;
		for (int niy = 0; niy < newHeight; niy++)
		{
			memcpy(&res->buffer[newWidth * (niy)], &tex->buffer[( (int)(niy*yMod) ) *newWidth], sizeof(EColor) * (newWidth));

		}

	}
	else
	{
		for (int nix = 0; nix < newWidth; nix++)
			for (int niy = 0; niy < newHeight; niy++)
			{
				res->PutPixel(nix, niy, tex->GetPixel(nix * xMod, niy * yMod));
			}
	}

}






void DirectGraphics::DrawText(int dx, int dy, std::string text, EColor clr)
{
	
	int tdy = 0;
	int tdx = 0;

	for (int i = 0; i < text.size(); i++)
	{
		int curChar = text[i]-32;

		int tx = curChar%16;
		int ty = curChar/16;

		if (text[i] == '\n')
		{
			tdy += 1;
			tdx = 0;
			continue;
		}
		
		int ofy = -1 + rand() % 3;

		for (int ctx = 0; ctx < 8; ctx++)
			for (int cty = 0; cty < 8; cty++)
			{

				if(fontTexture.GetPixel(tx*8+ctx, ty*8+cty).gR()>0)
					swapTexture.PutPixel(dx + ctx + tdx * 9 , dy + cty+tdy*9, clr);
			}
			
		tdx += 1;
		//memcpy(&swapTexture.buffer[dx+i*16 + swapTexture.width*(dy+c)], &fontTexture.buffer[curChar*16+ fontTexture.width*c], sizeof(EColor) * (16));
	}
}