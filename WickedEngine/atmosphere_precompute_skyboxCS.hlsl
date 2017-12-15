#include "atmosphereHF.hlsli"


[numthreads(1, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	float w, h, d;
	_SkyboxLUT.GetDimensions(w, h, d);

	// linear parameters
	float3 coords = float3(id.x / (w - 1), id.y / (h - 1), id.z / (d - 1));

	float height = coords.x * coords.x * _AtmosphereHeight;
	//float height = coords.x * _AtmosphereHeight;
	float ch = -(sqrt(height * (2 * _PlanetRadius + height)) / (_PlanetRadius + height));

	float viewZenithAngle = coords.y;
	//float viewZenithAngle = coords.y * 2.0 - 1.0;

	if (viewZenithAngle > 0.5)
	{
		viewZenithAngle = ch + pow((viewZenithAngle - 0.5) * 2, 5) * (1 - ch);
	}
	else
	{
		viewZenithAngle = ch - pow(viewZenithAngle * 2, 5) * (1 + ch);
	}

	float sunZenithAngle = (tan((2 * coords.z - 1 + 0.26)*0.75)) / (tan(1.26 * 0.75));// coords.z * 2.0 - 1.0;
																					  //float sunZenithAngle = coords.z * 2.0 - 1.0;

	float3 planetCenter = float3(0, -_PlanetRadius, 0);
	float3 rayStart = float3(0, height, 0);

	float3 rayDir = float3(sqrt(saturate(1 - viewZenithAngle * viewZenithAngle)), viewZenithAngle, 0);
	float3 lightDir = -float3(sqrt(saturate(1 - sunZenithAngle * sunZenithAngle)), sunZenithAngle, 0);

	float rayLength = 1e20;
	float2 intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, _PlanetRadius + _AtmosphereHeight);
	rayLength = intersection.y;

	intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, _PlanetRadius);
	if (intersection.x > 0)
		rayLength = min(rayLength, intersection.x);

	ScatteringOutput scattering = IntegrateInscattering(rayStart, rayDir, rayLength, planetCenter, lightDir);

	//color.inscattering.z = coords.z;
	_SkyboxLUT[id.xyz] = float4(scattering.rayleigh.xyz, scattering.mie.x);

#ifdef HIGH_QUALITY
	_SkyboxLUT2[id.xyz] = scattering.mie.yz;
#endif
}

