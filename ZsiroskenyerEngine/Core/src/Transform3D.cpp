#include "Transform3D.h"

cTransform3D::cTransform3D()
:scale(1, 1, 1) {
}

void cTransform3D::SetPos(const Vec3& p) {
	pos = p;
}
void cTransform3D::SetPos(float x, float y, float z) {
	pos.x = x; pos.y = y; pos.z = z;
}

void cTransform3D::SetRot(const Quat& r) {
	rot = r;
}
void cTransform3D::SetRot(float x, float y, float z, float w) {
	rot.x = x; rot.y = y; rot.z = z; rot.w = w;
}

void cTransform3D::SetScale(const Vec3& s) {
	scale = s;
}
void cTransform3D::SetScale(float x, float y, float z) {
	scale.x = x; scale.y = y; scale.z = z;
}

const Vec3& cTransform3D::GetPos  () const { return pos;  }
const Quat& cTransform3D::GetRot  () const { return rot;  }
const Vec3& cTransform3D::GetScale() const { return scale;}