////////////////////////////////////////////////////////////////////////////////
//	Common functionality for deferred shading lighting pass shaders.
//
////////////////////////////////////////////////////////////////////////////////

#include "common.cginc"

//------------------------------------------------------------------------------
//	Common shader constants
//------------------------------------------------------------------------------
float4x4 viewProj : register(c0);
float4x4 invViewProj : register(c4);
float4 camPos : register(c8);
float3 lightColor : register(c9);
float3 lightPos : register(c10);
float3 lightDir : register(c11);
float lightRange : register(c12);
float3 lightAtten : register(c13);
float2 lightAngle : register(c14);


//------------------------------------------------------------------------------
//	Processing G-Buffer
//------------------------------------------------------------------------------

// texture samplers
sampler2D gBuffer0 = {
    MipFilter = POINT,
    MinFilter = POINT,
    MagFilter = POINT,
};
sampler2D gBuffer1 = {
    MipFilter = POINT,
    MinFilter = POINT,
    MagFilter = POINT,
};
sampler2D gBuffer2 = {
    MipFilter = POINT,
    MinFilter = POINT,
    MagFilter = POINT,
};
sampler2D depthBuffer = {
    MipFilter = POINT,
    MinFilter = POINT,
    MagFilter = POINT,
};

// Decode whole g-buffer
void DecodeGBuffer(in float2 texCoord, out float3 diffuse, out float3 normal, out float3 worldPos, out float depth) {
	float4 gb0 = tex2D(gBuffer0, texCoord); // diffuse.rgb & alpha(unused)
	float2 gb1 = tex2D(gBuffer1, texCoord); // normal
	float4 gb2 = tex2D(gBuffer2, texCoord); // unused
	depth = tex2D(depthBuffer, texCoord); // depth
	
	diffuse = gb0.rgb;
	normal = UnpackNormal(gb1);
	worldPos = GetWorldPosition(texCoord, depth, invViewProj);	
}

//------------------------------------------------------------------------------
//	Lighting term calculation
//------------------------------------------------------------------------------

float Fresnel(float NdotV, float fresnelBias, float fresnelPow) {
  float facing = (1.0 - NdotV);
  return max(fresnelBias + (1.0 - fresnelBias) * pow(facing, fresnelPow), 0.0);
}

float CookTorranceSpecular(float3 N, float3 viewDir, float3 L, float roughness, float IOR)
{
	const float gaussConstant  = 100;

    //the things we need:	
    // normalized normal and vector to eye
    float3 Nn = normalize(N);
    float3 Vn = -viewDir;
    float Ktransmit;
    float m = roughness;
    float F = Fresnel( dot(N, Vn), 0, 1 / IOR);
    
    float cook = 0;
    float NdotV = dot(Nn, Vn);

    //half angle vector
    float3 Ln = normalize(L);
    float3 H = normalize(Vn + Ln);
    
    float NdotH = dot(Nn, H);
    float NdotL = dot(Nn, Ln);
    float VdotH = dot(Vn, H);
    
    float alpha = saturate(acos(NdotH));
    
    //microfacet distribution
    float D = gaussConstant*exp(-(alpha*alpha)/(m*m));
    
    //geometric attenuation factor
    float G = min(1, min((2 * NdotH * NdotV / VdotH), (2 * NdotH * NdotL / VdotH)));

    //sum contributions
	const float PISquare = 9.869604401f;
	return F * D * G / NdotV / PISquare;
}

// diffuse light
float3 DiffuseLight(float3 lightDir, float3 lightColor, float3 normal) {
	//float c = clamp(-dot(lightDir, normal), 0.0, 1.0);
	//return c * lightColor;
	return float3(0,0,0);
}

// specular light
float3 SpecularLight(float3 lightColor, float3 surfPosToLight, float3 normal, float3 viewDir, float glossiness) {
	float3 cook = lightColor * CookTorranceSpecular(normal, viewDir, normalize(surfPosToLight), 0.85f, 1.3);
	// UBER TODO WHY DO YOU CLAMP THAT AWESOME VALUE?
	return clamp(cook, 0, 1000000000);
}

//------------------------------------------------------------------------------
//	Helper functions
//------------------------------------------------------------------------------

// light fading near edges
float Fade(float t) {
	t = saturate(t);
	return 3*t*t - 2*t*t*t;
}