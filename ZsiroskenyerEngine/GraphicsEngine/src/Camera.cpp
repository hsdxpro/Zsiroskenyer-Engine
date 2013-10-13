// Camera.cpp implementation
#include "Camera.h"
#include "../../CommonLib/src/common.h"

cCamera::cCamera(float fovRad, float aspectRatio, float nearPlane, float farPlane) 
:fovRad(fovRad), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane) {
}

void cCamera::SetFOV(float fovRad) {
	this->fovRad = fovRad;
}

void cCamera::SetAspectRatio(float aspectRatio) {
	this->aspectRatio = aspectRatio;
}

void cCamera::SetNearPlane(float nearPlane) {
	this->nearPlane = nearPlane;
}

void cCamera::SetFarPlane(float farPlane) {
	this->farPlane = farPlane;
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
	//@TODO fuck dummy bruteforce target
	return Matrix44::MatrixViewRH(pos, Vec3(0.0f, 10.0f, 0.0f), up);
}

Matrix44 cCamera::GetProjMatrix() const {
	return Matrix44::MatrixProjPerspective(nearPlane, farPlane, fovRad);
}