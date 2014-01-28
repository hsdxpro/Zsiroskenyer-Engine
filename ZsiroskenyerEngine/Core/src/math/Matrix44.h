// Matrix44.h By Zsíroskenyér Engine 2013.10.09 23:17
// Homogeneous matrix 4x4
#pragma once

#include <cmath>
#include <iostream>
#include "Quat.h"
#include "Vec3.h"
#include "Vec4.h"

/* what a matrix has to do? 
(-decompose: get scale, rot, and translation from a matrix)
-determinant - IMPLEMENTED
-inverse - IMPLEMENTED (optimization tests maybe)
-multiply
(-reflect: mirrors coordinate system about a plane)
-rotation matrices: from quat, around x,y,z axis by w angle, optionally w angles around a random axis (it's what quats do)
-scaling matrix
-translation matrix
-transpose

useful combinations of the above:
-full transformation, mTransformation = mScale*mRot*mTranslation
*/

class Matrix44 {
public:
	// Data, accessible multiple ways
	union {
		struct {
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		struct {
			Vec4 r[4];
		};
		float m[4][4];
	};

	// Ctors
	Matrix44();
	Matrix44(	float _11, float _12, float _13, float _14,
				float _21, float _22, float _23, float _24,
				float _31, float _32, float _33, float _34,
				float _41, float _42, float _43, float _44);

	// Matrix stuff
	Matrix44& Identity();
	Matrix44& Inverse();
	Matrix44& Transpose();
	float Determinant();

	// Static matrix stuff
	static Matrix44 Inverse(const Matrix44& in);
	static Matrix44 Transpose(const Matrix44& in);

	// Linear transformations
	Matrix44& Scale(float x, float y, float z);
	Matrix44& Scale(const Vec3& s);

	Matrix44& Translate(float x, float y, float z);
	Matrix44& Translate(const Vec3& t);

	static Matrix44 RotationAxisAngle(const Vec3& axis, float angle);
	static Matrix44 RotationQuat(const Quat&); // typecast ((Matrix44)quaternion) is possibly slower than this shit

	static Matrix44 Translation(float x, float y, float z);
	static Matrix44 Translation(const Vec3& v);

	static Matrix44 RotationEuler(const Vec3& rot);
	static Matrix44 RotationEuler(float x, float y, float z);
 
	static Matrix44 RotationX(float angle);
	static Matrix44 RotationY(float angle);
	static Matrix44 RotationZ(float angle);

	static Matrix44 ViewRH(const Vec3& eye, const Vec3& target, const Vec3& up);
	static Matrix44 ProjPerspective(float nearPlane, float farPlane, float fovRad, float aspectRatio);
	static Matrix44 ProjOrtographic(float nearPlane, float farPlane, float left, float right, float bottom, float top);

	// Accessors
	float& operator() (unsigned row, unsigned col);
	const float& operator() (unsigned row, unsigned col) const;
	
	float& operator[] (unsigned index);
	const float& operator[] (unsigned index) const;

	void SetColumn(size_t idx, const Vec4& v);

	// Operators
	Matrix44& operator *= (const Matrix44& m2);
	Matrix44& operator += (const Matrix44& m2);
	Matrix44& operator -= (const Matrix44& m2);

	Matrix44& operator *= (const float&);
	Matrix44& operator /= (const float&);

	Matrix44 operator * (const Matrix44& m2) const;
	Matrix44 operator + (const Matrix44& m2) const;
	Matrix44 operator - (const Matrix44& m2) const;

	Matrix44 operator * (const float&) const;
	Matrix44 operator / (const float&) const;

	// Apply linear transformation
	Vec3 operator * (const Vec3& v) const;

	// Compare matrices
	bool operator==(const Matrix44& m2) const;
	bool operator!=(const Matrix44& m2) const;
};

// Utility
std::ostream& operator << (std::ostream& os, const Matrix44& m);


Matrix44 lerp(const Matrix44& m1, const Matrix44& m2, float t);







