////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Entity.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "GraphicsEntity.h" for more information.
////////////////////////////////////////////////////////////////////////////////

#include "GraphicsEntity.h"
#include "common.h"

cGraphicsEntity::cGraphicsEntity()
:isVisible(true), instanceGroup(NULL) {
}

Matrix44 cGraphicsEntity::GetWorldMatrix() const {
	return worldTransform.GetWorldMatrix(); 
}

void cGraphicsEntity::SetPos(const Vec3& p) { 
	SetPos(p.x, p.y, p.z); 
}
void cGraphicsEntity::SetPos(float x, float y, float z) {
	worldTransform.SetPos(x, y, z); 
}

void cGraphicsEntity::SetRot(const Quat& r) {
	SetRot(r.x, r.y, r.z, r.w); 
}
void cGraphicsEntity::SetRot(float x, float y, float z, float w) {
	worldTransform.SetRot(x, y, z, w); 
}

void cGraphicsEntity::SetScale(const Vec3& s) {
	SetScale(s.x, s.y, s.z); 
}
void cGraphicsEntity::SetScale(float x, float y, float z) {
	worldTransform.SetScale(x, y, z); 
}

const Vec3& cGraphicsEntity::GetPos() const {
	return worldTransform.GetPos();
}

const Quat& cGraphicsEntity::GetRot() const {
	return worldTransform.GetRot(); 
}

const Vec3& cGraphicsEntity::GetScale() const {
	return worldTransform.GetScale();
}

const cTransform3D& cGraphicsEntity::GetWorldTransform() const { 
	return worldTransform;
}