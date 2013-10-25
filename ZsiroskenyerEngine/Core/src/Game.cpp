#include "Game.h"

cGame::cGame(cCamera* c)
:activeCamera(c) {
}

void cGame::SetActiveLevel(cLevel* l) {
	activeLevel = l;
}