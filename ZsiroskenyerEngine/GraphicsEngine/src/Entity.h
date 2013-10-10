////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Entity.h
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	A class that represents an entity on the 3D scene.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\..\CommonLib\src\math\math_all.h"


class cInstanceGroup;



class cEntity {
	friend class cSceneManager;
public:
	cEntity();

	Vec3 position;
	Quat rotation;
	Vec3 scale;
	bool isVisible;
private:
	cInstanceGroup* instanceGroup;
};