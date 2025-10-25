#pragma once


void AddUnderLine(ImColor col_);
void TextURL(const char* name_, const char* URL_, bool SameLineBefore_, bool SameLineAfter_);

namespace ImGui {
	void BeginGroupPanel(const char* name, const ImVec2& size = ImVec2(-1.0f, 0.0f));
	void EndGroupPanel();
}