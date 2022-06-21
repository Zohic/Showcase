#pragma once
#include "EInstance.h"

class ELogic
{
	virtual void Setup() = 0;
	virtual void FrameWork() = 0;
	EInstance* instancePtr;
protected:
	
	DirectGraphics* dxg;
	InputManager* input;
	TimeManager* timer;
	
public:
	ELogic();
	void Make(EInstance* instance);
	void Begin();
};

#define CATCHCYCLE catch(EInstance::ECrashHandler e){e.CallCatch();}
#define CATCHCYCLE_R catch(EInstance::ECrashHandler e){e.CallCatch();return;}
