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
}
void cSoftEntityBullet::SetRot(float x, float y, float z, float w) {

}

void cSoftEntityBullet::SetScale(const Vec3& s) {
}
void cSoftEntityBullet::SetScale(float x, float y, float z) {
}

const Vec3& cSoftEntityBullet::GetPos() {
	return Vec3();
}

const Quat& cSoftEntityBullet::GetRot()  {
	return Quat();
}

const Vec3& cSoftEntityBullet::GetScale()  {
	return Vec3();
}