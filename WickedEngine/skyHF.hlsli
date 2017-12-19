#ifndef _SKY_HF_
#define _SKY_HF_
#include "globals.hlsli"
#include "lightingHF.hlsli"

#define ATMOSPHERE_REFERENCE
#define RENDER_SUN
#include "atmosphereHF.hlsli"

inline float3 GetSkyColor(in float3 normal)
{
	float3 _CameraPos = g_xFrame_MainCamera_CamPos;
	float3 _WorldSpaceLightPos0 = -GetSunDirection();

#ifdef ATMOSPHERE_REFERENCE
	float3 rayStart = _CameraPos;
	//float3 rayDir = normalize(mul((float3x3)_Object2World, i.vertex));
	float3 rayDir = normal;

	float3 lightDir = -_WorldSpaceLightPos0.xyz;

	float3 planetCenter = _CameraPos;
	planetCenter = float3(0, -_PlanetRadius, 0);

	float2 intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, _PlanetRadius + _AtmosphereHeight);
	float rayLength = intersection.y;

	intersection = RaySphereIntersection(rayStart, rayDir, planetCenter, _PlanetRadius);
	if (intersection.x > 0)
		rayLength = min(rayLength, intersection.x);

	float4 extinction;
	//float4 inscattering = IntegrateInscattering(rayStart, rayDir, rayLength, planetCenter, 1, lightDir, 16, extinction);
	//return float4(inscattering.xyz, 1);

	ScatteringOutput scatter = IntegrateInscattering(rayStart, rayDir, rayLength, planetCenter, lightDir);
	return float4(scatter.mie, 1);
#else
	float3 rayStart = _CameraPos;
	float3 rayDir = normalize(mul((float3x3)_Object2World, i.vertex));

	float3 lightDir = -_WorldSpaceLightPos0.xyz;

	float3 planetCenter = _CameraPos;
	planetCenter = float3(0, -_PlanetRadius, 0);

	float4 scatterR = 0;
	float4 scatterM = 0;

	float height = length(rayStart - planetCenter) - _PlanetRadius;
	float3 normal = normalize(rayStart - planetCenter);

	float viewZenith = dot(normal, rayDir);
	float sunZenith = dot(normal, -lightDir);

	float3 coords = float3(height / _AtmosphereHeight, viewZenith * 0.5 + 0.5, sunZenith * 0.5 + 0.5);

	coords.x = pow(height / _AtmosphereHeight, 0.5);
	float ch = -(sqrt(height * (2 * _PlanetRadius + height)) / (_PlanetRadius + height));
	if (viewZenith > ch)
	{
		coords.y = 0.5 * pow((viewZenith - ch) / (1 - ch), 0.2) + 0.5;
	}
	else
	{
		coords.y = 0.5 * pow((ch - viewZenith) / (ch + 1), 0.2);
	}
	coords.z = 0.5 * ((atan(max(sunZenith, -0.1975) * tan(1.26*1.1)) / 1.1) + (1 - 0.26));

	scatterR = tex3D(_SkyboxLUT, coords);

#ifdef HIGH_QUALITY
	scatterM.x = scatterR.w;
	scatterM.yz = tex3D(_SkyboxLUT2, coords).xy;
#else
	scatterM.xyz = scatterR.xyz * ((scatterR.w) / (scatterR.x));// *(_ScatteringR.x / _ScatteringM.x) * (_ScatteringM / _ScatteringR);
#endif

	float3 m = scatterM;
	//scatterR = 0;
	// phase function
	ApplyPhaseFunctionElek(scatterR.xyz, scatterM.xyz, dot(rayDir, -lightDir.xyz));
	float3 lightInscatter = (scatterR * _ScatteringR + scatterM * _ScatteringM) * _IncomingLight.xyz;
#ifdef RENDER_SUN
	lightInscatter += RenderSun(m, dot(rayDir, -lightDir.xyz)) * _SunIntensity;
#endif

	return float4(max(0, lightInscatter), 1);
#endif




	//static const float overBright = 1.02f;

	//normal = normalize(normal) * overBright;

	//float3 col = DEGAMMA(texture_env_global.SampleLevel(sampler_linear_clamp, normal, 0).rgb);
	//float3 sun = max(pow(abs(dot(GetSunDirection(), normal)), 256)*GetSunColor(), 0) * saturate(dot(GetSunDirection().xyz, normal));

	//return col + sun;
}


#endif // _SKY_HF_
