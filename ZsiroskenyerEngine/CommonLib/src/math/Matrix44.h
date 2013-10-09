// Matrix44.h By Zsíroskenyér Engine 2013.10.09 23:17
// Homogeneous matrix 4x4
#pragma once

#include <cmath>
#include <iostream>
#include "Quat.h"
#include "Vec3.h"

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

	// data, accessible in two ways
	union {
		struct {
			float _11, _12, _13, _14;
			float _21, _22, _23, _24;
			float _31, _32, _33, _34;
			float _41, _42, _43, _44;
		};
		float m[4][4];
	};


	Matrix44();
	Matrix44(	float _11, float _12, float _13, float _14,
				float _21, float _22, float _23, float _24,
				float _31, float _32, float _33, float _34,
				float _41, float _42, float _43, float _44);


	static Matrix44& MatrixIdentity(Matrix44& m);
	static float MatrixDeterminant(const Matrix44& m);
	static Matrix44& MatrixInverse(const Matrix44& in, Matrix44& out);
	static Matrix44& MatrixTranspose(const Matrix44& in, Matrix44& out);
	static Matrix44& MatrixTranspose(Matrix44& m);

	// rotation
	static Matrix44& MatrixRotationX (Matrix44& out, float angle);
	static Matrix44& MatrixRotationY (Matrix44& out, float angle);
	static Matrix44& MatrixRotationZ (Matrix44& out, float angle);
	static Matrix44& MatrixRotationAxisAngle (Matrix44& out, const Vec3& axis, float angle);
	static Matrix44& MatrixRotationQuat (Matrix44& out, const Quat&); // typecast ((Matrix44)quaternion) is possibly slower than this shit

	// scale
	static Matrix44& MatrixScale(Matrix44& out, float scX, float scY, float scZ);
	static Matrix44& MatrixScale(Matrix44& out, Vec3 scale);

	// translation
	static Matrix44& MatrixTranslation(Matrix44& out, float vX, float vY, float vZ);
	static Matrix44& MatrixTranslation(Matrix44& out, Vec3 v);

	// Accessors
	float& operator() (unsigned row, unsigned col);
	const float& operator() (unsigned row, unsigned col) const;

	Matrix44& operator *= (const Matrix44& m2);
	Matrix44& operator += (const Matrix44& m2);
	Matrix44& operator -= (const Matrix44& m2);

	Matrix44& operator *= (const float&);
	Matrix44& operator /= (const float&);

	Matrix44 operator * (const Matrix44& m2);
	Matrix44 operator + (const Matrix44& m2);
	Matrix44 operator - (const Matrix44& m2);

	Matrix44 operator * (const float&);
	Matrix44 operator / (const float&);

	Vec3 operator * (const Vec3& v);

	bool operator==(const Matrix44& m2) const;
	bool operator!=(const Matrix44& m2) const;
};

// Utility
std::ostream& operator << (std::ostream& os, const Matrix44& m);










