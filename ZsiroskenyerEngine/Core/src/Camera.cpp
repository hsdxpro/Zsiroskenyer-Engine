// Camera.cpp implementation
#include "Camera.h"
#include "../../Core/src/common.h"

cCamera::cCamera(float fovRad, float aspectRatio, float nearPlane, float farPlane) 
:fovRad(fovRad), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane), target(0, 1, 0), pos(0, 0, 0) {
}

void cCamera::SetFOV(float rad) {
	this->fovRad = rad;
}

void cCamera::SetAspectRatio(float r) {
	aspectRatio = r;
}

void cCamera::SetNearPlane(float nP) {
	nearPlane = nP;
}

void cCamera::SetFarPlane(float fP) {
	farPlane = fP;
}

void cCamera::SetPos(const Vec3& p) {
	pos = p;
}

void cCamera::SetTarget(const Vec3& p) {
	target = p;
}

void cCamera::SetDir(const Vec3& p) {
	target = pos + p;
}

float cCamera::GetFOV() const {
	return fovRad;
}

float cCamera::GetAspectRatio() const {
	return aspectRatio;
}

float cCamera::GetNearPlane() const {
	return nearPlane;
}

float cCamera::GetFarPlane() const {
	return farPlane;
}

Matrix44 cCamera::GetViewMatrix() const {
	const Vec3 up(0.0f, 0.0f, 1.0f);
	return Matrix44::MatrixViewRH(pos, target, up);
}

Matrix44 cCamera::GetProjMatrix() const {
	return Matrix44::MatrixProjPerspective(nearPlane, farPlane, fovRad, aspectRatio);
}

Vec3 cCamera::GetDirFront() const {
	return target - pos;
}

Vec3 cCamera::GetDirBack() const {
	return pos - target;
}

Vec3 cCamera::GetDirUp() const {
	return Vec3::Cross(GetDirFront(), GetDirRight());
}

Vec3 cCamera::GetDirDown() const {
	return Vec3::Cross(GetDirRight(), GetDirFront());
}

Vec3 cCamera::GetDirRight() const {
	return Vec3::Cross(GetDirFront(), Vec3(0.0f, 0.0f, 1.0f));
}

Vec3 cCamera::GetDirLeft() const {
	return Vec3::Cross(Vec3(0.0f, 0.0f, 1.0f), GetDirFront());
}

const Vec3& cCamera::GetPos() const {
	return pos;
}

const Quat& cCamera::GetRot() const {
	const Vec3 up(0, 0, 1);
	return Quat::DirToRot(target - pos, up);
}

const Vec3& cCamera::GetTarget() const {
	return target;
}