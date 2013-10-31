// PhysicsEngineBullet.h By Zsíroskenyér Team 2013.10.26 0:53
#pragma once

#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody\btSoftRigidDynamicsWorld.h"

#include "..\..\Core\src\IPhysicsEngine.h"
#include "..\..\Core\src\math\vec3.h"
#include "..\..\Core\src\IPhysicsEntity.h"

#include <map>
#include <vector>


class cPhysicsEngineBullet : public IPhysicsEngine {
public:
	void SimulateWorld(float deltaT) override;

	IPhysicsEntity* GetRigidEntity(const zsString& physicsGeom, float mass) override;
	btRigidBody* ShootBox(const Vec3& camPos,const Vec3& destination);

	void GetCollisionShapeEdges(Vec3* edges, size_t size , size_t& nEdges) override;

	cPhysicsEngineBullet();
	void Release() override;
protected:
	//Fizikai világ reprezentálója
	btSoftRigidDynamicsWorld* physicsWorld;
		
	//Fizika (Be,Ki)
	bool physicsEnabled;

	std::map<zsString, btCollisionShape*> collisionShapes;
};

// DLL accessor
extern "C"
	__declspec(dllexport)
	IPhysicsEngine* CreatePhysicsEngineBullet() {
		return new cPhysicsEngineBullet();
}