// Vec4.h By Zsíroskenyér Team 2013.10.09 23:12 last modified Németh Richárd
#pragma once

#include <math.h>
#include <iostream>

class Matrix44;
class Vec4 {
public:
	float x,y,z, w;

	Vec4();
	Vec4(float x, float y, float z, float w);
	Vec4(float v[4]);


	Vec4& Normalize();

	static Vec4 Normalize(const Vec4& v);
	static float Dot(const Vec4& v1, const Vec4& v2);
	static Vec4 Cross(const Vec4& v1, const Vec4& v2);
	static float Lenght(const Vec4& v);

	float Lenght() const;

	Vec4& operator *= (const Vec4& v2);
	Vec4& operator /= (const Vec4& v2);
	Vec4& operator += (const Vec4& v2);
	Vec4& operator -= (const Vec4& v2);

	Vec4& operator *= (const float& s);
	Vec4& operator /= (const float& s); 

	Vec4 operator * (const Vec4& v2) const;
	Vec4 operator / (const Vec4& v2) const;
	Vec4 operator + (const Vec4& v2) const;
	Vec4 operator - (const Vec4& v2) const;

	// Matrix trans
	Vec4 operator * (const Matrix44& m) const;
	Vec4& operator *= (const Matrix44& m);

	Vec4 operator * (const float&s ) const;
	Vec4 operator / (const float&s ) const;
	friend const Vec4 operator*(float s, Vec4 v);

	Vec4 operator - () const;
	float operator [](size_t idx) const;

	// boolean operators
	bool operator == (const Vec4& v) const;
	bool operator != (const Vec4& v) const;
};

// utility functions ///////////////////////////////
std::ostream& operator<<(std::ostream& os, Vec4 v);




