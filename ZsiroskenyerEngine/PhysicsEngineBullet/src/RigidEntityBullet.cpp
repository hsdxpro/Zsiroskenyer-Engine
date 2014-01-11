#include "RigidEntityBullet.h"

#include "BulletDynamics\Dynamics\btRigidBody.h"

cRigidEntityBullet::cRigidEntityBullet(btRigidBody* body) 
:body(body) {
}

void cRigidEntityBullet::Release() {
	delete this;
}

void cRigidEntityBullet::SetPos(const Vec3& p) {
	SetPos(p.x, p.y, p.z);
}
void cRigidEntityBullet::SetPos(float x, float y, float z) {
	pos.x = x;
	pos.y = y;
	pos.z = z;

	btTransform trans = body->getWorldTransform();
		trans.setOrigin(btVector3(x, y, z));
		body->setWorldTransform(trans);
	
	// For static objects we need to update motion
	if (body->getInvMass() == 0.0)
		body->getMotionState()->setWorldTransform(trans);
}

void cRigidEntityBullet::SetRot(const Quat& r) {
	SetRot(r.x, r.y, r.z, r.w);
}
void cRigidEntityBullet::SetRot(float x, float y, float z, float w) {
	rot.x = x;
	rot.y = y;
	rot.z = z;
	rot.w = w;

	btTransform trans = body->getWorldTransform();
		trans.setRotation(btQuaternion(x, y, z, w));
		body->setWorldTransform(trans);

	// For static objects we need to update motion
	if (body->getInvMass() == 0.0)
		body->getMotionState()->setWorldTransform(trans);
}

void cRigidEntityBullet::SetScale(const Vec3& s) {
	SetScale(s.x, s.y, s.z);
}
void cRigidEntityBullet::SetScale(float x, float y, float z) {
	scale.x = x;
	scale.y = y;
	scale.z = z;

	btTransform trans = body->getWorldTransform();
		trans.setBasis(trans.getBasis().scaled(btVector3(x, y, z)));
		body->setWorldTransform(trans);

	// For static objects we need to update motion
	if (body->getInvMass() == 0.0)
		body->getMotionState()->setWorldTransform(trans);
}

const Vec3& cRigidEntityBullet::GetPos() const {
	return pos;

	/*
	btTransform trans;
	trans = body->getWorldTransform();
		//body->getMotionState()->getWorldTransform(trans);
		const btVector3& pos = trans.getOrigin();
		worldTransform.SetPos(pos.x(), pos.y(), pos.z());
	return worldTransform.GetPos();
	*/
}

const Quat& cRigidEntityBullet::GetRot() const { 
	btTransform trans;
	body->getMotionState()->getWorldTransform(trans);
	const btQuaternion& btRot = trans.getRotation();

	auto ptr = const_cast<cRigidEntityBullet*>(this);
	ptr->rot.x = btRot.x();
	ptr->rot.y = btRot.y();
	ptr->rot.z = btRot.z();
	ptr->rot.w = btRot.w();

	return rot;
}

const Vec3& cRigidEntityBullet::GetScale() const { 
	return scale;
}