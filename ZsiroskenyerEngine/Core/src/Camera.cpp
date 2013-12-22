// Camera.cpp implementation
#include "Camera.h"
#include "../../Core/src/common.h"


cCamera::cCamera(cCamera::tProjOrtho proj, float nearPlane, float farPlane)
:nearPlane(nearPlane), farPlane(farPlane), target(0, 1, 0), pos(0, 0, 0), projOrtho(proj), projType(cCamera::ORTHO) {

}

cCamera::cCamera(cCamera::tProjPersp proj, float nearPlane, float farPlane)
:nearPlane(nearPlane), farPlane(farPlane), target(0, 1, 0), pos(0, 0, 0), projPersp(proj), projType(cCamera::PERSP) {

}

void cCamera::SetFOV(float rad) {
	projPersp.fovRad = rad;
}

void cCamera::SetAspectRatio(float r) {
	projPersp.aspectRatio = r;
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
	return projPersp.fovRad;
}

float cCamera::GetAspectRatio() const {
	return projPersp.aspectRatio;
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
	switch (projType)
	{
		case ORTHO:
			return Matrix44::MatrixProjOrtographic(nearPlane, farPlane, projOrtho.left, projOrtho.right, projOrtho.bottom, projOrtho.top);
		case PERSP:
			return Matrix44::MatrixProjPerspective(nearPlane, farPlane, projPersp.fovRad, projPersp.aspectRatio);
	}
	ASSERT_MSG(false, L"OMG wtf camera not ORTHO and not PERSP");
	return Matrix44();
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




///////////-------------- PROJECTION STRUCTS ------------------/////////////
cCamera::tProjOrtho::tProjOrtho(float left, float right, float bottom, float top)
:left(left), right(right), bottom(bottom), top(top) {
}

cCamera::tProjPersp::tProjPersp(float fovRad, float aspectRatio)
:fovRad(fovRad), aspectRatio(aspectRatio) {
}