#include <pch.h>
#include "GodMode.h"

namespace cheat::feature
{
	static bool AdventureActor_OnHitActor_Hook(void* hitBox, int uniqueAttackId, int onceAttackTargetCount, void* actor, void* raycastHit, bool damaged, void* hurtEffectPrefab, bool isHittedEffectScale, bool effectIgnoreTimeScale);

	GodMode& GodMode::GetInstance()
	{
		static GodMode instance;
		return instance;
	}

	GodMode::GodMode() : Feature(),
		f_GodMode(false)
	{
		HookManager::install(app::AdventureActor_OnHitActor, AdventureActor_OnHitActor_Hook);
	}

	const FeatureGUIInfo& GodMode::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ "", "Player", false };
		return info;
	}

	void GodMode::DrawMain()
	{
		ImGui::Checkbox("God Mode", &f_GodMode);
	}

	static bool AdventureActor_OnHitActor_Hook(void* hitBox, int uniqueAttackId, int onceAttackTargetCount, void* actor, void* raycastHit, bool damaged, void* hurtEffectPrefab, bool isHittedEffectScale, bool effectIgnoreTimeScale)
	{
		auto& godMode = GodMode::GetInstance();
		if (godMode.f_GodMode) return false;

		return CALL_ORIGIN(AdventureActor_OnHitActor_Hook, hitBox, uniqueAttackId, onceAttackTargetCount, actor, raycastHit, damaged, hurtEffectPrefab, isHittedEffectScale, effectIgnoreTimeScale);
	}
}