////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/GraphicsEntity.h
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	A class that represents an entity on the 3D scene.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../Core/src/math/math_all.h"

class cInstanceGroup;

class cGraphicsEntity {
	friend class cSceneManager;
public:
	Matrix44 GetWorldMatrix();

	void SetPosition(const Vec3& pos);
	void SetRotation(const Quat& rot);

	cGraphicsEntity();
protected:
	Vec3 position;
	Quat rotation;
	Vec3 scale;
	bool isVisible;

	cInstanceGroup* instanceGroup;
};