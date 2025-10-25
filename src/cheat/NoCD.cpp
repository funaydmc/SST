#include <pch.h>
#include "NoCD.h"

namespace cheat::feature
{
	static void PlayerSkillCd_ClearEnergy_Hook(void* __this);
	static void PlayerSkillCd_ReduceSkillSection_Hook(void* __this, int skillId, bool beginResume);
	static void PlayerSkillCd_ResetUseInterval_Hook(void* __this, int skillId);
	static void SkillInfo_set_currentEnergy_Hook(void* __this, int64_t value);

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

	static void PlayerSkillCd_ClearEnergy_Hook(void* __this) {
		NoCD& noCD = NoCD::GetInstance();
		if (noCD.f_UnlimitedEnergy) return;
		return CALL_ORIGIN(PlayerSkillCd_ClearEnergy_Hook, __this);
	}

	static void PlayerSkillCd_ReduceSkillSection_Hook(void* __this, int skillId, bool beginResume) {
		NoCD& noCD = NoCD::GetInstance();
		if (noCD.f_NoCooldown) return;
		return CALL_ORIGIN(PlayerSkillCd_ReduceSkillSection_Hook, __this, skillId, beginResume);
	
	}

	static void PlayerSkillCd_ResetUseInterval_Hook(void* __this, int skillId) {
		NoCD& noCD = NoCD::GetInstance();
		if (noCD.f_NoCooldown) return;
		return CALL_ORIGIN(PlayerSkillCd_ResetUseInterval_Hook, __this, skillId);
	}

	static void SkillInfo_set_currentEnergy_Hook(void* __this, int64_t value) {
		NoCD& noCD = NoCD::GetInstance();
		if (noCD.f_UnlimitedEnergy) {
			int64_t energy = app::SkillInfo_get_totalEnergy(__this);
			value = (int64_t)energy << 32;
		}
		return CALL_ORIGIN(SkillInfo_set_currentEnergy_Hook, __this, value);
	}

	//static void AdventureSkill_InitSkill_Hook(void* __this, int skillId, void* owner, void* targets, int skillLauncherType, bool isInterruptAttack, bool isBreakable, int skillCastBehaviourType, int countdownTiming, void* finishCallback, void* beginCDForMonsterCallback, bool ingoreCDAndEnergy) {
	//	NoCD& noCD = NoCD::GetInstance();
	//	if (noCD.f_NoCooldown || noCD.f_UnlimitedEnergy) ingoreCDAndEnergy = true;
	//	return CALL_ORIGIN(AdventureSkill_InitSkill_Hook, __this, skillId, owner, targets, skillLauncherType, isInterruptAttack, isBreakable, skillCastBehaviourType, countdownTiming, finishCallback, beginCDForMonsterCallback, ingoreCDAndEnergy);
	//}
}