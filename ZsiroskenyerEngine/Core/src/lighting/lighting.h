////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Provides function for physical light calculations and color management.
//	Light calculations:
//		- Rayleigh scattering on spectrum
//		- Spectrum to CIE XYZ color space
//		- XYZ <--> sRGB conversions
////////////////////////////////////////////////////////////////////////////////


#include "../math/vec3.h"


// Spectrum
class IntensitySpectrum {
public:
	// constructor
	IntensitySpectrum();

	// generate spectrum
	template<class IntensityFunc> // of signature float(*)(float) or compatible
	void Generate(IntensityFunc f);
	void BlackBody(float temperature);
	void Constant(float value);
	void Scale(float c); // scales entire spectrum

	// convert to whatever
	Vec3 TosRGB();
	Vec3 ToXYZ();

	// operators
	IntensitySpectrum& operator+=(const IntensitySpectrum& other);
	IntensitySpectrum& operator-=(const IntensitySpectrum& other);

	// access intensity by wavelength
	float operator[](int wavelength); // index by wavelenght: ONLY 380-780
private:
	float intensity[780 - 380];
	static float __dummy;
};


// Conversion
class LightConvert {
	static Vec3 RGBToXYZ(Vec3 rgb);
	static Vec3 XYZToRGB(Vec3 rgb);
	static Vec3 SpectrumToRGB(IntensitySpectrum spectrum);
	static Vec3 SpectrumToXYZ(IntensitySpectrum spectrum);
};


// Rayleigh scattering
// params:
//	spectrum: scattered away light is removed from this
//	coefficient: how much air, how much it causes scattering, @550nm
void Rayleigh(IntensitySpectrum& spectrum, float coefficient) {
	
}