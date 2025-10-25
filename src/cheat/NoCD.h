#pragma once
#include <src/Feature.h>

namespace cheat::feature
{
	class NoCD : public Feature
	{
	public:
		bool f_UnlimitedEnergy;
		bool f_NoCooldown;

		static NoCD& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

	private:
		NoCD();
	};
}

//void ClearEnergy_Hook(void* _this);
//bool get_CD_Hook(void* _this, int skillId);
//void ResetUseInterval_Hook(void* _this, int skillId);
//void SetSkillCurrentUseTimeHint_Hook(void* _this, int skillId, void* currentUseTimeHint);