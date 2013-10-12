// Camera.h By Zsíroskenyér Team 2013.10.13 0:14 last modified Richárd Németh
#pragma once

#include "../../CommonLib/src/math/Vec3.h"
#include "../../CommonLib/src/math/Matrix44.h"
#include "../../CommonLib/src/math/Quat.h"

class cCamera {
public:
	void setFOV(float radFOV);
	void setAspectRatio(float aspectRatio);
	void setNearPlane(float nearPlane);
	void setFarPlane(float farPlane);

	float getFOV() const;
	float getAspectRatio() const;
	float getNearPlane() const;
	float getFarPlane() const;

	Matrix44 getViewMatrix() const;
	Matrix44 getProjMatrix() const;

	cCamera(float radFOV, float aspectRatio, float nearPlane, float farPlane);
private:
	// Pos, Rot
	Vec3 pos;
	Quat rot;

	// Camera params
	float fovRad;
	float aspectRatio;
	float nearPlane;
	float farPlane;
};