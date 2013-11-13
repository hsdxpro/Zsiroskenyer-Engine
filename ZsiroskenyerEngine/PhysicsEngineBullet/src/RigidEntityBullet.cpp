#include "RigidEntityBullet.h"

cRigidEntityBullet::cRigidEntityBullet(btRigidBody* body) 
:body(body) {
}

void cRigidEntityBullet::Release() {
	delete this;
}

void cRigidEntityBullet::SetPos(float x, float y, float z) {
	worldTransform.SetPos(x, y, z);

	btTransform trans = body->getWorldTransform();
	trans.setOrigin(btVector3(x, y, z));
	body->setWorldTransform(trans);
}

void cRigidEntityBullet::SetRot(float x, float y, float z, float w) {
	worldTransform.SetRot(x, y, z, w);

	btTransform trans = body->getWorldTransform();
	trans.setRotation(btQuaternion(x, y, z, w));
	body->setWorldTransform(trans);
}

void cRigidEntityBullet::SetScale(float x, float y, float z) {
	worldTransform.SetScale(x, y, z);
}

const Vec3& cRigidEntityBullet::GetPos() const {
	btTransform trans;
	body->getMotionState()->getWorldTransform(trans);
	const btVector3& pos = trans.getOrigin();
	//worldTransform.SetPos(pos.x(), pos.y(), pos.z());
	return worldTransform.GetPos();
}

const Quat& cRigidEntityBullet::GetRot() const { 
	btTransform trans;
	body->getMotionState()->getWorldTransform(trans);
	const btQuaternion& rot = trans.getRotation();
	//worldTransform.SetRot(rot.x(), rot.y(), rot.z(), rot.w());
	return worldTransform.GetRot();
}

const Vec3& cRigidEntityBullet::GetScale() const { 
	return worldTransform.GetScale();
}