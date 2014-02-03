

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