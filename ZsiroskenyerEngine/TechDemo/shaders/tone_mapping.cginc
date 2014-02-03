////////////////////////////////////////////////////////////////////////////////
//	Tone mapping algorithms.
//
////////////////////////////////////////////////////////////////////////////////


#include "common_color.cginc"


// Lightness perception function: x E [0, 3]
float EyeResponse(float luminance) {
	return tanh((luminance-1.5f)*1.2f)*0.5f+0.5f;
}

// Remap lightness to LDR
	// parameters log scale
	// output range is [0, 1]
float RemapLuminance(float luminance, float adaptation) {
	return EyeResponse(luminance-adaptation+1.5f);
}


// Tone-mapper
	// params in linear
float3 ToneMap(float3 color, float adaptation) {
	float inLum = Luminance(color);
	float outLum = RemapLuminance(log10(inLum), log10(adaptation));
	color = AdjustLuminance(color, outLum);
	//return color; /*
	return SaturateToFit(color);//*/
}


// Lens effect
	// params in linear
float3 LensEffect(float3 color, float adaptation) {
	// lens effect occurs 0.5 brithness level above log-adaptation
	float3 treshold = AdjustLuminance(color, adaptation*3.16227f);

	color = max(float3(0.0f, 0.0f, 0.0f), color-treshold);
	return color;
}