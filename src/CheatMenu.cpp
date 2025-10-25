#include <pch.h>
#include "CheatMenu.h"
#include <render/renderer.h>
#include <render/gui-util.h>

namespace cheat
{
	CheatMenu& CheatMenu::GetInstance() {
		static CheatMenu instance;
		return instance;
	}

	void CheatMenu::Init() {
		 renderer::Init();
	}

	void CheatMenu::OnRender() {
		if (bShowMenu) {
			DrawMenu();
		}
	}

	void CheatMenu::DrawMenu() {
		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize({ 375, 275 }, ImGuiCond_Once);
		
		if (!ImGui::Begin("StellaSora")) {
			return ImGui::End();
		}

		ImGui::BeginTabBar("##tabs");
		for (const auto& [moduleName, features] : m_FeatureMap) {
			if (ImGui::BeginTabItem(moduleName.c_str())) {
				for (const auto& feature : features) {
					auto& featureInfo = feature->GetGUIInfo();
					if (featureInfo.isGroup) {
						ImGui::BeginGroupPanel(featureInfo.name.c_str());
						//ImGui::SeparatorText(featureInfo.name.c_str());
						feature->DrawMain();
						ImGui::EndGroupPanel();
					} else {
						feature->DrawMain();
					}
				}
				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
		ImGui::End();
	}

	void CheatMenu::PushFeature(Feature* feature) {
		auto& info = feature->GetGUIInfo();
		
		if (m_FeatureMap.count(info.moduleName) == 0) {
			m_FeatureMap[info.moduleName] = {};
		}

		m_FeatureMap[info.moduleName].push_back(feature);
	}

	void CheatMenu::AddFeatures(std::vector<Feature*> features)
	{
		for (const auto& feature : features)
		{
			PushFeature(feature);
		}
	}

	void CheatMenu::AddFeature(Feature* feature)
	{
		PushFeature(feature);
	}
}