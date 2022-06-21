#pragma once
#include <chrono>

struct TimeManager
{
	TimeManager();
	std::chrono::steady_clock::time_point lastUpdate;
	void GetReady();
	float deltaTime;
};