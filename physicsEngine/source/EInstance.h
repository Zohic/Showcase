#pragma once

#include <string>
#include <assert.h>
#include "specialHeader.h"

#include "DirectGraphics.h"
#include "TimeManager.h"
#include "EException.h"

#define IDI_ICON1 101



class ELogic;
class EInstance
{
	

	friend ELogic;
	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	static LRESULT CALLBACK WndProcBridgePhase1(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProcBridgePhase2ActualBridge(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	InputManager input;
	TimeManager timer;
	DirectGraphics dxg;

	bool Alive = true;
	const std::string ClassName = "EnergeniusWindow";
	const std::string StandartWindowHeader = "Energenius";
	std::string windowHeader;

	WNDCLASSEX wndClass = { 0 };
	HWND hWnd;
	
	
	void Interrupt(std::string reason);
	void EndSession();
	

	
	int HandleWindowMessages();
	
	EInstance& operator=(EInstance&) = delete;
	EInstance(const EInstance&) = delete;
	EInstance()
	{
		Alive = false;
		outCode = -1;
	}
	
	
public:
	class ECrashHandler : public EException
	{
		friend EInstance;
	public:
		ECrashHandler()
		{
			msg = "empty error";
		}
		ECrashHandler(std::string message) :EException(message)
		{
		}
		ECrashHandler(std::string message, std::string File, int Line):EException(message, File, Line)
		{
		}
		void CallCatch() override
		{
			std::string a;
			a += msg + '\n';
			if (full)
			{
				a += "in " + file + '\n';
				a += "line: " + std::to_string(line) + '\n';
			}
			MessageBox(EInstance::GetInstance().hWnd, a.c_str(), "problem occured", MB_OK | MB_ICONERROR);
		}
	};
	static EInstance& GetInstance()
	{
		static EInstance inst;
		return inst;
	}
	void Make(HINSTANCE hInstance, int WindowWidth, int WindowHeight, std::string WindowHeader = nullptr);
	int outCode;
};

