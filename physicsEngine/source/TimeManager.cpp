#include "TimeManager.h"

TimeManager::TimeManager()
{
	lastUpdate = std::chrono::steady_clock::now();
	deltaTime = 0;
}

void TimeManager::GetReady()
{
	const std::chrono::steady_clock::time_point oldTime = lastUpdate;
	lastUpdate = std::chrono::steady_clock::now();
	const std::chrono::duration<float> delta = lastUpdate - oldTime;
	deltaTime = delta.count();
}
