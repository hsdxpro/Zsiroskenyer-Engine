#include "Transform3D.h"

cTransform3D::cTransform3D()
:scale(1, 1, 1) {
}

Matrix44 cTransform3D::GetWorldMatrix() const {
	Matrix44 world;
		world.RotationQuat(this->rot);
		world.Scale(this->scale);
		world.Translate(this->pos);
	return world;
}