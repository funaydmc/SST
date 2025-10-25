#pragma once
#include "Feature.h"

namespace cheat::feature
{
	class About : public Feature
	{
	public:
		static About& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;
	};
}