////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Entity.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "GraphicsEntity.h" for more information.
////////////////////////////////////////////////////////////////////////////////

#include "GraphicsEntity.h"
#include "common.h"
#include "../../GraphicsEngineRaster/src/InstanceGroup.h"

cGraphicsEntity::cGraphicsEntity()
:isVisible(true), instanceGroup(NULL) {
}

void cGraphicsEntity::SetPos(const Vec3& p) {
	SetPos(p.x, p.y, p.z);
}
void cGraphicsEntity::SetPos(float x, float y, float z) {
	pos.x = x;
	pos.y = y;
	pos.z = z;
}

void cGraphicsEntity::SetRot(const Quat& r) {
	SetRot(r.x, r.y, r.z, r.w);
}
void cGraphicsEntity::SetRot(float x, float y, float z, float w) {
	rot.x = x;
	rot.y = y;
	rot.z = z;
	rot.w = w;
}

void cGraphicsEntity::SetScale(const Vec3& s) {
	SetScale(s.x, s.y, s.z);
}

void cGraphicsEntity::SetScale(float x, float y, float z) {
	scale.x = x;
	scale.y = y;
	scale.z = z;
}

const Vec3& cGraphicsEntity::GetPos() {
	return pos;
}

const Quat& cGraphicsEntity::GetRot() {
	return rot;
}

const Vec3& cGraphicsEntity::GetScale() {
	return scale;
}

const ITexture2D* cGraphicsEntity::GetTextureDiffuse(size_t subMtlIdx)	const {
	ASSERT(subMtlIdx >= 0 && subMtlIdx < (*instanceGroup->mtl.get()).GetNSubMaterials());
	return (*instanceGroup->mtl.get())[subMtlIdx].textureDiffuse.get();
}

const ITexture2D* cGraphicsEntity::GetTextureNormal(size_t subMtlIdx)	const {
	ASSERT(subMtlIdx >= 0 && subMtlIdx < (*instanceGroup->mtl.get()).GetNSubMaterials());
	return (*instanceGroup->mtl.get())[subMtlIdx].textureNormal.get();
}

const ITexture2D* cGraphicsEntity::GetTextureSpecular(size_t subMtlIdx)	const {
	ASSERT(subMtlIdx >= 0 && subMtlIdx < (*instanceGroup->mtl.get()).GetNSubMaterials());
	return (*instanceGroup->mtl.get())[subMtlIdx].textureSpecular.get();
}

const ITexture2D* cGraphicsEntity::GetTextureDisplace(size_t subMtlIdx)	const {
	ASSERT(subMtlIdx >= 0 && subMtlIdx < (*instanceGroup->mtl.get()).GetNSubMaterials());
	return (*instanceGroup->mtl.get())[subMtlIdx].textureDisplace.get();
}