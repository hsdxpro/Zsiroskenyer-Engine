////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Scene light for the GraphicsEngine.
////////////////////////////////////////////////////////////////////////////////

#include "../../Core/src/math/vec4.h"
#include "../../Core/src/math/vec3.h"


class cGraphicsLight {
public:
	cGraphicsLight() : type(AMBIENT), enabled(true) {}
	enum eLightType {
		AMBIENT = 1,
		POINT,
		SPOT,
		DIRECTIONAL,
	};
	eLightType type;
	Vec3 color;
	Vec3 position, direction;
	float atten0, atten1, atten2; // attenuation factors
	float range;
	float smallAngle, bigAngle; // spot light cone angle
	float cutoff; // no sharp edges [0,1)
	bool enabled;
private:
};