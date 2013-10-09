#include "color.h"



 

// operators
Colorf& Colorf::operator*=(const Colorf& c) {
		//a*=c.a;
	r*=c.r;
	g*=c.g;
	b*=c.b;
	return *this;
}
Colorf& Colorf::operator/=(const Colorf& c) {
		//a/=c.a;
	r/=c.r;
	g/=c.g;
	b/=c.b;
	return *this;
}
Colorf& Colorf::operator+=(const Colorf& c) {
		//a+=c.a;
	r+=c.r;
	g+=c.g;
	b+=c.b;
	return *this;
}
Colorf& Colorf::operator-=(const Colorf& c) {
		//a-=c.a;
	r-=c.r;
	g-=c.g;
	b-=c.b;
	return *this;
}

Colorf& Colorf::operator*=(const float& s) {
		//a*=s;
	r*=s;
	g*=s;
	b*=s;
	return *this;
}
Colorf& Colorf::operator/=(float s) {
	s=1.f/s;
		//a*=s;
	r*=s;
	g*=s;
	b*=s;
	return *this;
}

Colorf Colorf::operator*(const Colorf& c) {
	Colorf cr=*this;
	return cr*=c;
}
Colorf Colorf::operator/(const Colorf& c) {
	Colorf cr=*this;
	return cr/=c;
}
Colorf Colorf::operator+(const Colorf& c) {
	Colorf cr=*this;
	return cr+=c;
}
Colorf Colorf::operator-(const Colorf& c) {
	Colorf cr=*this;
	return cr-=c;
}

Colorf Colorf::operator*(const float& s) {
	Colorf cr=*this;
	return cr*=s;
}
Colorf Colorf::operator/(const float& s) {
	Colorf cr=*this;
	return cr/=s;
}



// methods
float Colorf::lightness() {
	return (r+g+b)*0.3333333f;
}
Colorf& Colorf::tomonochrome() {
	(*this)*=this->lightness();
	return *this;
}