// Vec3.h By Zs�roskeny�r Team 2013.10.09 23:12 last modified N�meth Rich�rd
#pragma once

#include <math.h>
#include <iostream>

class Matrix44;
class Vec3 {
	public:
		float x,y,z;

		Vec3() {x=y=z=0.0f;}
		Vec3(float x, float y, float z) {this->x=x; this->y=y; this->z=z;}
		Vec3(float v[3]) {x = v[0]; y=v[1]; z=v[2];}


		Vec3& normalize();

		static Vec3 normalize(const Vec3& v);
		static float dot(const Vec3& v1, const Vec3& v2);
		static Vec3 cross(const Vec3& v1, const Vec3& v2);
		static float lenght(const Vec3& v);

		float lenght() const;

		Vec3& operator *= (const Vec3& v2);
		Vec3& operator /= (const Vec3& v2);
		Vec3& operator += (const Vec3& v2);
		Vec3& operator -= (const Vec3& v2);

		Vec3& operator *= (const float& s);
		Vec3& operator /= (const float& s); 

		Vec3 operator * (const Vec3& v2) const;
		Vec3 operator / (const Vec3& v2) const;
		Vec3 operator + (const Vec3& v2) const;
		Vec3 operator - (const Vec3& v2) const;

		// Matrix trans
		Vec3 operator * (const Matrix44& m) const;
		Vec3& operator *= (const Matrix44& m);

		Vec3 operator * (const float&s ) const;
		Vec3 operator / (const float&s ) const;
		friend const Vec3 operator*(float s, Vec3 v);

		Vec3 operator - () const;

		// boolean operators
		bool operator == (const Vec3& w) const;
		bool operator != (const Vec3& w) const;
};

// utility functions ///////////////////////////////
std::ostream& operator<<(std::ostream& os, Vec3 v);




