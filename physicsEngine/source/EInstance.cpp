#include "EInstance.h"

LRESULT CALLBACK EInstance::WndProcBridgePhase1(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const createInfo = reinterpret_cast<CREATESTRUCTW*>(lParam);
		EInstance* const ins = static_cast<EInstance*>(createInfo->lpCreateParams);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ins));
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&EInstance::WndProcBridgePhase2ActualBridge));

		return ins->WndProc(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK EInstance::WndProcBridgePhase2ActualBridge(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	EInstance* const ins = reinterpret_cast<EInstance*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return ins->WndProc(hWnd, msg, wParam, lParam);
}

LRESULT EInstance::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_MOUSEMOVE:
	{
		const POINTS mpt = MAKEPOINTS(lParam);
		input.mouseMoved = true;
		input.UpdateMousePosition(mpt.x, mpt.y);
		break;
	}
	
	case WM_CLOSE:
		PostQuitMessage(1);
		break;
		//INPUT------------------------------------------------------------
	case WM_KEYDOWN:
	{
		if (!(lParam & 0x40000000))
		{
			int r = input.GetIndex((char)wParam);
			if (r >= 0)
				input.SetKeyPressed(r);
			input.SetKeyPressed(kANY);
			input.SetKeyPressed(kANYM);
		}
		break;
	}
	case WM_KEYUP:
	{
		int r = input.GetIndex((char)wParam);

		if (r >= 0)
			input.SetKeyReleased(r);

		input.SetKeyReleased(kANY);
		input.SetKeyReleased(kANYM);
		break;
	}
	case WM_SYSKEYDOWN:
	{
		if (!(lParam & 0x40000000))
		{
			int r = -1;
			if ((char)wParam == VK_MENU)
				r = sALT;
			if (r > 0)
			{
				input.SetKeyPressed(r);
				input.SetKeyPressed(kANY);
				input.SetKeyPressed(kANYM);
			}
		}
		break;
	}
	case WM_SYSKEYUP:
	{
		int r = -1;
		if ((char)wParam == VK_MENU)
			r = sALT;
		if (r > 0)
		{
			input.SetKeyReleased(r);
			input.SetKeyReleased(kANY);
			input.SetKeyReleased(kANYM);
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS mpt = MAKEPOINTS(lParam);
		input.UpdateMousePosition(mpt.x, mpt.y);
		input.SetKeyPressed(BL);
		input.SetKeyPressed(kANYM);
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS mpt = MAKEPOINTS(lParam);
		input.UpdateMousePosition(mpt.x, mpt.y);
		input.SetKeyReleased(BL);
		input.SetKeyReleased(kANYM);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS mpt = MAKEPOINTS(lParam);
		input.UpdateMousePosition(mpt.x, mpt.y);
		input.SetKeyPressed(BR);
		input.SetKeyPressed(kANYM);
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS mpt = MAKEPOINTS(lParam);
		input.UpdateMousePosition(mpt.x, mpt.y);
		input.SetKeyReleased(BR);
		input.SetKeyReleased(kANYM);
		break;
	}
	case WM_MBUTTONDOWN:
	{
		const POINTS mpt = MAKEPOINTS(lParam);
		input.UpdateMousePosition(mpt.x, mpt.y);
		input.SetKeyPressed(BM);
		input.SetKeyPressed(kANYM);
		break;
	}
	case WM_MBUTTONUP:
	{
		const POINTS mpt = MAKEPOINTS(lParam);
		input.UpdateMousePosition(mpt.x, mpt.y);
		input.SetKeyReleased(BM);
		input.SetKeyReleased(kANYM);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		input.wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		break;
	}
	//END INPUT-----------------------------------------------------------
	case WM_CHAR:
	{

	}
	break;
	}
	

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


void EInstance::Make(HINSTANCE hInstance, int WindowWidth, int WindowHeight, std::string WindowHeader)
{
	input.InitKeys();
	
	if (WindowHeader.empty())
		windowHeader = StandartWindowHeader;
	else
		windowHeader = WindowHeader;

	wndClass.cbSize = sizeof(wndClass);
	wndClass.style = CS_OWNDC;
	wndClass.lpfnWndProc = WndProcBridgePhase1;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	wndClass.hCursor = nullptr;
	wndClass.hbrBackground = nullptr;
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = ClassName.c_str();
	wndClass.hIconSm = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));;
	RegisterClassEx(&wndClass);
	

	RECT wr;
	wr.left = 200;
	wr.right = WindowWidth + wr.left;
	wr.top = 200;
	wr.bottom = WindowHeight + wr.top;

	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);


	hWnd = CreateWindowEx(
		0,
		ClassName.c_str(),
		windowHeader.c_str(),
		WS_CAPTION | WS_MAXIMIZEBOX | WS_SYSMENU,
		wr.left, wr.top, wr.right - wr.left, wr.bottom - wr.top,
		nullptr,
		nullptr,
		hInstance,
		this
	);


	ShowWindow(hWnd, SW_SHOW);

	dxg.inputManagerPtr = &input;

	dxg.Init(hWnd, WindowWidth, WindowHeight);
	Alive = true;

	
}



int EInstance::HandleWindowMessages()
{
	//0 - work
	//1 - quit
	//2 - problem
	BOOL result;
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			outCode = msg.wParam;
			return 1;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}
	return 0;
}

void EInstance::Interrupt(std::string reason)
{
	dxg.swapTexture.Clear(EPalette::BLUE);
	dxg.DrawText(dxg.screenWidth/2, dxg.screenHeight/2, reason, EPalette::WHITE);
	dxg.ProcessFrame();
	EndSession();
}

void EInstance::EndSession()
{
	Alive = false;
	UnregisterClass(ClassName.c_str(), wndClass.hInstance);
}

