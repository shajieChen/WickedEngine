#include "globals.hlsli"
#include "cullingShaderHF.hlsli"
#include "lightingHF.hlsli"
#include "envReflectionHF.hlsli"
#include "packHF.hlsli"
#include "reconstructPositionHF.hlsli"

RWTEXTURE2D(deferred_Diffuse, float4, 0);
RWTEXTURE2D(deferred_Specular, float4, 1);

[numthreads(TILED_RENDERING_BLOCKSIZE, TILED_RENDERING_BLOCKSIZE, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint2 pixel = DTid.xy;
	if (pixel.x > (uint)GetInternalResolution().x || pixel.y > (uint)GetInternalResolution().y)
	{
		return;
	}

	float depth = texture_depth[pixel];

	uint2 tileIndex = uint2(floor(pixel / TILED_CULLING_BLOCKSIZE));
	uint startOffset = flatten2D(tileIndex, g_xWorld_EntityCullingTileCount.xy) * MAX_SHADER_ENTITY_COUNT_PER_TILE;
	uint arrayProperties = EntityIndexList[startOffset];
	uint arrayLength = arrayProperties & 0x00FFFFFF; // count of every element in the tile
	uint decalCount = (arrayProperties & 0xFF000000) >> 24; // count of just the decals in the tile
	startOffset += 1; // first element was the itemcount
	uint iterator = 0;

	float3 diffuse = 0, specular = 0;
	float4 baseColor = texture_gbuffer0[pixel];
	float4 g1 = texture_gbuffer1[pixel];
	float4 g3 = texture_gbuffer3[pixel];
	float3 N = decode(g1.xy);
	float roughness = g3.x;
	float reflectance = g3.y;
	float metalness = g3.z;
	float ao = g3.w;
	BRDF_HELPER_MAKEINPUTS(baseColor, reflectance, metalness);
	float3 P = getPosition((float2)pixel * g_xWorld_InternalResolution_Inverse, depth);
	float3 V = normalize(g_xFrame_MainCamera_CamPos - P);

	// decals are disabled, set the iterator to skip decals:
	iterator = decalCount;

	[loop]
	for (; iterator < arrayLength; iterator++)
	{
		ShaderEntityType light = EntityArray[EntityIndexList[startOffset + iterator]];

		LightingResult result = (LightingResult)0;

		switch (light.type)
		{
		case ENTITY_TYPE_DIRECTIONALLIGHT:
		{
			result = DirectionalLight(light, N, V, P, roughness, f0);
		}
		break;
		case ENTITY_TYPE_POINTLIGHT:
		{
			result = PointLight(light, N, V, P, roughness, f0);
		}
		break;
		case ENTITY_TYPE_SPOTLIGHT:
		{
			result = SpotLight(light, N, V, P, roughness, f0);
		}
		break;
		case ENTITY_TYPE_SPHERELIGHT:
		{
			result = SphereLight(light, N, V, P, roughness, f0);
		}
		break;
		case ENTITY_TYPE_DISCLIGHT:
		{
			result = DiscLight(light, N, V, P, roughness, f0);
		}
		break;
		case ENTITY_TYPE_RECTANGLELIGHT:
		{
			result = RectangleLight(light, N, V, P, roughness, f0);
		}
		break;
		case ENTITY_TYPE_TUBELIGHT:
		{
			result = TubeLight(light, N, V, P, roughness, f0);
		}
		break;
		}

		diffuse += max(0.0f, result.diffuse);
		specular += max(0.0f, result.specular);
	}

	VoxelRadiance(N, V, P, f0, roughness, diffuse, specular, ao);

	specular = max(specular, EnvironmentReflection(N, V, P, roughness, f0));

	deferred_Diffuse[pixel] = float4(diffuse, ao);
	deferred_Specular[pixel] = float4(specular, 1);

}