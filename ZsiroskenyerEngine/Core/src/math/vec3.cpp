// Implementation
#include "Vec3.h"
#include "Matrix44.h"

#include "../common.h"

#define VEC3_NORM_TOLERANCE 0.00003f

Vec3& Vec3::operator*=(const Vec3& v2) {
	x*=v2.x;
	y*=v2.y;
	z*=v2.z;
	return *this;
}

Vec3& Vec3::operator/=(const Vec3& v2) {
	x/=v2.x;
	y/=v2.y;
	z/=v2.z;
	return *this;
}

Vec3& Vec3::operator += (const Vec3& v2) {
	x+=v2.x;
	y+=v2.y;
	z+=v2.z;
	return *this;
}

Vec3& Vec3::operator -= (const Vec3& v2) {
	x-=v2.x;
	y-=v2.y;
	z-=v2.z;
	return *this;
}

Vec3 Vec3::operator * (const Vec3& v2) const {
	Vec3 vr=*this;
	vr*=v2;
	return vr;
}

Vec3 Vec3::operator / (const Vec3& v2) const {
	Vec3 vr = *this;
	vr /= v2;
	return vr;
}

Vec3 Vec3::operator + (const Vec3& v2) const {
	Vec3 vr = *this;
	vr += v2;
	return vr;
}

Vec3 Vec3::operator - (const Vec3& v2) const {
	Vec3 vr = *this;
	vr -= v2;
	return vr;
}

Vec3 Vec3::operator - () const {
	Vec3 vr(-x, -y, -z);
	return vr;
}

Vec3& Vec3::operator *= (const float& s) {
	x*=s; y*=s; z*=s;
	return *this;
}

Vec3& Vec3::operator *= (const Matrix44& m) {
	x = m._11 * x + m._21 * y + m._31 * z + m._41;
	y = m._12 * x + m._22 * y + m._32 * z + m._42;
	z = m._13 * x + m._23 * y + m._33 * z + m._43;
	return *this;
}

Vec3 Vec3::operator * (const float&s) const {
	Vec3 vr=*this;
	vr*=s;
	return vr;
}

Vec3& Vec3::operator /= (const float& s) {
	(*this) *= (1.0f / s);
	return *this;
}

Vec3 Vec3::operator / (const float&s) const {
	Vec3 vr=*this;
	vr /= s;
	return vr;
}

bool Vec3::operator == (const Vec3& w) const {
	if (x == w.x && y == w.y && z == w.z)
		return true;
	else
		return false;
}

bool Vec3::operator != (const Vec3& w) const {
	return !(*this==w);
}

Vec3& Vec3::Normalize() {
	float d = fabs(1.f-(x*x + y*y + z*z));
	if (d > VEC3_NORM_TOLERANCE) {
		float scale = 1.0f / (this->Lenght());
		(*this) *= scale;
	}
	return *this;
}

float Vec3::Lenght() const {
	return sqrt(x*x + y*y + z*z);
}

float Vec3::operator [](size_t idx) const {
	ZSASSERT(idx >= 0 && idx <= 4);
	return *((float*)this + idx);
}

Vec3 Vec3::Normalize(const Vec3& v) {
	Vec3 n = v;
	n.Normalize();
	return n;
}

float Vec3::Dot(const Vec3& v1, const Vec3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vec3 Vec3::Cross(const Vec3& v1, const Vec3& v2) {
	Vec3 ret(	v1.y*v2.z - v1.z*v2.y,
				v1.z*v2.x - v1.x*v2.z,
				v1.x*v2.y - v1.y*v2.x );
	return ret;
}

float Vec3::Lenght(const Vec3& v) {
	return v.Lenght();
}


Vec3 operator * (const Vec3& v, const Matrix44& m) {
	Vec3 v2 = v;
	v2 *= m;
	return v2;
}

Vec3 operator * (const Matrix44& m, const Vec3& v) {
	Vec3 v2 = v;
	v2 *= m;
	return v2;
};


std::ostream& operator << (std::ostream& os, Vec3 v) {
	os << v.x << ',' << v.y << ',' << v.z;
	return os;
}
