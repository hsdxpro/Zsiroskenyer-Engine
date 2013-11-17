// Camera.h By Zsíroskenyér Team 2013.10.13 0:14 last modified Richárd Németh
#pragma once

// Math
#include "math/Vec3.h"
#include "math/Matrix44.h"
#include "math/Quat.h"

class cCamera {
public:
	void SetFOV(float rad);
	void SetAspectRatio(float r);
	void SetNearPlane(float nP);
	void SetFarPlane(float fP);

	void SetPos(const Vec3& p);
	void SetTarget(const Vec3& p);
	void SetDir(const Vec3& p);

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

	const Vec3& GetPos() const;
	const Vec3& GetTarget() const;

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