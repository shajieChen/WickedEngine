#include "atmosphereHF.hlsli"


[numthreads(1, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	float w, h, d;
	_InscatteringLUT.GetDimensions(w, h, d);

	float2 coords = float2(id.x / (w - 1), id.y / (h - 1));

	float3 v1 = lerp(_BottomLeftCorner.xyz, _BottomRightCorner.xyz, coords.x);
	float3 v2 = lerp(_TopLeftCorner.xyz, _TopRightCorner.xyz, coords.x);

	float3 rayEnd = lerp(v1, v2, coords.y);
	float3 rayStart = _CameraPos.xyz;

	float3 rayDir = rayEnd - rayStart;
	float rayLength = length(rayDir);
	rayDir /= rayLength;

	float3 planetCenter = float3(0, -_PlanetRadius, 0);
	PrecomputeLightScattering(rayStart, rayDir, rayLength, planetCenter, normalize(_LightDir), id, d);
}

