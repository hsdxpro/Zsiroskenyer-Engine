////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/GraphicsEntity.h
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	A class that represents an entity on the 3D scene.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "math/math_all.h"
#include "Transform3D.h"

class cInstanceGroup;

class cGraphicsEntity {
	friend class cSceneManager;
public:
	cGraphicsEntity();

	void SetPos(const Vec3& p);
	void SetPos(float x, float y, float z);

	void SetRot(const Quat& r);
	void SetRot(float x, float y, float z, float w);

	void SetScale(const Vec3& s);
	void SetScale(float x, float y, float z);

	Matrix44 GetWorldMatrix() const;

	const cTransform3D& GetWorldTransform() const ;

	const Vec3& GetPos() const;
	const Quat& GetRot() const;
	const Vec3& GetScale() const;

protected:
	cTransform3D worldTransform;

	bool isVisible;

	cInstanceGroup* instanceGroup;
};