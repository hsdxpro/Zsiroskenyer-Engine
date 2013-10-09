///////////////////////////////////////////////////
// header for Quaternion class
// quat.h
///////////////////////////////////////////////////
#pragma once

#include <math.h>
#include "vec3.h"

//#include "matrix.h"


// defines ///////////////////////////////////////
#define QUAT_NORM_TOLERANCE 0.00003f


/////////////////////////////////////////////////////////
// forward declare stuff
union Matrix44;


// quaternion class definition ///////////////////
class Quat {
	public:
		// vector components
		float w,x,y,z;

		// constructors
		Quat();
		Quat(float w, float x, float y, float z);
		Quat(const Vec3& v, float angle);

		// operators
		Quat& operator*=(const Quat& q2);		
		const Quat operator~() const;
		Quat operator*(const Quat& q2) const;
		// boolean operators
		bool operator==(const Quat& r) const;
		bool operator!=(const Quat& r) const;

		// methods
		float lenght() const;
		Quat& normalize();

		// friend function
		friend Quat normalize(const Quat& q);
		friend float lenght(const Quat& q);

		// type casting to rotation matrix
		operator Matrix44(); // -> no fucking use this! probably slower than MatrixRotationQuat()
};



// functions related to quaternions //////////////
Vec3 QuatRotateVec3(Vec3 v, Quat q);
Vec3 QuatRotateVec3_2(Vec3 v, Quat q);
Quat& QuatConjugate(Quat& out, const Quat& q);
float QuatDot(const Quat& q1, const Quat& q2);

Quat& QuatRotation(Quat& out, Vec3 axis, float angle);

Quat EulerAnglesToQuat(const Vec3& eulerAngles);
Vec3 QuatToEulerAngles(const Quat& quaternion);




// utility function //////////////////////////////
// print quat
std::ostream& operator<<(std::ostream& os, Quat q);





