////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Entity.cpp
//	2013.oct.09, Zsiroskenyer Team, P�ter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "GraphicsEntity.h" for more information.
////////////////////////////////////////////////////////////////////////////////

#include "GraphicsEntity.h"
#include "common.h"

cGraphicsEntity::cGraphicsEntity()
:isVisible(true), instanceGroup(NULL) {
}

Matrix44 cGraphicsEntity::GetWorldMatrix() {
	Matrix44 world;
		world.RotationQuat(this->rot);
		world.Scale(this->scale);
		world.Translate(this->pos);
	return world;
}