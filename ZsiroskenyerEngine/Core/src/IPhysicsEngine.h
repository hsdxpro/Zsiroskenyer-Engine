// IPhysicsEngine.h By Zs�roskeny�r Team 2013.10.23 22:34
#pragma once

class IPhysicsEngine {
public:
	virtual void simulateWorld() = 0;
	virtual void Release() = 0;
};