#include "RigidTypeBullet.h"

cRigidTypeBullet::cRigidTypeBullet(btCollisionShape* colShape, float mass) 
:colShape(colShape), mass(mass) 
{
	// Calc Local inertia only for dynamic objects
	if(mass != 0.0f)
		colShape->calculateLocalInertia(mass, localInertia);
}

void cRigidTypeBullet::Release() {
}

btCollisionShape* cRigidTypeBullet::GetCollisionShape() {
	return colShape;
}

float cRigidTypeBullet::GetMass() const {
	return mass;
}

const btVector3& cRigidTypeBullet::GetLocalInertia() const {
	return localInertia;
}