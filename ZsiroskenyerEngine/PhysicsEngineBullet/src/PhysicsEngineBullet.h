// PhysicsEngineBullet.h By Zsíroskenyér Team 2013.10.26 0:53
#pragma once

#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody\btSoftRigidDynamicsWorld.h"

#include "..\..\Core\src\IPhysicsEngine.h"
#include "..\..\Core\src\math\vec3.h"

#include <map>

class cPhysicsEngineBullet : public IPhysicsEngine {
public:
	void SimulateWorld(float deltaT) override;

	IPhysicsType* LoadRigidType(const zsString& geomPath, const cGeometryBuilder::tGeometryDesc& desc, float mass) override;
	IPhysicsType* GetRigidType(const zsString& geomPath) override;

	btRigidBody* ShootBox(const Vec3& camPos,const Vec3& destination);

	cPhysicsEngineBullet();
	void Release() override;
protected:
	//Fizikai világ reprezentálója
	btSoftRigidDynamicsWorld* physicsWorld;
		
	//Fizika (Be,Ki)
	bool physicsEnabled;

	std::map<zsString, btCollisionShape*> collisionShapes;
	std::map<zsString, IPhysicsType*> physicsTypes;
};

// DLL accessor
extern "C"
	__declspec(dllexport)
	IPhysicsEngine* CreatePhysicsEngineBullet() {
		return new cPhysicsEngineBullet();
}