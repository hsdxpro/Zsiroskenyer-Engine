#include "Vec4.h"
#include "Matrix44.h"

#define VEC4_NORM_TOLERANCE 0.00003f

Vec4::Vec4()
:x(0), y(0), z(0), w(0) {
}

Vec4::Vec4(const Vec3& v, float w)
:x(v.x), y(v.y), z(v.z), w(w) {
}

Vec4::Vec4(float x, float y, float z, float w)
:x(x), y(y), z(z), w(w) {
}

Vec4::Vec4(float v[4])
:x(v[0]), y(v[1]), z(v[2]), w(v[3]) {
}

Vec4& Vec4::operator*=(const Vec4& v2) {
	x *= v2.x;
	y *= v2.y;
	z *= v2.z;
	w *= v2.w;
	return *this;
}

Vec4& Vec4::operator/=(const Vec4& v2) {
	x /= v2.x;
	y /= v2.y;
	z /= v2.z;
	w /= v2.w;
	return *this;
}

Vec4& Vec4::operator += (const Vec4& v2) {
	x += v2.x;
	y += v2.y;
	z += v2.z;
	w += v2.w;
	return *this;
}

Vec4& Vec4::operator -= (const Vec4& v2) {
	x -= v2.x;
	y -= v2.y;
	z -= v2.z;
	w -= v2.w;
	return *this;
}

Vec4 Vec4::operator * (const Vec4& v2) const {
	Vec4 vr = *this;
	vr *= v2;
	return vr;
}

Vec4 Vec4::operator / (const Vec4& v2) const {
	Vec4 vr = *this;
	vr /= v2;
	return vr;
}

Vec4 Vec4::operator + (const Vec4& v2) const {
	Vec4 vr = *this;
	vr += v2;
	return vr;
}

Vec4 Vec4::operator - (const Vec4& v2) const {
	Vec4 vr = *this;
	vr -= v2;
	return vr;
}

Vec4 Vec4::operator - () const {
	Vec4 vr(-x, -y, -z, -w);
	return vr;
}

Vec4& Vec4::operator *= (const float& s) {
	x*=s; y*=s; z*=s; w*=s;
	return *this;
}

Vec4& Vec4::operator *= (const Matrix44& m) {
	x = m._11 * x + m._21 * y + m._31 * z + m._41;
	y = m._12 * x + m._22 * y + m._32 * z + m._42;
	z = m._13 * x + m._23 * y + m._33 * z + m._43;
	z = m._14 * x + m._24 * y + m._34 * z + m._44;
	return *this;
}

Vec4 Vec4::operator * (const float&s) const {
	Vec4 vr = *this;
	vr *= s;
	return vr;
}

Vec4& Vec4::operator /= (const float& s) {
	(*this) *= (1.0f / s);
	return *this;
}

Vec4 Vec4::operator / (const float&s) const {
	Vec4 vr=*this;
	vr /= s;
	return vr;
}

bool Vec4::operator == (const Vec4& v) const {
	if (x == v.x && y == v.y && z == v.z && w == v.z)
		return true;
	else
		return false;
}

bool Vec4::operator != (const Vec4& v) const {
	return !(*this == v);
}

float Vec4::operator [](size_t idx) const {
	return *(((float*)this) + idx);
}

Vec4& Vec4::Normalize() {
	float d = fabs(1.f-(x*x + y*y + z*z));
	if (d > VEC4_NORM_TOLERANCE) {
		float scale = 1.0f / (this->Lenght());
		(*this) *= scale;
	}
	return *this;
}

float Vec4::Lenght() const {
	return sqrt(x*x + y*y + z*z);
}

Vec4 Vec4::Normalize(const Vec4& v) {
	Vec4 n=v;
	n.Normalize();
	return n;
}

float Vec4::Dot(const Vec4& v1, const Vec4& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vec4 Vec4::Cross(const Vec4& v1, const Vec4& v2) {
	Vec4 ret(	v1.y*v2.z - v1.z*v2.y,
				v1.z*v2.x - v1.x*v2.z,
				v1.x*v2.y - v1.y*v2.x , 1.0f);
	return ret;
}

float Vec4::Lenght(const Vec4& v) {
	return v.Lenght();
}

Vec4 operator * (const Vec4& v, const Matrix44& m) {
	Vec4 v2 = v;
	v2 *= m;
	return v2;
}

Vec4 operator * (const Matrix44& m, const Vec4& v) {
	Vec4 v2 = v;
	v2 *= m;
	return v2;
};

std::ostream& operator<<(std::ostream& os, Vec4 v) {
	os << v.x << ',' << v.y << ',' << v.z;
	return os;
}