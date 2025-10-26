#pragma once
#include <unordered_map>
#include <vector>
#include "Feature.h"

namespace cheat
{
	class CheatMenu {
	public:
		static CheatMenu& GetInstance();

		void Init();
		void AddFeatures(std::vector<Feature*> features);
		void AddFeature(Feature* feature);

		void OnRender();
	
		static inline bool bShowMenu = true;
	private:
		std::unordered_map<std::string, std::vector<Feature*>> m_FeatureMap;
		
		void PushFeature(Feature* feature);

	protected:
		void DrawMenu();
	};
}