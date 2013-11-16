#include "Timer.h"

float cTimer::getDeltaSeconds() {
	static std::chrono::system_clock::time_point last;
	static std::chrono::system_clock::time_point curr;
	static bool firstRun = true;
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

size_t cTimer::GetFps(float deltaSeconds) {
	// add delta micro time each call
	static float collectedSeconds = 0;
	collectedSeconds += deltaSeconds;
	static size_t frameCount = 0;
	frameCount++;

	static size_t lastFrameCount = 30;

	// reach 1 million micro sec, so 1 second passed, return the frame count that passed
	if (collectedSeconds > 1.0f) {
		size_t result = frameCount;
		lastFrameCount = result;
		collectedSeconds = 0;
		frameCount = 0;
		return result;
	}
	else {
		return lastFrameCount;
	}
}