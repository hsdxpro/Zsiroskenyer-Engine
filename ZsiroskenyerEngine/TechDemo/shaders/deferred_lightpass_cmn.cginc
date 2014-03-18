////////////////////////////////////////////////////////////////////////////////
//	Common functionality for deferred shading lighting pass shaders.
//
////////////////////////////////////////////////////////////////////////////////

#include "common.cginc"

//------------------------------------------------------------------------------
//	Common shader constants
//------------------------------------------------------------------------------
float4x4 viewProj	 : register(c0);
float4x4 invViewProj : register(c4);
float3 camPos		 : register(c8);
float3 lightColor	 : register(c9);
float3 lightPos		 : register(c10);
float3 lightDir		 : register(c11);
float lightRange	 : register(c12);
float3 lightAtten	 : register(c13);
float2 lightAngle	 : register(c14);
float nearPlane		 : register(c15);
float farPlane		 : register(c16);


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

// Sample and decode whole g-buffer
void SampleGBuffer(in float2 texCoord, out float3 diffuse, out float3 normal, out float glossiness, out float specLevel) {
	GBUFFER gb;
	gb.diffuse = tex2D(gBuffer0, texCoord);		// diffuse.rgb & alpha(unused)
	gb.normal = tex2D(gBuffer1, texCoord).xy;	// packed normal
	gb.misc = tex2D(gBuffer2, texCoord);		// specular(2) / unused(2)
	
	DecodeGBuffer(gb, diffuse, normal, glossiness, specLevel);
}

//------------------------------------------------------------------------------
//	Lighting term calculation
//------------------------------------------------------------------------------

float Fresnel(float NdotV, float fresnelBias, float fresnelPow) {
	float facing = (1.0 - NdotV);
	return max(fresnelBias + (1.0 - fresnelBias) * pow(facing, fresnelPow), 0.0);
}

float CookTorranceSpecular(float3 N, float3 viewDir, float3 L, float roughness, float IOR) {
	const float gaussConstant  = 10;

    // the things we need:	
    // normalized normal and vector to eye
    float3 Nn = N;
    float3 Vn = -viewDir;
    float Ktransmit;
    float m = roughness;
    float F = Fresnel( dot(N, Vn), 0, 1 / IOR);
    
    float cook = 0;
    float NdotV = dot(Nn, Vn);

    //half angle vector
    float3 Ln = L;
    float3 H = normalize(Vn + Ln);

    float NdotH = clamp(dot(Nn, H), 0.000001f, 1.0f);
    float NdotL = clamp(dot(Nn, Ln), 0.000001f, 1.0f);
    float VdotH = clamp(dot(Vn, H), 0.000001f, 1.0f);
    
    float alpha = acos(NdotH);
    
    //microfacet distribution
    float D = gaussConstant*0.398942280f*exp(-(alpha*alpha)/(m*m));
    
    //geometric attenuation factor
    float G = min(1, min((2 * NdotH * NdotV / VdotH), (2 * NdotH * NdotL / VdotH)));

    //sum contributions
	const float PISquare = 9.869604401f;
	//return 1.0f / NdotV / PISquare;
	//return G / PISquare;
	return F * D * G / NdotV / PISquare;
}

// diffuse terv
float DiffuseTerm(float3 lightDir, float3 normal) {
	return saturate(-dot(lightDir, normal));
}

// specular terv
float SpecularTerm(float3 lightDir, float3 normal, float3 viewDir, float glossiness) {
	return CookTorranceSpecular(normal, viewDir, lightDir, 1.0f - glossiness, 1.6f);
}

//------------------------------------------------------------------------------
//	Helper functions
//------------------------------------------------------------------------------

// light fading near edges
float Fade(float t) {
	t = saturate(t);
	return 3*t*t - 2*t*t*t;
}