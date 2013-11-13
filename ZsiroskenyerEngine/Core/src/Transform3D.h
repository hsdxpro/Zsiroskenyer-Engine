// Transform3D.h By Zsíroskenyér Team 2013.11.12 19:59
// Yap now we need one common thing between 3D things :D
#pragma once

#include "math/vec3.h"
#include "math/quat.h"
#include "math/Matrix44.h"

class cTransform3D {
public:
	cTransform3D();

	void SetPos(const Vec3& p);
	void SetPos(float x, float y, float z);
	
	void SetRot(const Quat& r);
	void SetRot(float x, float y, float z, float w);
	
	void SetScale(const Vec3& s);
	void SetScale(float x, float y, float z);
	
	Matrix44 GetWorldMatrix() const;
	
	const Vec3& GetPos();
	const Quat& GetRot();
	const Vec3& GetScale();

protected:
	Vec3 pos;
	Quat rot;
	Vec3 scale;
};