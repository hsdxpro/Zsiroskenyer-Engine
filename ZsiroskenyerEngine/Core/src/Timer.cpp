#include "Timer.h"

float cTimer::getDeltaSeconds() {
	static std::chrono::system_clock::time_point last;
	static std::chrono::system_clock::time_point curr;
	static bool firstRun = false;
	if(firstRun) {
		last = std::chrono::system_clock::now();
		curr = last;
		firstRun = false;
	}

	curr = std::chrono::system_clock::now();
	float deltaT = std::chrono::duration_cast<std::chrono::duration<float>>(curr - last).count();
	last = curr;

	return deltaT;
}

double cTimer::getDeltaSecondsPrecise() {
	static std::chrono::system_clock::time_point last;
	static std::chrono::system_clock::time_point curr;
	static bool firstRun = false;
	if(firstRun) {
		last = std::chrono::system_clock::now();
		curr = last;
		firstRun = false;
	}

	curr = std::chrono::system_clock::now();
	double deltaT = std::chrono::duration_cast<std::chrono::duration<double>>(curr - last).count();
	last = curr;

	return deltaT;
}