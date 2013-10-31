// Implementation
// history:
// 2012.oct.11, quaternion rotation seems to be perfect

#include "Quat.h"
#include "Matrix44.h"

Quat::Quat() 
:x(0.0f), y(0.0f), z(0.0f), w(1.0f) {
}

Quat::Quat(float x, float y, float z, float w) {
	this->x=x;
	this->y=y;
	this->z=z;
	this->w=w;
}

Quat::Quat(const Vec3& v, float angle) {
	float sin_angle = sin(angle*0.5f);
	x = v.x*sin_angle;
	y = v.y*sin_angle;
	z = v.z*sin_angle;
	w = cos(angle*0.5f);
}

Quat& Quat::operator *= (const Quat& q2) {
	float w1=w, x1=x, y1=y, z1=z;
    x =  x1 * q2.w + y1 * q2.z - z1 * q2.y + w1 * q2.x;
    y = -x1 * q2.z + y1 * q2.w + z1 * q2.x + w1 * q2.y;
    z =  x1 * q2.y - y1 * q2.x + z1 * q2.w + w1 * q2.z;
    w = -x1 * q2.x - y1 * q2.y - z1 * q2.z + w1 * q2.w;
	return *this;
}

Quat Quat::operator * (const Quat& q2) const {
	Quat r;
	r.w = w*q2.w - x*q2.x - y*q2.y - z*q2.z;
	r.x = w*q2.x + x*q2.w + y*q2.z - z*q2.y;
	r.y = w*q2.y - x*q2.z + y*q2.w + z*q2.x;
	r.z = w*q2.z + x*q2.y - y*q2.x + z*q2.w;
	return r;
}

const Quat Quat::operator ~ () const {
	Quat r;
	r.w=w;
	r.x=-x;
	r.y=-y;
	r.z=-z;
	return r;
}

bool Quat::operator == (const Quat& r) const {
	if (w==r.w && x==r.x && y==r.y && z==r.z)
		return true;
	else
		return false;
}

bool Quat::operator != (const Quat& r) const {
	return !(*this==r);
}

float Quat::lenght() const {
	return sqrt(w*w + x*x + y*y + z*z);
}

Quat& Quat::normalize() {
	float d = fabs(1.f-(w*w + x*x + y*y + z*z));
	if (d>QUAT_NORM_TOLERANCE) {
		float scale = 1.f/(this->lenght());
		w*=scale;
		x*=scale;
		y*=scale;
		z*=scale;
	}
	return *this;
}

Quat normalize(const Quat& q) {
	Quat n=q;
	n.normalize();
	return n;
}

float lenght(const Quat& q) {
	return q.lenght();
}

Vec3 QuatRotateVec3(Vec3 v, Quat q) {
	Quat qv(0.f,v.x,v.y,v.z);

	Quat qvr = q*qv*(~q);
	
	Vec3 vr;
	vr.x = qvr.x;
	vr.y = qvr.y;
	vr.z = qvr.z;

	return vr;
}

Vec3 QuatRotateVec3_2(Vec3 v, Quat q) {
	Vec3 vr;
	float w=q.w, x=-q.x, y=q.y, z=q.z;
	vr.x = w*w*v.x + 2*y*w*v.z - 2*z*w*v.y + x*x*v.x + 2*y*x*v.y + 2*z*x*v.z - z*z*v.x - y*y*v.x;
	vr.y = 2*x*y*v.x + y*y*v.y + 2*z*y*v.z + 2*w*z*v.x - z*z*v.y + w*w*v.y - 2*x*w*v.z - x*x*v.y;
	vr.z = 2*x*z*v.x + 2*y*z*v.y + z*z*v.z - 2*w*y*v.x - y*y*v.z + 2*w*x*v.y - x*x*v.z + w*w*v.z;
	return vr;
}

Quat& QuatConjugate(Quat& out, const Quat& q) {
	out = ~q; return out;
}

float QuatDot(const Quat& q1, const Quat& q2) {
	return q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
}

Quat& QuatRotation(Quat& out, Vec3 axis, float angle) {
	float sin_angle = sin(angle*0.5f);
	out.w = cos(angle*0.5f);
	out.x = axis.x*sin_angle;
	out.y = axis.y*sin_angle;
	out.z = axis.z*sin_angle;
	return out;
}

Quat::operator Matrix44() {
	Matrix44 m (
		1.f*-2.f*(y*y + z*z),		2.f*(x*y - z*w),		2.f*(x*z + y*w),		0.f,
		2.f*(x*y + z*w),			1.f-2.f*(x*x + y*y),	2.f*(y*z - x*w),		0.f,
		2.f*(x*z - y*w),			2.f*(y*z + x*w),		1.f-2.f*(x*x + y*y),	0.f,
		0.f,						0.f,					0.f,					1.f
	);
	return m;
};

Quat Quat::EulerAnglesToQuat(const Vec3& eulerAngles) {
	return EulerAnglesToQuat(eulerAngles.x, eulerAngles.y, eulerAngles.z);
}

Quat Quat::EulerAnglesToQuat(float rotX, float rotY, float rotZ) {
	Quat q;
	float cos_x = cos(rotX * 0.5f);
	float sin_x = sin(rotX * 0.5f);
	float cos_y = cos(rotY * 0.5f);
	float sin_y = sin(rotY * 0.5f);
	float cos_z = cos(rotZ * 0.5f);
	float sin_z = sin(rotZ * 0.5f);
						   
	q.w =	cos_x *	cos_y *	cos_z	+	sin_x *	sin_y *	sin_z;
	q.x =	sin_x *	cos_y *	cos_z	-	cos_x *	sin_y *	sin_z;
	q.y =	cos_x *	sin_y *	cos_z	+	sin_x *	cos_y *	sin_z;
	q.z =	cos_x *	cos_y *	sin_z	-	sin_x *	sin_y *	cos_z;

	return q;
}

Vec3 Quat::QuatToEulerAngles(const Quat& q) {
	Vec3 eulerAngles;
	eulerAngles.x = atan2(2.f*(q.w*q.x+q.y*q.z), 1-2.f*(q.x*q.x+q.y*q.y));
	eulerAngles.y = asin(2.f*(q.w*q.y-q.x*q.z));
	eulerAngles.z = atan2(2.f*(q.w*q.z+q.x*q.y), 1-2.f*(q.y*q.y+q.z*q.z));
	return eulerAngles;
}

std::ostream& operator<<(std::ostream& os, Quat q) {
	os << q.w << ':' << q.x << ',' << q.y << ',' << q.z;
	return os;
}



