// Game.h By Zsíroskenyér Team 2013.10.25 22:44
#pragma once

#include "Camera.h"
#include "Level.h"
#include <vector>

class cGame {
public:
	void SetActiveLevel(cLevel* level);

	cGame(cCamera* cam);
protected:
	cCamera* activeCamera;
	cLevel *activeLevel;
	std::vector<cLevel> levels;
};