// Camera.cpp implementation
#include "Camera.h"
#include "../../CommonLib/src/common.h"

cCamera::cCamera(float fovRad, float aspectRatio, float nearPlane, float farPlane) 
:fovRad(fovRad), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane) {
}

void cCamera::setFOV(float fovRad) {
	this->fovRad = fovRad;
}

void cCamera::setAspectRatio(float aspectRatio) {
	this->aspectRatio = aspectRatio;
}

void cCamera::setNearPlane(float nearPlane) {
	this->nearPlane = nearPlane;
}

void cCamera::setFarPlane(float farPlane) {
	this->farPlane = farPlane;
}

float cCamera::getFOV() const {
	return fovRad;
}

float cCamera::getAspectRatio() const {
	return aspectRatio;
}

float cCamera::getNearPlane() const {
	return nearPlane;
}

float cCamera::getFarPlane() const {
	return farPlane;
}

Matrix44 cCamera::getViewMatrix() const {
	const Vec3 up(0.0f, 0.0f, 1.0f);
	//@TODO fuck dummy bruteforce target
	return Matrix44::MatrixViewRH(pos, Vec3(0.0f, 10.0f, 0.0f), up);
}

Matrix44 cCamera::getProjMatrix() const {
	return Matrix44::MatrixProjPerspective(nearPlane, farPlane, fovRad);
}