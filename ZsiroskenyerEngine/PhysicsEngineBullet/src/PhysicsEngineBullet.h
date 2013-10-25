// PhysicsEngineBullet.h By Zsíroskenyér Team 2013.10.26 0:53
#pragma once

#include "..\..\Core\src\IPhysicsEngine.h"

class cPhysicsEngineBullet : public IPhysicsEngine {
	void simulateWorld() override;
	void Release() override;
};

// DLL accessor
extern "C"
	__declspec(dllexport)
	IPhysicsEngine* CreatePhysicsEngineBullet() {
		return new cPhysicsEngineBullet();
}