////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	Commonly used shader algorithms and helper functions
//
////////////////////////////////////////////////////////////////////////////////


// Convert from clip space to world space
float3 GetWorldPosition(float2 screenCoords, float depth, float4x4 invViewProj) {
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