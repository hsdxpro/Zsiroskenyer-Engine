////////////////////////////////////////////////////////////////////////////////
//	G-buffer material processing and pixel shader
//
////////////////////////////////////////////////////////////////////////////////

#include "common.cginc"

// basic parameters
static const float4 diffuseColor = float4(1,1,1,0);
static const float glossiness = 0.5f;
static const float specularLevel = 0.2f;

// texture properties
static const bool hasDiffuseMap = true;
static const bool hasNormalMap = true;
static const bool hasGlossinessMap = false;
static const bool hasSpecLevelMap = false;
static const bool useCutout = true;

// texture maps
sampler2D diffuseMap = {
	MipFilter = LINEAR,
    MinFilter = LINEAR,
    MagFilter = LINEAR,
	AddressU = WRAP,
	AddressV = WRAP,
};

sampler2D normalMap = {
	MipFilter = LINEAR,
    MinFilter = LINEAR,
   	MagFilter = LINEAR,
	AddressU = WRAP,
	AddressV = WRAP,
};

sampler2D glossinessMap = {
	MipFilter = LINEAR,
    MinFilter = LINEAR,
    MagFilter = LINEAR,
	AddressU = WRAP,
	AddressV = WRAP,
};

sampler2D specLevelMap = {
	MipFilter = LINEAR,
    MinFilter = LINEAR,
    MagFilter = LINEAR,
	AddressU = WRAP,
	AddressV = WRAP,
};

//------------------------------------------------------------------------------
//	Pixel shader for no normal maps
//------------------------------------------------------------------------------

GBUFFER PixelShader_Simple (
	float3 normal,
	float2 texCoord
)
{
	// fill basic from shader const
	float3 diffuse = diffuseColor;
	float gloss = glossiness;
	float specLvl = specularLevel;

	// fill textures
	float4 mapSample;

	// diffuse & cutout
	if (hasDiffuseMap) {
		mapSample = tex2D(diffuseMap, texCoord);
		if (useCutout && mapSample.a < 0.5f)
			discard;
		diffuse *= mapSample.rgb;
	}

	// specular
	if (hasGlossinessMap) {
		float value = tex2D(glossinessMap, texCoord);
		gloss *= value;
	}
	if (hasSpecLevelMap) {
		float value = tex2D(specLevelMap, texCoord);
		specLvl *= value;
	}

	return EncodeGBuffer(diffuse, normal, gloss, specLvl);
}

//------------------------------------------------------------------------------
//	Pixel shader WITH normal maps
//------------------------------------------------------------------------------

GBUFFER PixelShader_NormalMap (
	float3x3 TBN,
	float2 texCoord
)
{
	// fill basic from shader const
	float3 diffuse = diffuseColor;
	float gloss = glossiness;
	float specLvl = specularLevel;

	// normal
	float3 normal = 
		normalize(mul(tex2D(normalMap, texCoord).xyz * 2.0f - float3(1.f,1.f,1.f), TBN));

	// fill textures
	float4 mapSample;

	// diffuse & cutout
	if (hasDiffuseMap) {
		mapSample = tex2D(diffuseMap, texCoord);
		if (useCutout && mapSample.a < 0.5f)
			discard;
		diffuse *= mapSample.rgb;
	}

	// specular
	if (hasGlossinessMap) {
		float value = tex2D(glossinessMap, texCoord);
		gloss *= value;
	}
	if (hasSpecLevelMap) {
		float value = tex2D(specLevelMap, texCoord);
		specLvl *= value;
	}

	return EncodeGBuffer(diffuse, normal, gloss, specLvl);
}