///////////////////////////////////////////////////
// header for Vec3 class
// vec3.h
///////////////////////////////////////////////////
#pragma once

#include <math.h>
#include <iostream>


union Matrix44;


// Vec3 class definition //////////////////////////
class Vec3 {
	public:
		// data members
		float x,y,z;
		// constructors
		Vec3() {x=y=z=0.f;}
		Vec3(float x, float y, float z) {this->x=x; this->y=y; this->z=z;}
		Vec3(float v[3]) {x = v[0]; y=v[1]; z=v[2];}

		// operators
		// compund assign
		Vec3& operator*=(const Vec3& v2);
		Vec3& operator/=(const Vec3& v2);
		Vec3& operator+=(const Vec3& v2);
		Vec3& operator-=(const Vec3& v2);
		// simple arithmetic
		const Vec3 operator*(const Vec3& v2) const;
		const Vec3 operator/(const Vec3& v2) const;
		const Vec3 operator+(const Vec3& v2) const;
		const Vec3 operator-(const Vec3& v2) const;
		// scaling
		Vec3& operator*=(const float& s); // scale the vector3 (multiply)
		const Vec3 operator*(const float&s ) const;
		friend const Vec3 operator*(float s, Vec3 v);
		Vec3& operator/=(const float& s); // scale the vector3 (divide)
		const Vec3 operator/(const float&s ) const;
		// products
		float operator%(const Vec3& v2) const;	// dot product
		Vec3& operator^=(const Vec3& v2);		// cross product
		Vec3 operator^(const Vec3& v2) const;
		// minus sign (vector pointing to the other way on the same axis)
		const Vec3 operator-();

		// boolean operators
		bool operator==(const Vec3& w);
		bool operator!=(const Vec3& w);


		// methods
		Vec3& normalize();
		float lenght() const;

		// friend function 
		friend Vec3 normalize(const Vec3& v);
		friend float dot(const Vec3& v1, const Vec3& v2);
		friend Vec3 cross(const Vec3& v1, const Vec3& v2);
		friend float lenght(const Vec3& v);
};


// matrix transforms
Vec3& operator*=(Vec3& v, const Matrix44& m);
Vec3 operator*(const Vec3& v, const Matrix44& m);
Vec3 operator*(const Matrix44& m, const Vec3& v);






// utility functions ///////////////////////////////
// print vec3
std::ostream& operator<<(std::ostream& os, Vec3 v);




