// Camera.h By Zsíroskenyér Team 2013.10.13 0:14 last modified Richárd Németh
#pragma once

#include "../../CommonLib/src/math/Vec3.h"
#include "../../CommonLib/src/math/Matrix44.h"
#include "../../CommonLib/src/math/Quat.h"

class cCamera {
public:
	void SetFOV(float radFOV);
	void SetAspectRatio(float aspectRatio);
	void SetNearPlane(float nearPlane);
	void SetFarPlane(float farPlane);

	float GetFOV() const;
	float GetAspectRatio() const;
	float GetNearPlane() const;
	float GetFarPlane() const;

	Matrix44 GetViewMatrix() const;
	Matrix44 GetProjMatrix() const;


	Vec3 GetDirFront() const;
	Vec3 GetDirBack() const;
	Vec3 GetDirUp() const;
	Vec3 GetDirDown() const;
	Vec3 GetDirRight() const;
	Vec3 GetDirLeft() const;

	cCamera(float radFOV, float aspectRatio, float nearPlane, float farPlane);
private:
	// Pos, Rot
	Vec3 pos;
	Vec3 target;

	// Camera params
	float fovRad;
	float aspectRatio;
	float nearPlane;
	float farPlane;
};