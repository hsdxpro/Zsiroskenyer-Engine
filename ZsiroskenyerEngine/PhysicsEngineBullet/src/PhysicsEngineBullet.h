// PhysicsEngineBullet.h By Zsíroskenyér Team 2013.10.26 0:53
#pragma once

#include "..\..\Core\src\IPhysicsEngine.h"
#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody\btSoftRigidDynamicsWorld.h"
#include "..\..\Core\src\math\vec3.h"

class cPhysicsEngineBullet : public IPhysicsEngine {
public:
	void simulateWorld(float deltaT) override;
	btRigidBody* ShootBox(const Vec3& camPos,const Vec3& destination);

	cPhysicsEngineBullet();
	void Release() override;
protected:
	//Fizikai világ reprezentálója
	btSoftRigidDynamicsWorld* physicsWorld;
		
	//Fizika (Be,Ki)
	bool physicsEnabled;
};

// DLL accessor
extern "C"
	__declspec(dllexport)
	IPhysicsEngine* CreatePhysicsEngineBullet() {
		return new cPhysicsEngineBullet();
}