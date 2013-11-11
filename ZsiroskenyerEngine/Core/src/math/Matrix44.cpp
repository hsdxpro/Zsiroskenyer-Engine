///////////////////////////////////////////////////////////////////////////
// matrix.cpp
// definitions for matrix-stuff
///////////////////////////////////////////////////////////////////////////
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

Matrix44 Matrix44::operator * (const Matrix44 & m2) {
	return Matrix44(	m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0],		m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1]  + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1],		m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2]  + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2],		m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3]  + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3],
						m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0],		m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1]  + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1],		m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2]  + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2],		m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3]  + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3],
						m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0],		m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1]  + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1],		m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2]  + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2],		m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3]  + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3],
						m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0],		m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1]  + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1],		m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2]  + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2],		m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3]  + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3] );
}

Matrix44 Matrix44::operator + (const Matrix44 & m2) {
	Matrix44 m = *this;
	m += m2;
	return m;
}

Matrix44 Matrix44::operator - (const Matrix44 & m2) {
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

bool Matrix44::operator == (const Matrix44& m2) const {
	float *p1 = (float*)m, *p2 = (float*)m2.m;
	for (int i=0; i<16; i++) {
		if (p1[i] != p2[i]) {
			return false;
		}
	}
	return true;
}

bool Matrix44::operator != (const Matrix44& m2) const {
	return !(*this == m2);
}

Matrix44& Matrix44::Identity() {
	_11 = 1.f; _12 = 0.f; _13 = 0.f; _14 = 0.f;
	_21 = 0.f; _22 = 1.f; _23 = 0.f; _24 = 0.f;
	_31 = 0.f; _32 = 0.f; _33 = 1.f; _34 = 0.f;
	_41 = 0.f; _42 = 0.f; _43 = 0.f; _44 = 1.f;
	return *this;
}

Matrix44& Matrix44::Transpose(const Matrix44& in) {
	_11 = in._11;	_12 = in._21;	_13 = in._31;	_14 = in._41;
	_21 = in._12;	_22 = in._22;	_23 = in._32;	_24 = in._42;
	_31 = in._13;	_32 = in._23;	_33 = in._33;	_34 = in._43;
	_41 = in._14;	_42 = in._24;	_43 = in._34;	_44 = in._44;
	return *this;
}

void Matrix44::SetColumn(size_t idx, const Vec4& v) {
	for(size_t i = 0; i < 4; i++) {
		this->m[i][idx] = v[i];
	}
}

Matrix44& Matrix44::Transpose() {

	
	Vec4 rows[4];
	memcpy(rows, this, sizeof(Matrix44));
	for(size_t i = 0; i < 4; i++)
		SetColumn(i, rows[i]);
	/*
	float t;
	t=_12;	_12 = _21;	_21=t;
	t=_13;	_13 = _31;	_31=t;
	t=_14;	_14 = _41;	_41=t;

	t= _24;	_24 = _42;	_42=t;
	t= _23;	_23 = _32;	_32=t;
	t= _34;	_34 = _43;	_43=t;
	*/
	return *this;
};

float Matrix44::Determinant() {
	return
		  _11 * ( _22 * (_33 * _44 - _43 * _34) - _23 * (_32 * _44 - _42 * _34) + _24 * (_32 * _43 - _42 * _33) )
		- _12 * ( _21 * (_33 * _44 - _43 * _34) - _23 * (_31 * _44 - _41 * _34) + _24 * (_31 * _43 - _41 * _33) )
		+ _13 * ( _21 * (_32 * _44 - _42 * _34) - _22 * (_31 * _44 - _41 * _34) + _24 * (_31 * _42 - _41 * _32) )
		- _14 * ( _21 * (_32 * _43 - _42 * _33) - _22 * (_31 * _43 - _41 * _33) + _23 * (_31 * _42 - _32 * _41) );
}

Matrix44& Matrix44::Inverse(Matrix44& out) {
	// determinant
	float det_A;
	// partial computations for speed optimizations (hopefully faster, should be tested)
	float 
		A = _33 * _44 - _43 * _34,
		B = _32 * _44 - _42 * _34,
		C = _32 * _43 - _42 * _33,
		D = _31 * _44 - _41 * _34,
		E = _31 * _43 - _41 * _33,
		F = _31 * _42 - _41 * _32,


		G = _23 * _44 - _43 * _24,
		H = _22 * _44 - _42 * _24,
		I = _22 * _43 - _42 * _23,
		J = _21 * _44 - _41 * _24,
		K = _21 * _43 - _41 * _23,
		L = _21 * _42 - _41 * _22,


		M = _23 * _34 - _33 * _24,
		N = _22 * _34 - _32 * _24,
		O = _22 * _33 - _32 * _23,
		P = _21 * _34 - _31 * _24,
		Q = _21 * _33 - _31 * _23,
		R = _21 * _32 - _31 * _22;	

	// A; B; C; D; E; F
	out._11 =  (_22*A - _23*B + _24*C);
	out._21 = -(_21*A - _23*D + _24*E);
	out._31 =  (_21*B - _22*D + _24*F);
	out._41 = -(_21*C - _22*E + _23*F);

	det_A = 1.f/(_11*out._11 + _12 * out._21 + _13 * out._31 + _14 * out._41);

	out._12 = -(_12*A - _13*B + _14*C) * det_A;
	out._22 =  (_11*A - _13*D + _14*E) * det_A;
	out._32 = -(_11*B - _12*D + _14*F) * det_A;
	out._42 =  (_11*C - _12*E + _13*F) * det_A;

	// G, H, I, J, K, L
	out._13 =  (_12*G - _13*H + _14*I) * det_A;
	out._23 = -(_11*G - _13*J + _14*K) * det_A;
	out._33 =  (_11*H - _12*J + _14*L) * det_A;
	out._43 = -(_11*I - _12*K + _13*L) * det_A;

	// M, N, O, P, Q, R
	out._14 = -(_12*M - _13*N + _14*O) * det_A;
	out._24 =  (_11*M - _13*P + _14*Q) * det_A;
	out._34 = -(_11*N - _12*P + _14*R) * det_A;
	out._44 =  (_11*O - _12*Q + _13*R) * det_A;

	// post multiplication by det_A, cause det_a was not available when these were computed
	out._11 *= det_A; 
	out._21 *= det_A; 
	out._31 *= det_A; 
	out._41	*= det_A; 

	return out;
}

Matrix44& Matrix44::Inverse() {
	return Inverse(*this);
}

Matrix44& Matrix44::Scale(float scX, float scY, float scZ) {
	_11 *= scX;
	_22 *= scY;
	_33 *= scZ;
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

Matrix44& Matrix44::Translation( float vX, float vY, float vZ) {
	Identity();
	_41 = vX;
	_42 = vY;
	_43 = vZ;
	return *this;
}

Matrix44& Matrix44::Translation(const Vec3& v) {
	Identity();
	_41 = v.x;
	_42 = v.y;
	_43 = v.z;
	return *this;
}

Matrix44& Matrix44::RotateEuler(const Vec3& rot) {
	return RotationZ(rot.z) * RotationX(rot.x) * RotationY(rot.y);
}

Matrix44& Matrix44::RotationX (float angle) {
	Identity();
	// compute sine and cosine
	float sin_a, cos_a;
	sin_a = sin(angle);
	cos_a = cos(angle);
	// set matrix
	_22 = cos_a;
	_23 = sin_a;
	_32 = -sin_a;
	_33 = cos_a;

	return *this;
}

Matrix44& Matrix44::RotationY(float angle) {
	Identity();
	// compute sine and cosine
	float sin_a, cos_a;
	sin_a = sin(angle);
	cos_a = cos(angle);
	// set matrix
	_11 = cos_a;
	_13 = -sin_a;
	_31 = sin_a;
	_33 = cos_a;

	return *this;
}

Matrix44& Matrix44::RotationZ(float angle) {
	Identity();
	// compute sine and cosine
	float sin_a, cos_a;
	sin_a = sin(angle);
	cos_a = cos(angle);
	// set matrix
	_11 = cos_a;
	_12 = sin_a;
	_21 = -sin_a;
	_22 = cos_a;

	return *this;
}

Matrix44& Matrix44::RotationAxisAngle(const Vec3& axis, float angle) {
	float c = cos(angle);
	float s = sin(angle);
	float t = 1.f-c;

	_11 = t*axis.x*axis.x + c;				_12 = t*axis.x*axis.y + axis.z*s;		_13 = t*axis.x*axis.z - axis.y*s;
	_21 = t*axis.x*axis.y - axis.z*s;		_22 = t*axis.y*axis.y + c;				_23 = t*axis.y*axis.z + axis.x*s;
	_31 = t*axis.x*axis.z + axis.y*s;		_32 = t*axis.y*axis.z - axis.x*s;		_33 = t*axis.z*axis.z + c;

	// Set back temps to zero
	_41 = 0.f;
	_42 = 0.f;
	_43 = 0.f;
	_14 = 0.f;
	_24 = 0.f;
	_34 = 0.f;
	_44 = 1.f;

	return *this;
};

Matrix44& Matrix44::RotationQuat ( const Quat& q) {
	_11 =1.f-2.f*(q.y*q.y + q.z*q.z);		_12 =2.f*(q.x*q.y + q.z*q.w);		_13 =2.f*(q.x*q.z - q.y*q.w);		_14 =0.f;
	_21 =2.f*(q.x*q.y - q.z*q.w);			_22 =1.f-2.f*(q.x*q.x + q.z*q.z);	_23 =2.f*(q.y*q.z + q.x*q.w);		_24 =0.f;
	_31 =2.f*(q.x*q.z + q.y*q.w);			_32 =2.f*(q.y*q.z - q.x*q.w);		_33 =1.f-2.f*(q.x*q.x + q.y*q.y);	_34 =0.f;
	_41 =0.f;								_42 =0.f;							_43 =0.f;							_44 =1.f;

	return *this;
};


Matrix44 Matrix44::MatrixViewRH(const Vec3& eye, const Vec3& target, const Vec3& up) {
	Vec3 zaxis = (target - eye).Normalize();		// The "look-at" vector.
	Vec3 xaxis = Vec3::Cross(zaxis, up).Normalize();// The "right" vector.
	Vec3 yaxis = Vec3::Cross(xaxis, zaxis);			// The "up" vector.

	// Create a 4x4 orientation matrix from the right, up, and at vectors
	Matrix44 orientation (	xaxis.x, yaxis.x, zaxis.x, 0,
							xaxis.y, yaxis.y, zaxis.y, 0,
							xaxis.z, yaxis.z, zaxis.z, 0,
							0	   , 0      , 0      , 1 );

	// Create a 4x4 translation matrix by negating the eye position.
	Matrix44 translation (	1,      0,      0,    0,
							0,      1,      0,     0, 
							0,      0,      1,     0,
							-eye.x, -eye.y, -eye.z,  1 );

	// Combine the orientation and translation to compute the view matrix
	return ( translation * orientation );
}

Matrix44 Matrix44::MatrixProjPerspective(float nearPlane, float farPlane, float fovRad, float aspectRatio) {
	Matrix44 mat;
	mat.Identity();
	mat._11 = mat._22 = 1.0f / tanf(fovRad * 0.5f);
	mat._11 /= aspectRatio;
	mat._33 =   farPlane / (farPlane - nearPlane);
	mat._43 =  -(farPlane * nearPlane) / (farPlane - nearPlane);
	mat._34 =   1;
	mat._44 = 0;
	return mat;
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
