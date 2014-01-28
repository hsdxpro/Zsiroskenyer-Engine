////////////////////////////////////////////////////////////////////////////////
// matrix.cpp
// definitions for matrix-stuff
////////////////////////////////////////////////////////////////////////////////
// history:
// 2012.oct.8, monday, 17:57 -> all transform matrices changed to their transpose
//			REASON:							  |a b c d|
//				this is used now:	[x,y,z,1] |e f g h|
//					float1x4				  |i j k l|
//											  |m n o p|
//				this was used:	|x| |a b c d|
//					float4x1	|y| |e f g h|
//								|z| |i j k l|
//								|1| |m n o p|
//			CONSEQUENCE: scale, rotx/y/z, translate should work
//						 quat and axis-angle must be tested
//
// 2012.oct.11, 9:19 -> axis angle matrix rotation doesn't work, fix it

#include "Matrix44.h"
#include "Quat.h"
#include "Vec3.h"
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Constructor
Matrix44::Matrix44() 
{
	this->_11 = 1.0f; this->_12 = 0.0f; this->_13 = 0.0f; this->_14 = 0.0f;
	this->_21 = 0.0f; this->_22 = 1.0f; this->_23 = 0.0f; this->_24 = 0.0f;
	this->_31 = 0.0f; this->_32 = 0.0f; this->_33 = 1.0f; this->_34 = 0.0f;
	this->_41 = 0.0f; this->_42 = 0.0f; this->_43 = 0.0f; this->_44 = 1.0f;
}

Matrix44::Matrix44(	float _11, float _12, float _13, float _14,
					float _21, float _22, float _23, float _24,
					float _31, float _32, float _33, float _34,
					float _41, float _42, float _43, float _44)
{
	this->_11 = _11; this->_12 = (_12); this->_13 = (_13); this->_14 = (_14);
	this->_21 = _21; this->_22 = (_22); this->_23 = (_23); this->_24 = (_24);
	this->_31 = _31; this->_32 = (_32); this->_33 = (_33); this->_34 = (_34);
	this->_41 = _41; this->_42 = (_42); this->_43 = (_43); this->_44 = (_44);
}

////////////////////////////////////////////////////////////////////////////////
// Accessors
float& Matrix44::operator() (unsigned row, unsigned col) {
	return m[row][col];
}

const float& Matrix44::operator() (unsigned row, unsigned col) const {
	return m[row][col];
};

float& Matrix44::operator[] (unsigned index) {
	return *(((float*)this) + index);
}

const float& Matrix44::operator[] (unsigned index) const {
	return *(((float*)this)+ index);
}

void Matrix44::SetColumn(size_t idx, const Vec4& v) {
	for (size_t i = 0; i < 4; i++) {
		this->m[i][idx] = v[i];
	}
}

////////////////////////////////////////////////////////////////////////////////
// Operators
Matrix44& Matrix44::operator *= (const Matrix44 & m2) {
	*this = Matrix44(	m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0],		m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1]  + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1],		m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2]  + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2],		m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3]  + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3],
						m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0],		m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1]  + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1],		m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2]  + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2],		m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3]  + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3],
						m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0],		m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1]  + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1],		m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2]  + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2],		m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3]  + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3],
						m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0],		m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1]  + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1],		m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2]  + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2],		m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3]  + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3] );
	return *this;
};

Matrix44& Matrix44::operator += (const Matrix44 & m2) {
	_11 += m2._11;	_12 += m2._12;	_13 += m2._13;	_14 += m2._14;
	_21 += m2._21;	_22 += m2._22;	_23 += m2._23;	_24 += m2._24;
	_31 += m2._31;	_32 += m2._32;	_33 += m2._33;	_34 += m2._34;
	_41 += m2._41;	_42 += m2._42;	_43 += m2._43;	_44 += m2._44;
	return *this;
}

Matrix44& Matrix44::operator -= (const Matrix44 & m2) {
	_11 -= m2._11;	_12 -= m2._12;	_13 -= m2._13;	_14 -= m2._14;
	_21 -= m2._21;	_22 -= m2._22;	_23 -= m2._23;	_24 -= m2._24;
	_31 -= m2._31;	_32 -= m2._32;	_33 -= m2._33;	_34 -= m2._34;
	_41 -= m2._41;	_42 -= m2._42;	_43 -= m2._43;	_44 -= m2._44;
	return *this;
}

Matrix44& Matrix44::operator *= (const float& f) {
	float* p = (float*)m;
	for (int i=0; i<16; i++) {
		p[i] *= f;
	}
	return *this;
}

Matrix44& Matrix44::operator /= (const float& f) {
	float* p = (float*)m;
	for (int i=0; i<16; i++) {
		p[i] /= f;
	}
	return *this;
}

Matrix44 Matrix44::operator * (const Matrix44 & m2) const {
	return Matrix44(	m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0],		m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1]  + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1],		m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2]  + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2],		m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3]  + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3],
						m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0],		m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1]  + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1],		m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2]  + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2],		m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3]  + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3],
						m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0],		m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1]  + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1],		m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2]  + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2],		m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3]  + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3],
						m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0],		m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1]  + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1],		m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2]  + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2],		m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3]  + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3] );
}

Matrix44 Matrix44::operator + (const Matrix44 & m2) const {
	Matrix44 m = *this;
	m += m2;
	return m;
}

Matrix44 Matrix44::operator - (const Matrix44 & m2) const {
	Matrix44 m = *this;
	m -= m2;
	return m;
}

Matrix44 Matrix44::operator * (const float& f) const {
	Matrix44 m = *this;
	m *= f;
	return m;
}

Matrix44 Matrix44::operator / (const float& f) const {
	Matrix44 m = *this;
	m /= f;
	return m;
}

////////////////////////////////////////////////////////////////////////////////
// Compare
bool Matrix44::operator == (const Matrix44& m2) const {
	return memcmp(this, &m2, sizeof(Matrix44)) == 0;
}

bool Matrix44::operator != (const Matrix44& m2) const {
	return !(*this == m2);
}

////////////////////////////////////////////////////////////////////////////////
// Matrix stuff
Matrix44& Matrix44::Identity() {
	_11 = 1; _12 = 0; _13 = 0; _14 = 0;
	_21 = 0; _22 = 1; _23 = 0; _24 = 0;
	_31 = 0; _32 = 0; _33 = 1; _34 = 0;
	_41 = 0; _42 = 0; _43 = 0; _44 = 1;
	return *this;
}

Matrix44& Matrix44::Inverse() {
	*this = Matrix44::Inverse(*this);
	return *this;
}

Matrix44& Matrix44::Transpose() {
	float t;
	t = _12;	_12 = _21;	_21 = t;
	t = _13;	_13 = _31;	_31 = t;
	t = _14;	_14 = _41;	_41 = t;

	t = _24;	_24 = _42;	_42 = t;
	t = _23;	_23 = _32;	_32 = t;
	t = _34;	_34 = _43;	_43 = t;

	return *this;
};

float Matrix44::Determinant() {
	return
		_11 * (_22 * (_33 * _44 - _43 * _34) - _23 * (_32 * _44 - _42 * _34) + _24 * (_32 * _43 - _42 * _33))
		- _12 * (_21 * (_33 * _44 - _43 * _34) - _23 * (_31 * _44 - _41 * _34) + _24 * (_31 * _43 - _41 * _33))
		+ _13 * (_21 * (_32 * _44 - _42 * _34) - _22 * (_31 * _44 - _41 * _34) + _24 * (_31 * _42 - _41 * _32))
		- _14 * (_21 * (_32 * _43 - _42 * _33) - _22 * (_31 * _43 - _41 * _33) + _23 * (_31 * _42 - _32 * _41));
}

// Static matrix stuff
Matrix44 Matrix44::Transpose(const Matrix44& in) {
	Matrix44 out = in;
	out.Transpose();
	return out;
}

Matrix44 Matrix44::Inverse(const Matrix44& in) {
	Matrix44 out;
	// determinant
	float det_A;
	// partial computations for speed optimizations (hopefully faster, should be tested)
	float 
		A = in._33 * in._44 - in._43 * in._34,
		B = in._32 * in._44 - in._42 * in._34,
		C = in._32 * in._43 - in._42 * in._33,
		D = in._31 * in._44 - in._41 * in._34,
		E = in._31 * in._43 - in._41 * in._33,
		F = in._31 * in._42 - in._41 * in._32,


		G = in._23 * in._44 - in._43 * in._24,
		H = in._22 * in._44 - in._42 * in._24,
		I = in._22 * in._43 - in._42 * in._23,
		J = in._21 * in._44 - in._41 * in._24,
		K = in._21 * in._43 - in._41 * in._23,
		L = in._21 * in._42 - in._41 * in._22,


		M = in._23 * in._34 - in._33 * in._24,
		N = in._22 * in._34 - in._32 * in._24,
		O = in._22 * in._33 - in._32 * in._23,
		P = in._21 * in._34 - in._31 * in._24,
		Q = in._21 * in._33 - in._31 * in._23,
		R = in._21 * in._32 - in._31 * in._22;

	// A; B; C; D; E; F
	out._11 = (in._22*A - in._23*B + in._24*C);
	out._21 = -(in._21*A - in._23*D + in._24*E);
	out._31 = (in._21*B - in._22*D + in._24*F);
	out._41 = -(in._21*C - in._22*E + in._23*F);

	det_A = 1.0f / (in._11*out._11 + in._12 * out._21 + in._13 * out._31 + in._14 * out._41);

	out._12 = -(in._12*A - in._13*B + in._14*C) * det_A;
	out._22 = (in._11*A - in._13*D + in._14*E) * det_A;
	out._32 = -(in._11*B - in._12*D + in._14*F) * det_A;
	out._42 = (in._11*C - in._12*E + in._13*F) * det_A;

	// G, H, I, J, K, L
	out._13 = (in._12*G - in._13*H + in._14*I) * det_A;
	out._23 = -(in._11*G - in._13*J + in._14*K) * det_A;
	out._33 = (in._11*H - in._12*J + in._14*L) * det_A;
	out._43 = -(in._11*I - in._12*K + in._13*L) * det_A;

	// M, N, O, P, Q, R
	out._14 = -(in._12*M - in._13*N + in._14*O) * det_A;
	out._24 = (in._11*M - in._13*P + in._14*Q) * det_A;
	out._34 = -(in._11*N - in._12*P + in._14*R) * det_A;
	out._44 = (in._11*O - in._12*Q + in._13*R) * det_A;

	// post multiplication by det_A, cause det_a was not available when these were computed
	out._11 *= det_A;
	out._21 *= det_A;
	out._31 *= det_A;
	out._41	*= det_A;

	return out;
}

////////////////////////////////////////////////////////////////////////////////
// Linear transformations
Matrix44& Matrix44::Scale(float x, float y, float z) {
	_11 *= x;
	_22 *= y;
	_33 *= z;
	return *this;
}

Matrix44& Matrix44::Scale(const Vec3& s) {
	_11 *= s.x;
	_22 *= s.y;
	_33 *= s.z;
	return *this;
}

Matrix44& Matrix44::Translate(float x, float y, float z) {
	_41 += x;
	_42 += y;
	_43 += z;
	return *this;
}

Matrix44& Matrix44::Translate(const Vec3& t) {
	_41 += t.x;
	_42 += t.y;
	_43 += t.z;
	return *this;
}

Matrix44 Matrix44::Translation( float x, float y, float z) {
	Matrix44 m;
		m._41 = x;
		m._42 = y;
		m._43 = z;
	return m;
}

Matrix44 Matrix44::Translation(const Vec3& v) {
	Matrix44 m;
		m._41 = v.x;
		m._42 = v.y;
		m._43 = v.z;
	return m;
}

Matrix44 Matrix44::RotationEuler(const Vec3& rot) {
	return RotationX(rot.x) * RotationY(rot.y) * RotationZ(rot.z);
}

Matrix44 Matrix44::RotationEuler(float x, float y, float z) {
	return RotationX(x) * RotationY(y) * RotationZ(z);
}

Matrix44 Matrix44::RotationX(float angle) {
	Matrix44 m;
		m._22 = m._33 = cos(angle);
		m._32 = m._23 = sin(angle); m._32 *= -1;
	return m;
}

Matrix44 Matrix44::RotationY(float angle) {
	Matrix44 m;
		m._11 = m._33 = cos(angle);
		m._13 = m._31 = sin(angle); m._13 *= -1;
	return m;
}

Matrix44 Matrix44::RotationZ(float angle) {
	Matrix44 m;
		m._11 = m._22 = cos(angle);
		m._21 = m._12 = sin(angle); m._21 *= -1;
	return m;
}

Matrix44 Matrix44::RotationAxisAngle(const Vec3& axis, float angle) {
	Matrix44 m;

	float c = cos(angle);
	float s = sin(angle);
	float t = 1.f-c;

	m._11 = t*axis.x*axis.x + c;			m._12 = t*axis.x*axis.y + axis.z*s;		m._13 = t*axis.x*axis.z - axis.y*s;
	m._21 = t*axis.x*axis.y - axis.z*s;		m._22 = t*axis.y*axis.y + c;			m._23 = t*axis.y*axis.z + axis.x*s;
	m._31 = t*axis.x*axis.z + axis.y*s;		m._32 = t*axis.y*axis.z - axis.x*s;		m._33 = t*axis.z*axis.z + c;

	// Set back temps to zero
	/*m._41 = 0.f;
	m._42 = 0.f;
	m._43 = 0.f;
	m._14 = 0.f;
	m._24 = 0.f;
	m._34 = 0.f;
	m._44 = 1.f;*/

	return m;
};

Matrix44 Matrix44::RotationQuat ( const Quat& q) {
	Matrix44 m;
		m._11 =1.f-2.f*(q.y*q.y + q.z*q.z);		m._12 =2.f*(q.x*q.y + q.z*q.w);		m._13 =2.f*(q.x*q.z - q.y*q.w);		m._14 =0.f;
		m._21 =2.f*(q.x*q.y - q.z*q.w);			m._22 =1.f-2.f*(q.x*q.x + q.z*q.z);	m._23 =2.f*(q.y*q.z + q.x*q.w);		m._24 =0.f;
		m._31 =2.f*(q.x*q.z + q.y*q.w);			m._32 =2.f*(q.y*q.z - q.x*q.w);		m._33 =1.f-2.f*(q.x*q.x + q.y*q.y);	m._34 =0.f;
		m._41 =0.f;								m._42 =0.f;							m._43 =0.f;							m._44 =1.f;
	return m;
};


Matrix44 Matrix44::ViewRH(const Vec3& eye, const Vec3& target, const Vec3& up) {
	Vec3 baseFront = (target - eye).Normalize();		// The "look-at" vector.
	Vec3 baseRight = Cross(baseFront, up).Normalize();	// The "right" vector.
	Vec3 baseUp = Cross(baseRight, baseFront);			// The "up" vector.

	// Create a 4x4 orientation matrix from the right, up, and at vectors
	// TRANPOSE of ROT
	Matrix44 orientation (	baseRight.x, baseUp.x, baseFront.x, 0,
							baseRight.y, baseUp.y, baseFront.y, 0,
							baseRight.z, baseUp.z, baseFront.z, 0,
							0	   , 0      , 0      , 1 );

	// Create a 4x4 translation matrix by negating the eye position.
	// NEGATE POS
	Matrix44 translation (	1,      0,      0,    0,
							0,      1,      0,     0, 
							0,      0,      1,     0,
							-eye.x, -eye.y, -eye.z,  1 );

	// Combine the orientation and translation to compute the view matrix
	return translation * orientation;
}

Matrix44 Matrix44::ProjPerspective(float nearPlane, float farPlane, float fovRad, float aspectRatio) {
	Matrix44 m;
		m._11  = m._22 = 1.0f / tanf(fovRad * 0.5f);
		m._11 /= aspectRatio;
		m._33  = farPlane / (farPlane - nearPlane);
		m._43  = -(farPlane * nearPlane) / (farPlane - nearPlane);
		m._34  = 1;
		m._44  = 0;
	return m;
}

Matrix44 Matrix44::ProjOrtographic(float nearPlane, float farPlane, float left, float right, float bottom, float top) {
	Matrix44 m;
		m._11 = 2 / (right - left);
		m._22 = 2 / (top - bottom);
		m._33 = 2 / (farPlane - nearPlane);
	return m;
}

///////////////////////////////////////////////////////////////////////////
// utility functions
void MatrixPrint (const Matrix44& m, std::ostream& os/*=std::cout*/) {
	for (unsigned row = 0; row<4; row++) {
		for (unsigned col = 0; col<4; col++) {
			os << m(row,col) << "	";
		}
		os << "\n";
	}
}

std::ostream& operator << (std::ostream& os, const Matrix44& m) {
	MatrixPrint(m, os);
	return os;
}


Matrix44 lerp(const Matrix44& m1, const Matrix44& m2, float t) {
	return m2 * t + m1 * (1 - t);
}
