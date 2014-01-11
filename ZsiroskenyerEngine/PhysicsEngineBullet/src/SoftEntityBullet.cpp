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
	body->translate(btVector3(x, y, z));
}

void cSoftEntityBullet::SetRot(const Quat& r) {
	// TODO
}
void cSoftEntityBullet::SetRot(float x, float y, float z, float w) {
	// TODO
}

void cSoftEntityBullet::SetScale(const Vec3& s) {
	// TODO
}
void cSoftEntityBullet::SetScale(float x, float y, float z) {
	// TODO
}

const Vec3& cSoftEntityBullet::GetPos() const {
	return Vec3(); // TODO
}

const Quat& cSoftEntityBullet::GetRot() const {
	return Quat(); // TODO
}

const Vec3& cSoftEntityBullet::GetScale() const {
	return Vec3(); // TODO
}