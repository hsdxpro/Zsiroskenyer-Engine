////////////////////////////////////////////////////////////////////////////////
//	Tone mapping algorithms.
//
////////////////////////////////////////////////////////////////////////////////




// Compute luminance of a color
float Luminance(float3 color) {
	return 0.2126f*color.r + 0.7152*color.g + 0.0722*color.b;
}

// Normalize a color (luminance=1)
float3 NormalizeColor(float3 color) {
	float luminance = max(1e-20f, Luminance(color));
	color /= luminance;
	return color;
}

// Scale color to certain luminance
float3 AdjustLuminance(float3 color, float luminance) {
	return NormalizeColor(color)*luminance;
}

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

// Saturate to fit
	// avoids clamping of colors
	// desaturates while keeping luminance
	// use only with colors of luminance from 0 to 1
	// WARNING: if no clamping would occur, it DOES NOTHING
float3 SaturateToFit(float3 color) {
	float m = max(color.r, max(color.g, color.b));
	float l = Luminance(color);
	if (l-m < 0.0f) {
		float t = min((l-1.0f)/(l-m), 1.0f);
		float3 base = float3(l,l,l);
		return base + (color-base)*t;
	}
	else {
		return color;
	}
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