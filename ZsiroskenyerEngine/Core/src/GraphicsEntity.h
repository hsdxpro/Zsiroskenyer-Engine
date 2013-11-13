////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/GraphicsEntity.h
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	A class that represents an entity on the 3D scene.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "math/math_all.h"
#include "Transform3D.h"
#include "IBaseEntity.h"

class cInstanceGroup;

class cGraphicsEntity : public IBaseEntity {
	friend class cSceneManager;
public:
	cGraphicsEntity();

	void SetPos(float x, float y, float z) override;
	void SetRot(float x, float y, float z, float w) override;
	void SetScale(float x, float y, float z) override;

	const Vec3& GetPos() const override;
	const Quat& GetRot() const override;
	const Vec3& GetScale() const override;
protected:
	bool isVisible;

	cInstanceGroup* instanceGroup;
};