#include <pch.h>
#include "NoCD.h"

namespace cheat::feature
{
	static void PlayerSkillCd_ReduceSkillSection_Hook(app::PlayerSkillCd* __this, int32_t skillId, bool beginResume, MethodInfo* method);
	static void PlayerSkillCd_ResetUseInterval_Hook(app::PlayerSkillCd* __this, int32_t skillId, MethodInfo* method);
	static void PlayerSkillCd_ClearEnergy_Hook(app::PlayerSkillCd* __this, MethodInfo* method);
	static void SkillInfo_set_currentEnergy_Hook(app::SkillInfo* __this, app::FP value, MethodInfo* method);

	NoCD& NoCD::GetInstance()
	{
		static NoCD instance;
		return instance;
	}

	NoCD::NoCD() : Feature(),
		f_UnlimitedEnergy(false),
		f_NoCooldown(false)
	{
		HookManager::install(app::PlayerSkillCd_ReduceSkillSection, PlayerSkillCd_ReduceSkillSection_Hook);
		HookManager::install(app::PlayerSkillCd_ResetUseInterval, PlayerSkillCd_ResetUseInterval_Hook);
		HookManager::install(app::PlayerSkillCd_ClearEnergy, PlayerSkillCd_ClearEnergy_Hook);
		HookManager::install(app::SkillInfo_set_currentEnergy, SkillInfo_set_currentEnergy_Hook);
	}

	const FeatureGUIInfo& NoCD::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ "Cooldown Effect", "Player", true };
		return info;
	}

	void NoCD::DrawMain()
	{
		ImGui::Checkbox("No Cooldown", &f_NoCooldown);
		ImGui::Checkbox("Unlimited Energy", &f_UnlimitedEnergy);
	}

	static void PlayerSkillCd_ClearEnergy_Hook(app::PlayerSkillCd* __this, MethodInfo* method) {
		NoCD& noCD = NoCD::GetInstance();
		if (noCD.f_UnlimitedEnergy) return;
		return CALL_ORIGIN(PlayerSkillCd_ClearEnergy_Hook, __this, method);
	}

	static void PlayerSkillCd_ReduceSkillSection_Hook(app::PlayerSkillCd* __this, int32_t skillId, bool beginResume, MethodInfo* method) {
		NoCD& noCD = NoCD::GetInstance();
		if (noCD.f_NoCooldown) return;
		return CALL_ORIGIN(PlayerSkillCd_ReduceSkillSection_Hook, __this, skillId, beginResume, method);
	
	}

	static void PlayerSkillCd_ResetUseInterval_Hook(app::PlayerSkillCd* __this, int32_t skillId, MethodInfo* method) {
		NoCD& noCD = NoCD::GetInstance();
		if (noCD.f_NoCooldown) return;
		return CALL_ORIGIN(PlayerSkillCd_ResetUseInterval_Hook, __this, skillId, method);
	}

	static void SkillInfo_set_currentEnergy_Hook(app::SkillInfo* __this, app::FP value, MethodInfo* method) {
		NoCD& noCD = NoCD::GetInstance();
		if (noCD.f_UnlimitedEnergy) {
			int32_t energy = app::SkillInfo_get_totalEnergy(__this, nullptr);
			value = app::FP_op_Implicit(energy, nullptr);
		}
		return CALL_ORIGIN(SkillInfo_set_currentEnergy_Hook, __this, value, method);
	}
}