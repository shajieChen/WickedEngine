#include "wiInitializer.h"
#include "WickedEngine.h"

#include <thread>
#include <sstream>

using namespace std;

namespace wiInitializer
{
	bool initializationStarted = false;
	uint64_t barrier = 0;

	void InitializeComponentsImmediate()
	{
		InitializeComponentsAsync();
		wiJobSystem::DrainBarrier(barrier);
	}
	void InitializeComponentsAsync()
	{
		initializationStarted = true;

		wiBackLog::post("\n[wiInitializer] Initializing Wicked Engine, please wait...\n");

		wiJobSystem::Initialize();

		wiJobSystem::Execute([] { wiFont::Initialize(); });
		wiJobSystem::Execute([] { wiImage::Initialize(); });
		wiJobSystem::Execute([] { wiInputManager::Initialize(); });
		wiJobSystem::Execute([] { wiRenderer::Initialize(); wiWidget::LoadShaders(); });
		wiJobSystem::Execute([] { wiSoundEffect::Initialize(); wiMusic::Initialize(); });
		wiJobSystem::Execute([] { wiTextureHelper::Initialize(); });
		wiJobSystem::Execute([] { wiSceneSystem::wiHairParticle::Initialize(); });
		wiJobSystem::Execute([] { wiSceneSystem::wiEmittedParticle::Initialize(); });
		wiJobSystem::Execute([] { wiLensFlare::Initialize(); });
		wiJobSystem::Execute([] { wiOcean::Initialize(); });
		wiJobSystem::Execute([] { wiGPUSortLib::LoadShaders(); });
		wiJobSystem::Execute([] { wiGPUBVH::LoadShaders(); });
		wiJobSystem::Execute([] { wiPhysicsEngine::Initialize(); });
		barrier = wiJobSystem::Barrier();

	}

	bool IsInitializeFinished()
	{
		return initializationStarted && wiJobSystem::IsBarrierReached(barrier);
	}
}