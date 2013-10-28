#include "RigidEntityBullet.h"

cRigidEntityBullet::cRigidEntityBullet(btRigidBody* body) 
:body(body) {
}

void cRigidEntityBullet::Release() {
}

void cRigidEntityBullet::SetPosition(const Vec3& pos) {
	// get Trans, modify, apply
}

void cRigidEntityBullet::SetRotation(const Quat& rot) {
	// get Trans, modify, apply
}

Vec3 cRigidEntityBullet::GetPosition() {
	btTransform trans;
		body->getMotionState()->getWorldTransform(trans);
		const btVector3& pos = trans.getOrigin();
	return Vec3(pos.x(), pos.y(), pos.z());
}

Quat cRigidEntityBullet::GetRotation()  {
	btTransform trans;
		body->getMotionState()->getWorldTransform(trans);
		const btQuaternion& rot = trans.getRotation();
	return Quat(rot.x(), rot.y(), rot.z(), rot.w());
}