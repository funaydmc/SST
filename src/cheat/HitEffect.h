#pragma once
#include "Feature.h"

namespace cheat::feature
{
	class HitEffect : public Feature
	{
	public:
		bool f_GodMode;

		bool f_MultiHit;
		int f_MultiHitMultiplier;

		static HitEffect& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

	private:
		HitEffect();
	};
}