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

	IPhysicsEntity* CreateRigidEntity(const zsString& physicsGeom, float mass) override;
	IPhysicsEntity* CreateSoftEntity(const zsString& physicsGeom, float mass) override;

	void ShootBox(float size, const Vec3& pos, const Vec3& dir, float power);

	void GetCollisionShapeEdges(Vec3* edges, size_t size , size_t& nEdges) override;

	cPhysicsEngineBullet();
	~cPhysicsEngineBullet();
	void Release() override;
protected:
	//Fizikai világ reprezentálója
	btSoftRigidDynamicsWorld* physicsWorld;

	btSoftBodyWorldInfo	softBodyWorldInfo;

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