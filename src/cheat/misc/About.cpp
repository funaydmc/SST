#include <pch.h>
#include "About.h"
#include <render/gui-util.h>

namespace cheat::feature
{
	About& About::GetInstance()
	{
		static About instance;
		return instance;
	}

	const FeatureGUIInfo& About::GetGUIInfo() const
	{
		static const FeatureGUIInfo info{ "", "About", false };
		return info;
	}

	void About::DrawMain()
	{
		ImGui::Text("Developed by: ");
		TextURL("TheReVeaLz", "https://github.com/TheReVeaLz", false, false);
	}
}