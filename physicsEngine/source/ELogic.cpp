#include "ELogic.h"
#define EEXC_FILENAME "ELogic.cpp"



ELogic::ELogic()
{
	instancePtr = nullptr;
	dxg = nullptr;
	input = nullptr;
	timer = nullptr;
}

void ELogic::Make(EInstance* inst)
{
	try
	{	
		instancePtr = inst;
		dxg = &inst->dxg;
		input = &inst->input;
		timer = &inst->timer;

		if(instancePtr == nullptr)
			throw EInstance::ECrashHandler("insatnce set to null", EEXC_FILENAME, 24);
		if (dxg == nullptr)
			throw EInstance::ECrashHandler("dxg set to null", EEXC_FILENAME, 26);
		if (input == nullptr)
			throw EInstance::ECrashHandler("input set to null", EEXC_FILENAME, 28);
		if (timer == nullptr)
			throw EInstance::ECrashHandler("timer set to null", EEXC_FILENAME, 30);
	}CATCHCYCLE_R
	
}

void ELogic::Begin()
{
	Setup();
	while (true)
	{
		if (EInstance::GetInstance().HandleWindowMessages() == 1)
		{
			break;
		}
			
		try
		{
			if (instancePtr == nullptr)
				throw EInstance::ECrashHandler("no insatnce in main loop", EEXC_FILENAME, 46);

			instancePtr->input.ResetKeys();
			instancePtr->timer.GetReady();
			FrameWork();
			instancePtr->dxg.ProcessFrame();
		}CATCHCYCLE_R
	}
	EInstance::GetInstance().EndSession();
}