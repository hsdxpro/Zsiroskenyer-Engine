// IbaseEntity.h By Zsíroskenyér Team 2013.11.13
#pragma once

#include "math/Matrix44.h"
#include "math/vec3.h"
#include "math/quat.h"
#include "Transform3D.h"

class IBaseEntity {
public:
	virtual void SetPos(float x, float y, float z) = 0;
	virtual void SetRot(float x, float y, float z, float w) = 0;
	virtual void SetScale(float x, float y, float z) = 0;

	inline void SetPos(const Vec3& p);
	inline void SetRot(const Quat& r);
	inline void SetScale(const Vec3& s);
	inline Matrix44 GetWorldMatrix() const;

	virtual const Vec3& GetPos() const = 0;
	virtual const Quat& GetRot() const = 0;
	virtual const Vec3& GetScale() const = 0;

protected:
	cTransform3D worldTransform;
};

inline Matrix44 IBaseEntity::GetWorldMatrix() const {
	Matrix44 world;
	world.RotationQuat(GetRot());
	world.Scale(GetScale());
	world.Translate(GetPos());
	return world;
}

inline void IBaseEntity::SetPos(const Vec3& p) {
	SetPos(p.x, p.y, p.z); 
}

inline void IBaseEntity::SetRot(const Quat& r) {
	SetRot(r.x, r.y, r.z, r.w); 
}

inline void IBaseEntity::SetScale(const Vec3& s) {
	SetScale(s.x, s.y, s.z); 
}