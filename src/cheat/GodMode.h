#pragma once
#include "Feature.h"

namespace cheat::feature
{
	class GodMode : public Feature
	{
	public:
		bool f_GodMode;

		static GodMode& GetInstance();

		const FeatureGUIInfo& GetGUIInfo() const override;
		void DrawMain() override;

	private:
		GodMode();
	};
}