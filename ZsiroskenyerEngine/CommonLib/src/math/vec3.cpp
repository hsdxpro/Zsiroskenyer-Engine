///////////////////////////////////////////////////
// source file for Vec3 class
// vec3.cpp
///////////////////////////////////////////////////
#include "vec3.h"
#include "matrix.h"


// defines ////////////////////////////////////////
#define VEC3_NORM_TOLERANCE 0.00003f




// operators //////////////////////////////////////
// compound arithmetic
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
Vec3& Vec3::operator+=(const Vec3& v2) {
	x+=v2.x;
	y+=v2.y;
	z+=v2.z;
	return *this;
}
Vec3& Vec3::operator-=(const Vec3& v2) {
	x-=v2.x;
	y-=v2.y;
	z-=v2.z;
	return *this;
}
// simple arithmetic
const Vec3 Vec3::operator*(const Vec3& v2) const {
	Vec3 vr=*this;
	vr*=v2;
	return vr;
}
const Vec3 Vec3::operator/(const Vec3& v2) const {
	Vec3 vr=*this;
	vr/=v2;
	return vr;
}
const Vec3 Vec3::operator+(const Vec3& v2) const {
	Vec3 vr=*this;
	vr+=v2;
	return vr;
}
const Vec3 Vec3::operator-(const Vec3& v2) const {
	Vec3 vr=*this;
	vr-=v2;
	return vr;
}
// dot product
float Vec3::operator%(const Vec3& v2) const {
	return x*v2.x + y*v2.y + z*v2.z;
}
// cross product
Vec3& Vec3::operator^=(const Vec3& v2) {
	*this = (*this)^v2;
	return *this;
}
Vec3 Vec3::operator^(const Vec3& v2) const {
	Vec3 ret;
	ret.x = (y*v2.z - z*v2.y);
	ret.y = (z*v2.x - x*v2.z);
	ret.z = (x*v2.y - y*v2.x);
	return ret;
}
// unaries
const Vec3 Vec3::operator-() {
	Vec3 vr;
	vr.x = -x;
	vr.y = -y;
	vr.z = -z;
	return vr;
}
// scaling
Vec3& Vec3::operator*=(const float& s) { // multiply
	x*=s; y*=s; z*=s;
	return *this;
}
const Vec3 Vec3::operator*(const float&s) const {
	Vec3 vr=*this;
	vr*=s;
	return vr;
}
const Vec3 operator*(float s, Vec3 v) {
	return v*s;
}
Vec3& Vec3::operator/=(const float& s) { // divide
	(*this)*=(1.f/s);
	return *this;
}
const Vec3 Vec3::operator/(const float&s) const {
	Vec3 vr=*this;
	vr/=s;
	return vr;
}

// boolean 
bool Vec3::operator==(const Vec3& w) {
	if (x==w.x && y==w.y && z==w.z)
		return true;
	else
		return false;
}
bool Vec3::operator!=(const Vec3& w) {
	return !(*this==w);
}



// methods /////////////////////////////////////////
Vec3& Vec3::normalize() {
	float d = fabs(1.f-(x*x + y*y + z*z));
	if (d>VEC3_NORM_TOLERANCE) {
		float scale = 1.f/(this->lenght());
		(*this)*=scale;
	}
	return *this;
}
float Vec3::lenght() const {
	return sqrt(x*x + y*y + z*z);
}




// vector functions ////////////////////////////////
Vec3 normalize(const Vec3& v) {
	Vec3 n=v;
	n.normalize();
	return n;
}
float dot(const Vec3& v1, const Vec3& v2) {
	return v1%v2;
}
Vec3 cross(const Vec3& v1, const Vec3& v2) {
	return v1^v2;
}
float lenght(const Vec3& v) {
	return v.lenght();
}





// matrix transforms
Vec3& operator*=(Vec3& v, const Matrix44& m) {
	float 
		x = m._11*v.x + m._21*v.y + m._31*v.z + m._41,
		y = m._12*v.x + m._22*v.y + m._32*v.z + m._42,
		z = m._13*v.x + m._23*v.y + m._33*v.z + m._43;
	v.x=x, v.y=y, v.z=z;
	return v;
}
Vec3 operator*(const Vec3& v, const Matrix44& m) {
	Vec3 v2 = v;
	v2*=m;
	return v2;
}
Vec3 operator*(const Matrix44& m, const Vec3& v) {
	Vec3 v2 = v;
	v2*=m;
	return v2;
};



// utility functions ///////////////////////////////
std::ostream& operator<<(std::ostream& os, Vec3 v) {
	os << v.x << ',' << v.y << ',' << v.z;
	return os;
}