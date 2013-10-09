///////////////////////////////////////////////////////////////////////////
// matrix.h
// Matrix class for 3d graphics and stuff
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <cmath>
#include <iostream>

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

union Matrix44;
typedef Matrix44 Matrix;

class Vec3;
class Quat;



///////////////////////////////////////////////////////////////////////////
// matrix functions
	// matrix math stuff
Matrix44& MatrixIdentity(Matrix44& m);
float MatrixDeterminant(const Matrix44& m);
Matrix44& MatrixInverse(const Matrix44& in, Matrix44& out);
Matrix44& MatrixTranspose(const Matrix44& in, Matrix44& out);
Matrix44& MatrixTranspose(Matrix44& m);
	// rotation
Matrix44& MatrixRotationX (Matrix44& out, float angle);
Matrix44& MatrixRotationY (Matrix44& out, float angle);
Matrix44& MatrixRotationZ (Matrix44& out, float angle);
Matrix44& MatrixRotationAxisAngle (Matrix44& out, const Vec3& axis, float angle);
Matrix44& MatrixRotationQuat (Matrix44& out, const Quat&); // typecast ((Matrix44)quaternion) is possibly slower than this shit
	// scale
Matrix44& MatrixScale(Matrix44& out, float scX, float scY, float scZ);
Matrix44& MatrixScale(Matrix44& out, Vec3 scale);
	// translation
Matrix44& MatrixTranslation(Matrix44& out, float vX, float vY, float vZ);
Matrix44& MatrixTranslation(Matrix44& out, Vec3 v);


///////////////////////////////////////////////////////////////////////////
// float4x4 matrix
union Matrix44 {
	friend void MatrixPrint (const Matrix44& m, std::ostream& os=std::cout);




	// data, accessible in two ways
	float m[4][4];
	struct {
		float	_11, _12, _13, _14,
				_21, _22, _23, _24,
				_31, _32, _33, _34,
				_41, _42, _43, _44;
	};


	// constructors
	Matrix44();
	Matrix44(	float _11, float _12, float _13, float _14,
				float _21, float _22, float _23, float _24,
				float _31, float _32, float _33, float _34,
				float _41, float _42, float _43, float _44);


	// element access
	float& operator() (unsigned row, unsigned col);
	const float& operator() (unsigned row, unsigned col) const;


	// arithmetic unary operators
	Matrix44& operator *= (const Matrix44 & m2);
	Matrix44& operator += (const Matrix44 & m2);
	Matrix44& operator -= (const Matrix44 & m2);

	Matrix44& operator *= (const float&);
	Matrix44& operator /= (const float&);

	// arithmetic binary
	Matrix44 operator * (const Matrix44 & m2);
	Matrix44 operator + (const Matrix44 & m2);
	Matrix44 operator - (const Matrix44 & m2);

	Matrix44 operator * (const float&);
	Matrix44 operator / (const float&);


	// comparison 
	bool operator==(const Matrix44& m2) const;
	bool operator!=(const Matrix44& m2) const;
};












///////////////////////////////////////////////////////////////////////////
// utility functions
//void MatrixPrint (const Matrix44& m, std::ostream& os);
std::ostream& operator << (std::ostream& os, const Matrix44& m);










