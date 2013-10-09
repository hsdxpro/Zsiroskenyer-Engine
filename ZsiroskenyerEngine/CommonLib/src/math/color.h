/////////////////////////////////////////////////////
//	Colorf ---> float4
//	Color ----> DWORD
//	color structs
/////////////////////////////////////////////////////
#pragma once



struct Colorf {
	public:
		// members
		float r,g,b,a;

		// constructors
		Colorf() {a=r=g=b=0.f;}
		Colorf(float m) {a=1.f; r=g=b=m;}
		Colorf(float r, float g, float b, float a) {
			this->a = a;
			this->r = r;
			this->g = g;
			this->b = b;
		}
		Colorf(float r, float g, float b) {
			this->a = 1.f;
			this->r = r;
			this->g = g;
			this->b = b;
		}

		// operators
		Colorf& operator*=(const Colorf& c);
		Colorf& operator/=(const Colorf& c);
		Colorf& operator+=(const Colorf& c);
		Colorf& operator-=(const Colorf& c);

		Colorf& operator*=(const float& s);
		Colorf& operator/=(float s);

		Colorf operator*(const Colorf& c);
		Colorf operator/(const Colorf& c);
		Colorf operator+(const Colorf& c);
		Colorf operator-(const Colorf& c);

		Colorf operator*(const float& s);
		Colorf operator/(const float& s);

		// methods
		float lightness();
		Colorf& tomonochrome();
};