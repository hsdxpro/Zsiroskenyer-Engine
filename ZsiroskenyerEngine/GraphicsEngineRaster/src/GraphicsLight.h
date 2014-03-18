////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Graphics engine's light object
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../Core/src/IGraphicsLight.h"


////////////////////////////////////////////////////////////////////////////////
//	Ligth objects
class cGraphicsLight : public IGraphicsLight {
	friend class cGraphicsEngine;
public:
	// default ctor for default values
	cGraphicsLight();

	// set
	void SetType(eLightType type) override;

	void SetColor(const Vec3& color) override;
	void SetPosition(const Vec3& pos) override;
	void SetDirection(const Vec3& dir) override;

	void SetAtten(float atten0, float atten1, float atten2) override;
	void SetAtten0(float atten0) override;
	void SetAtten1(float atten1) override;
	void SetAtten2(float atten2) override;

	void SetRange(float range) override;
	void SetAngle(float inner, float outer) override;
	void SetAngleInner(float inner) override;
	void SetAngleOuter(float outer) override;
	void SetCutoff(float cutoff) override;

	void SetShadowResolution(unsigned pixels) override;
	void SetShadowQuality(eShadowQuality quality) override;

	// get
	eLightType GetType() const override;
	Vec3 GetColor() const override;
	Vec3 GetPosition() const override;
	Vec3 GetDirection() const override;

	float GetAtten0() const override;
	float GetAtten1() const override;
	float GetAtten2() const override;

	float GetRange() const override;
	float GetAngleInner() const override;
	float GetAngleOuter() const override;
	float GetCutoff() const override;

	unsigned GetShadowResolution() const override;
	eShadowQuality GetShadowQuality() const override;

	// enable/disable
	void Enable(bool val) override;
	bool IsEnabled() const override;
private:
	eLightType type;
	Vec3 color;
	Vec3 position, direction;
	float atten0, atten1, atten2; // attenuation factors
	float range;
	float smallAngle, bigAngle; // spot light cone angle
	float cutoff; // no sharp edges [0,1)
	bool enabled;

	eShadowQuality shadowQuality;
	unsigned shadowResolution;
};