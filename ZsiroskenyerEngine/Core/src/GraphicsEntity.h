////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/GraphicsEntity.h
//	2013.oct.09, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	A class that represents an entity on the 3D scene.
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "math/math_all.h"
#include "Transform3D.h"

class cInstanceGroup;

class cGraphicsEntity : public cTransform3D {
	friend class cSceneManager;
public:
	cGraphicsEntity();

	Matrix44 GetWorldMatrix();
protected:
	bool isVisible;

	cInstanceGroup* instanceGroup;
};