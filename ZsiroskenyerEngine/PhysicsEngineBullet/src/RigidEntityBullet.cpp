#include "RigidEntityBullet.h"

cRigidEntityBullet::cRigidEntityBullet(btRigidBody* body) 
:body(body) {
}

void cRigidEntityBullet::Release() {
}

void cRigidEntityBullet::SetPosition(const Vec3& pos) {
	btTransform trans;
	body->getMotionState()->getWorldTransform(trans);
	trans.setOrigin(btVector3(pos.x, pos.y, pos.z));
	body->setWorldTransform(trans);
}

void cRigidEntityBullet::SetRotation(const Quat& rot) {
	btTransform trans;
	body->getMotionState()->getWorldTransform(trans);
	trans.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
	body->setWorldTransform(trans);
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