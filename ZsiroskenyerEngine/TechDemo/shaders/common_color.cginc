////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Common stuff in connection with lighting and color, color spaces.
//
////////////////////////////////////////////////////////////////////////////////


// Compute luminance of a color
float Luminance(float3 color) {
	return 0.2126f*color.r + 0.7152*color.g + 0.0722*color.b;
}

// Normalize a color (luminance=1)
float3 NormalizeLuminance(float3 color) {
	float luminance = max(1e-20f, Luminance(color));
	color /= luminance;
	return color;
}

// Scale color to certain luminance
float3 AdjustLuminance(float3 color, float luminance) {
	return NormalizeLuminance(color)*luminance;
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