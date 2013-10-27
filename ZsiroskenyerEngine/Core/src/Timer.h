// Timer.h By Zsíroskenyér Team 2013.10.27 20:51
#pragma once

#include <chrono>

class cTimer {
public:
	// Get delta seconds since last call
	static float getDeltaSeconds();

	// Get delta seconds since last call (precise)
	static double getDeltaSecondsPrecise();
};