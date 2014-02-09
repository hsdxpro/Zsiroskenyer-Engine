////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Scene light interface
////////////////////////////////////////////////////////////////////////////////

#include "../../Core/src/math/vec4.h"
#include "../../Core/src/math/vec3.h"



class IGraphicsLight {
public:
	//virtual ~IGraphicsLight() {}; // should NOT be deleted or released, but RemoveLighted

	// light types
	enum eLightType {
		AMBIENT = 1,
		POINT,
		SPOT,
		DIRECTIONAL,
	};

	// set
	virtual void SetType(eLightType type) = 0;

	virtual void SetColor(const Vec3& color) = 0;
	virtual void SetPosition(const Vec3& pos) = 0;
	virtual void SetDirection(const Vec3& dir) = 0;

	virtual void SetAtten(float atten0, float atten1, float atten2) = 0;
	virtual void SetAtten0(float atten0) = 0;
	virtual void SetAtten1(float atten1) = 0;
	virtual void SetAtten2(float atten2) = 0;

	virtual void SetRange(float range) = 0;
	virtual void SetAngle(float inner, float outer) = 0;
	virtual void SetAngleInner(float inner) = 0;
	virtual void SetAngleOuter(float outer) = 0;
	virtual void SetCutoff(float cutoff) = 0;

	// get
	virtual eLightType GetType() = 0;
	virtual Vec3 GetColor() = 0;
	virtual Vec3 GetPosition() = 0;
	virtual Vec3 GetDirection() = 0;

	virtual float GetAtten0() = 0;
	virtual float GetAtten1() = 0;
	virtual float GetAtten2() = 0;

	virtual float GetRange() = 0;
	virtual float GetAngleInner() = 0;
	virtual float GetAngleOuter() = 0;
	virtual float GetCutoff() = 0;

	// enable/disable
	virtual void Enable(bool val) = 0;
	virtual bool IsEnabled() = 0;
};
