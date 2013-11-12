////////////////////////////////////////////////////////////////////////////////
//	File: GraphicsEngine/src/Entity.cpp
//	2013.oct.09, Zsiroskenyer Team, Péter Kardos
////////////////////////////////////////////////////////////////////////////////
//	See header "GraphicsEntity.h" for more information.
////////////////////////////////////////////////////////////////////////////////

#include "GraphicsEntity.h"
#include "../../Core/src/common.h"

cGraphicsEntity::cGraphicsEntity() 
: 		position(0,0,0),
		rotation(Vec3(0,0,0), 0),
		scale(1,1,1),
		isVisible(true),
		instanceGroup(NULL)
{
}

void cGraphicsEntity::SetPos(const Vec3& pos) {
	position = pos;
}

void cGraphicsEntity::SetRotation(const Quat& rot) {
	rotation = rot;
}

Matrix44 cGraphicsEntity::GetWorldMatrix() {
	Matrix44 world;
		world.RotationQuat(rotation);
		world.Scale(scale);
		world.Translate(position);
	return world;
}