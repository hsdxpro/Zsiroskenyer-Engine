// Quat.h By Zsíroskenyér Team 2013.10.09 23:52 last modified Németh Richárd
#pragma once

#include <math.h>
#include "vec3.h"

#define QUAT_NORM_TOLERANCE 0.00003f

class Matrix44;
class Quat {
	public:
		float w,x,y,z;

		Quat();
		Quat(float w, float x, float y, float z);
		Quat(const Vec3& v, float angle);

		float lenght() const;
		Quat& normalize();

		static Quat normalize(const Quat& q);
		static float lenght(const Quat& q);

		static Vec3 QuatRotateVec3(Vec3 v, Quat q);
		static Vec3 QuatRotateVec3_2(Vec3 v, Quat q);
		static Quat& QuatConjugate(Quat& out, const Quat& q);
		static float QuatDot(const Quat& q1, const Quat& q2);

		static Quat& QuatRotation(Quat& out, Vec3 axis, float angle);

		static Quat EulerAnglesToQuat(const Vec3& eulerAngles);
		static Vec3 QuatToEulerAngles(const Quat& quaternion);

		Quat& operator*=(const Quat& q2);		
		const Quat operator~() const;
		Quat operator*(const Quat& q2) const;

		bool operator==(const Quat& r) const;
		bool operator!=(const Quat& r) const;

		// type casting to rotation matrix
		operator Matrix44(); // -> no fucking use this! probably slower than MatrixRotationQuat()
};

// Utility
std::ostream& operator<<(std::ostream& os, Quat q);





