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
#include "quat.h"
#include "vec3.h"
#include <memory>

Matrix44::Matrix44() {
	_11 = 1.f; _12 = 0.f; _13 = 0.f; _14 = 0.f;
	_21 = 0.f; _22 = 1.f; _23 = 0.f; _24 = 0.f;
	_31 = 0.f; _32 = 0.f; _33 = 1.f; _34 = 0.f;
	_41 = 0.f; _42 = 0.f; _43 = 0.f; _44 = 1.f;
};

Matrix44::Matrix44(	float _11, float _12, float _13, float _14,
					float _21, float _22, float _23, float _24,
					float _31, float _32, float _33, float _34,
					float _41, float _42, float _43, float _44)
:	_11(_11), _12(_12), _13(_13), _14(_14),
	_21(_21), _22(_22), _23(_23), _24(_14),
	_31(_31), _32(_32), _33(_33), _34(_14),
	_41(_41), _42(_42), _43(_43), _44(_14)
{ }


float& Matrix44::operator() (unsigned row, unsigned col) {
	return m[row][col];
}
const float& Matrix44::operator() (unsigned row, unsigned col) const {
	return m[row][col];
};

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
	return (*this)*=(1.f/f);
}

Matrix44 Matrix44::operator * (const Matrix44 & m2) {
	return Matrix44(	m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0],		m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1]  + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1],		m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2]  + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2],		m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3]  + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3],
						m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0],		m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1]  + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1],		m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2]  + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2],		m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3]  + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3],
						m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0],		m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1]  + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1],		m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2]  + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2],		m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3]  + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3],
						m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0],		m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1]  + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1],		m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2]  + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2],		m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3]  + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3] );
}

Matrix44 Matrix44::operator + (const Matrix44 & m2) {
	Matrix44 m = *this;
	m+=m2;
	return m;
}

Matrix44 Matrix44::operator - (const Matrix44 & m2) {
	Matrix44 m = *this;
	m-=m2;
	return m;
}

Matrix44 Matrix44::operator * (const float& f) {
	Matrix44 m = *this;
	m*=f;
	return m;
}

Matrix44 Matrix44::operator / (const float& f) {
	Matrix44 m = *this;
	m/=f;
	return m;
}

bool Matrix44::operator==(const Matrix44& m2) const {
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

Matrix44& MatrixIdentity(Matrix44& m) {
	m._11 = 1.f; m._12 = 0.f; m._13 = 0.f; m._14 = 0.f;
	m._21 = 0.f; m._22 = 1.f; m._23 = 0.f; m._24 = 0.f;
	m._31 = 0.f; m._32 = 0.f; m._33 = 1.f; m._34 = 0.f;
	m._41 = 0.f; m._42 = 0.f; m._43 = 0.f; m._44 = 1.f;
	return m;
}

Matrix44& MatrixTranspose(const Matrix44& in, Matrix44& out) {
	out._11 = in._11;	out._12 = in._21;	out._13 = in._31;	out._14 = in._41;
	out._21 = in._12;	out._22 = in._22;	out._23 = in._32;	out._24 = in._42;
	out._31 = in._13;	out._32 = in._23;	out._33 = in._33;	out._34 = in._43;
	out._41 = in._14;	out._42 = in._24;	out._43 = in._34;	out._44 = in._44;
	return out;
}

Matrix44& MatrixTranspose(Matrix44& m) {
	float t;
	t=m._12;	m._12 = m._21;	m._21=t;
	t=m._13;	m._13 = m._31;	m._31=t;
	t=m._14;	m._14 = m._41;	m._41=t;

	t= m._24;	m._24 = m._42;	m._42=t;
	t= m._23;	m._23 = m._32;	m._32=t;
	t= m._34;	m._34 = m._43;	m._43=t;

	return m;
};


float MatrixDeterminant(const Matrix44& m) {
	return
		m._11*( m._22*(m._33*m._44-m._43*m._34) - m._23*(m._32*m._44-m._42*m._34) + m._24*(m._32*m._43-m._42*m._33) )
		- m._12*( m._21*(m._33*m._44-m._43*m._34) - m._23*(m._31*m._44-m._41*m._34) + m._24*(m._31*m._43-m._41*m._33) )
		+ m._13*( m._21*(m._32*m._44-m._42*m._34) - m._22*(m._31*m._44-m._41*m._34) + m._24*(m._31*m._42-m._41*m._32) )
		- m._14*( m._21*(m._32*m._43-m._42*m._33) - m._22*(m._31*m._43-m._41*m._33) + m._23*(m._31*m._42-m._32*m._41) );
}

Matrix44& MatrixInverse(const Matrix44& in, Matrix44& out) {
	// determinant
	float det_A;
	// partial computations for speed optimizations (hopefully faster, should be tested)
	float 
		A = in._33*in._44-in._43*in._34,
		B = in._32*in._44-in._42*in._34,
		C = in._32*in._43-in._42*in._33,
		D = in._31*in._44-in._41*in._34,
		E = in._31*in._43-in._41*in._33,
		F = in._31*in._42-in._41*in._32,


		G = in._23*in._44-in._43*in._24,
		H = in._22*in._44-in._42*in._24,
		I = in._22*in._43-in._42*in._23,
		J = in._21*in._44-in._41*in._24,
		K = in._21*in._43-in._41*in._23,
		L = in._21*in._42-in._41*in._22,


		M = in._23*in._34-in._33*in._24,
		N = in._22*in._34-in._32*in._24,
		O = in._22*in._33-in._32*in._23,
		P = in._21*in._34-in._31*in._24,
		Q = in._21*in._33-in._31*in._23,
		R = in._21*in._32-in._31*in._22;	

	// A; B; C; D; E; F
	out._11 = (in._22*A - in._23*B + in._24*C);
	out._21 = -(in._21*A - in._23*D + in._24*E);
	out._31 = (in._21*B - in._22*D + in._24*F);
	out._41 = -(in._21*C - in._22*E + in._23*F);

	det_A = 1.f/(in._11*out._11 + in._12*out._21 + in._13*out._31 + in._14*out._41);

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

Matrix44& MatrixScale(Matrix44& out, float scX, float scY, float scZ) {
	MatrixIdentity(out);
	out._11 = scX;
	out._22 = scY;
	out._33 = scZ;
	return out;
}

Matrix44& MatrixScale(Matrix44& out, Vec3 scale) {
	MatrixIdentity(out);
	out._11 = scale.x;
	out._22 = scale.y;
	out._33 = scale.z;
	return out;
}

Matrix44& MatrixTranslation(Matrix44& out, float vX, float vY, float vZ) {
	MatrixIdentity(out);
	out._41 = vX;
	out._42 = vY;
	out._43 = vZ;
	return out;
}

Matrix44& MatrixTranslation(Matrix44& out, Vec3 v) {
	MatrixIdentity(out);
	out._41 = v.x;
	out._42 = v.y;
	out._43 = v.z;
	return out;
}

Matrix44& MatrixRotationX (Matrix44& out, float angle) {
	MatrixIdentity(out);
	// compute sine and cosine
	float sin_a, cos_a;
	sin_a = sin(angle);
	cos_a = cos(angle);
	// set matrix
	out._22 = cos_a;
	out._23 = sin_a;
	out._32 = -sin_a;
	out._33 = cos_a;

	return out;
}
Matrix44& MatrixRotationY (Matrix44& out, float angle) {
	MatrixIdentity(out);
	// compute sine and cosine
	float sin_a, cos_a;
	sin_a = sin(angle);
	cos_a = cos(angle);
	// set matrix
	out._11 = cos_a;
	out._13 = -sin_a;
	out._31 = sin_a;
	out._33 = cos_a;

	return out;
}
Matrix44& MatrixRotationZ (Matrix44& out, float angle) {
	MatrixIdentity(out);
	// compute sine and cosine
	float sin_a, cos_a;
	sin_a = sin(angle);
	cos_a = cos(angle);
	// set matrix
	out._11 = cos_a;
	out._12 = sin_a;
	out._21 = -sin_a;
	out._22 = cos_a;

	return out;
}
Matrix44& MatrixRotationAxisAngle (Matrix44& out, const Vec3& axis, float angle) {
	float c = cos(angle);
	float s = sin(angle);
	float t = 1.f-c;

	out._11 = t*axis.x*axis.x + c;				out._12 = t*axis.x*axis.y + axis.z*s;		out._13 = t*axis.x*axis.z - axis.y*s;
	out._21 = t*axis.x*axis.y - axis.z*s;		out._22 = t*axis.y*axis.y + c;				out._23 = t*axis.y*axis.z + axis.x*s;
	out._31 = t*axis.x*axis.z + axis.y*s;		out._32 = t*axis.y*axis.z - axis.x*s;		out._33 = t*axis.z*axis.z + c;

	// set back temps to zero
	out._41 = 0.f;
	out._42 = 0.f;
	out._43 = 0.f;
	out._14 = 0.f;
	out._24 = 0.f;
	out._34 = 0.f;
	out._44 = 1.f;

	return out;
};

Matrix44& MatrixRotationQuat (Matrix44& out, const Quat& q) {
	out._11 =1.f-2.f*(q.y*q.y + q.z*q.z);		out._12 =2.f*(q.x*q.y + q.z*q.w);		out._13 =2.f*(q.x*q.z - q.y*q.w);		out._14 =0.f;
	out._21 =2.f*(q.x*q.y - q.z*q.w);			out._22 =1.f-2.f*(q.x*q.x + q.z*q.z);	out._23 =2.f*(q.y*q.z + q.x*q.w);		out._24 =0.f;
	out._31 =2.f*(q.x*q.z + q.y*q.w);			out._32 =2.f*(q.y*q.z - q.x*q.w);		out._33 =1.f-2.f*(q.x*q.x + q.y*q.y);	out._34 =0.f;
	out._41 =0.f;								out._42 =0.f;							out._43 =0.f;							out._44 =1.f;

	return out;
};




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
