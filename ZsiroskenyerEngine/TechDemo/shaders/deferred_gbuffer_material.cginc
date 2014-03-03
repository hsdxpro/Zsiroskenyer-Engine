////////////////////////////////////////////////////////////////////////////////
//	G-buffer material processing and pixel shader
//
////////////////////////////////////////////////////////////////////////////////

#include "common.cginc"

struct {
	// basic parameters
	float3 diffuseColor;
	float glossiness;
	float specularLevel;

	// texture properties
	bool hasDiffuseMap;
	bool hasNormalMap;
	bool hasGlossinessMap;
	bool hasSpecLevelMap;
	bool useCutout;
} c : register(c20);


//------------------------------------------------------------------------------
//	Pixel shader for no normal maps
//------------------------------------------------------------------------------

GBUFFER PixelShader_Simple (float3 normal, float2 texCoord0) {
	// fill basic from shader const
	float3 diffuse = c.diffuseColor;
	float gloss = c.glossiness;
	float specLvl = c.specularLevel;

	// fill textures
	float4 mapSample;

	// diffuse & cutout
	if (c.hasDiffuseMap) {
		mapSample = tex2D(diffuseMap, texCoord0);
		if (c.useCutout && mapSample.a < 0.5f)
			discard;
		diffuse *= mapSample.rgb;
	}

	// specular
	if (c.hasGlossinessMap) {
		float value = tex2D(glossinessMap, texCoord0);
		gloss *= value;
	}
	if (c.hasSpecLevelMap) {
		float value = tex2D(specLevelMap, texCoord0);
		specLvl *= value;
	}

	return EncodeGBuffer(diffuse, normal, gloss, specLvl);
}

//------------------------------------------------------------------------------
//	Pixel shader WITH normal maps
//------------------------------------------------------------------------------

GBUFFER PixelShader_NormalMap (float3x3 TBN, float2 texCoord) {
	// normal
	float3 normal =
		normalize(mul(tex2D(normalMap, texCoord).xyz * 2.0f - float3(1.f,1.f,1.f), TBN));

	return PixelShader_Simple(normal, texCoord);
}