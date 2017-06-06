#include "globals.hlsli"
#include "grassHF_GS.hlsli"
#include "cullingShaderHF.hlsli"

static const uint vertexBuffer_stride = 16 * 3; // pos, normal, tangent 
RAWBUFFER(vertexBuffer, 0);

RWRAWBUFFER(argumentBuffer, 0); // indirect draw args
RWRAWBUFFER(indexBuffer, 1);

groupshared Frustum frustum;


[numthreads(GRASS_CULLING_THREADCOUNT_TILED, GRASS_CULLING_THREADCOUNT_TILED, 1)]
void main(ComputeShaderInput IN)
{
	if (IN.groupIndex == 0)
	{
		// View space eye position is always at the origin.
		const float3 eyePos = float3(0, 0, 0);

		// Compute 4 points on the far clipping plane to use as the 
		// frustum vertices.
		float4 screenSpace[4];
		// Top left point
		screenSpace[0] = float4(IN.groupID.xy * GRASS_CULLING_THREADCOUNT_TILED, 1.0f, 1.0f);
		// Top right point
		screenSpace[1] = float4(float2(IN.groupID.x + 1, IN.groupID.y) * GRASS_CULLING_THREADCOUNT_TILED, 1.0f, 1.0f);
		// Bottom left point
		screenSpace[2] = float4(float2(IN.groupID.x, IN.groupID.y + 1) * GRASS_CULLING_THREADCOUNT_TILED, 1.0f, 1.0f);
		// Bottom right point
		screenSpace[3] = float4(float2(IN.groupID.x + 1, IN.groupID.y + 1) * GRASS_CULLING_THREADCOUNT_TILED, 1.0f, 1.0f);

		float3 viewSpace[4];
		// Now convert the screen space points to view space
		for (int i = 0; i < 4; i++)
		{
			viewSpace[i] = ScreenToView(screenSpace[i]).xyz;
		}

		// Left plane
		frustum.planes[0] = ComputePlane(viewSpace[2], eyePos, viewSpace[0]);
		// Right plane
		frustum.planes[1] = ComputePlane(viewSpace[1], eyePos, viewSpace[3]);
		// Top plane
		frustum.planes[2] = ComputePlane(viewSpace[0], eyePos, viewSpace[1]);
		// Bottom plane
		frustum.planes[3] = ComputePlane(viewSpace[3], eyePos, viewSpace[2]);
	}

	GroupMemoryBarrierWithGroupSync();

	for (uint i = IN.groupIndex; i < xParticleCount; i += GRASS_CULLING_THREADCOUNT_TILED * GRASS_CULLING_THREADCOUNT_TILED)
	{
		const uint fetchAddress = i * vertexBuffer_stride;
		float4 pos = float4(asfloat(vertexBuffer.Load3(fetchAddress)), 1);
		pos = mul(pos, xWorld);
		pos = mul(pos, g_xFrame_MainCamera_View);

		if (PointInsideFrustum(pos.xyz, frustum, 1, LOD2))
		{
			uint indexCount;
			argumentBuffer.InterlockedAdd(0, 1, indexCount);
			indexBuffer.Store(indexCount * 4, i);
		}
	}
}