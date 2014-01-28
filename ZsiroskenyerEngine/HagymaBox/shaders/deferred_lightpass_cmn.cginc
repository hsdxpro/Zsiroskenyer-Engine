////////////////////////////////////////////////////////////////////////////////
//	Common functionality for deferred shading lighting pass shaders.
//
////////////////////////////////////////////////////////////////////////////////


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


// Convert from clip space to world space
float3 GetWorldPosition(float2 screenCoords, float depth) {
	screenCoords.x = 2.0f * (screenCoords.x - 0.5f);
	screenCoords.y = -2.0f * (screenCoords.y - 0.5f);
	float4 posH = float4(screenCoords, depth, 1.0f); // NDC space
	float4 posW = mul(posH, invViewProj);
	posW /= posW.w;
	return posW.xyz;
}

// Unpack normals
float3 UnpackNormal(float2 packedNormal) {
	float3 normal;
	normal.x = packedNormal.r;
	normal.y = 2.0f*(packedNormal.g>0.0f ? packedNormal.g-0.5f:packedNormal.g+0.5f);
	normal.z = sqrt(clamp(1.0f - normal.x*normal.x - normal.y*normal.y, 0, 1)) * (packedNormal.g>0.0f ? 1.0f:-1.0f);
	return normal;
}

// Decode whole g-buffer
void DecodeGBuffer(in float2 texCoord, out float3 diffuse, out float3 normal, out float3 worldPos, out float depth) {
	float4 gb0 = tex2D(gBuffer0, texCoord); // diffuse.rgb & alpha(unused)
	float2 gb1 = tex2D(gBuffer1, texCoord); // normal
	float4 gb2 = tex2D(gBuffer2, texCoord); // unused
	depth = tex2D(depthBuffer, texCoord); // depth
	
	diffuse = gb0.rgb;
	normal = UnpackNormal(gb1);
	worldPos = GetWorldPosition(texCoord, depth);	
}

//------------------------------------------------------------------------------
//	Lighting term calculation
//------------------------------------------------------------------------------

// diffuse light
float3 DiffuseLight(float3 lightDir, float3 lightColor, float3 normal) {
	float c = clamp(-dot(lightDir, normal), 0.0, 1.0);
	return c * lightColor;
}

// specular light
float3 SpecularLight(float3 lightDir, float3 lightColor, float3 normal, float3 viewDir, float glossiness) {
	float specFactor = 0.0f;
	return lightColor * specFactor;;
}

//------------------------------------------------------------------------------
//	Helper functions
//------------------------------------------------------------------------------

// light fading near edges
float Fade(float t) {
	t = saturate(t);
	return 3*t*t - 2*t*t*t;
}