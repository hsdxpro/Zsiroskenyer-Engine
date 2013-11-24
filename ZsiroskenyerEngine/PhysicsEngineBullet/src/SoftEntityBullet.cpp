#include "SoftEntityBullet.h"

#include "BulletSoftBody\btSoftBody.h"

cSoftEntityBullet::cSoftEntityBullet(btSoftBody* body) 
:body(body) {
}

void cSoftEntityBullet::Release() {
	delete this;
}

void cSoftEntityBullet::SetPos(const Vec3& p) {
	SetPos(p.x, p.y, p.z);
}
void cSoftEntityBullet::SetPos(float x, float y, float z) {
	worldTransform.SetPos(x, y, z);

	btTransform trans = body->getWorldTransform();
	trans.setOrigin(btVector3(x, y, z));
	body->setWorldTransform(trans);
}

void cSoftEntityBullet::SetRot(const Quat& r) {
	SetRot(r.x, r.y, r.z, r.w);
}
void cSoftEntityBullet::SetRot(float x, float y, float z, float w) {
	worldTransform.SetRot(x, y, z, w);

	btTransform trans = body->getWorldTransform();
	trans.setRotation(btQuaternion(x, y, z, w));
	body->setWorldTransform(trans);
}

void cSoftEntityBullet::SetScale(const Vec3& s) {
	SetScale(s.x, s.y, s.z);
}
void cSoftEntityBullet::SetScale(float x, float y, float z) {
	worldTransform.SetScale(x, y, z);

	btTransform trans = body->getWorldTransform();
	trans.setBasis(trans.getBasis().scaled(btVector3(x, y, z)));
	body->setWorldTransform(trans);
}

const Vec3& cSoftEntityBullet::GetPos() {
	btTransform trans = body->getWorldTransform();
	const btVector3& pos = trans.getOrigin();
	worldTransform.SetPos(pos.x(), pos.y(), pos.z());
	return worldTransform.GetPos();
}

const Quat& cSoftEntityBullet::GetRot()  {
	btTransform trans = body->getWorldTransform();
	const btQuaternion& rot = trans.getRotation();
	worldTransform.SetRot(rot.x(), rot.y(), rot.z(), rot.w());
	return worldTransform.GetRot();
}

const Vec3& cSoftEntityBullet::GetScale()  {
	return worldTransform.GetScale();
}