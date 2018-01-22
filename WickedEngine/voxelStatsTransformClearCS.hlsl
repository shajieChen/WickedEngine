#include "globals.hlsli"

RWRAWBUFFER(voxelstats, 0);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint voxelcount = voxelstats.Load(4 * 3);

	// fill indirect dispatch args:
	voxelstats.Store3(0, uint3(ceil((float)voxelcount / 1024.0f), 1, 1));

	// clear counter stat:
	voxelstats.Store(4 * 3, 0);

	// fill indirect draw args:
	voxelstats.Store4(4 * 4, uint4(voxelcount, 1, 0, 0));
}