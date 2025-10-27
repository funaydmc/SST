#include <pch.h>
#include "GodMode.h"

namespace cheat::feature
{
	static bool AdventureActor_OnHitActor_Hook(app::AdventureActor* __this, app::HitBox* hitBox, int32_t uniqueAttackId, int32_t onceAttackTargetCount, app::LogicEntity* actor, app::DeterministicRaycastHit* raycastHit, bool* damaged, app::GameObject* hurtEffectPrefab, bool isHittedEffectScale, bool effectIgnoreTimeScale, MethodInfo* method);

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

	static bool AdventureActor_OnHitActor_Hook(app::AdventureActor* __this, app::HitBox* hitBox, int32_t uniqueAttackId, int32_t onceAttackTargetCount, app::LogicEntity* actor, app::DeterministicRaycastHit* raycastHit, bool* damaged, app::GameObject* hurtEffectPrefab, bool isHittedEffectScale, bool effectIgnoreTimeScale, MethodInfo* method)
	{
		auto& godMode = GodMode::GetInstance();
		if (godMode.f_GodMode && strcmp(__this->klass->_0.name, "PlayerAdventureActor") != 0) {
			*damaged = true;
			return true;
		}

		return CALL_ORIGIN(AdventureActor_OnHitActor_Hook, __this, hitBox, uniqueAttackId, onceAttackTargetCount, actor, raycastHit, damaged, hurtEffectPrefab, isHittedEffectScale, effectIgnoreTimeScale, method);
	}
}