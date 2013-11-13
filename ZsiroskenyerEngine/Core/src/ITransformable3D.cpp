#include "ITransformable3D.h"

void ITransformable3D::SetWorldTransform(ITransformable3D* t) {
	SetPos(t->GetPos());
	SetRot(t->GetRot());
	SetScale(t->GetScale());
}

Matrix44 ITransformable3D::GetWorldMatrix() {
	Matrix44 world;
	world.RotationQuat(GetRot());
	world.Scale(GetScale());
	world.Translate(GetPos());
	return world;
}