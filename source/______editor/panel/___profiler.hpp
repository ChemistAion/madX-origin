#pragma once

#include "imgui/imgui.h"
#include "im3d.h"

#include "panel.hpp"

namespace madX::Editor::Frontend
{
	class Profiler final : public Panel
	{
	public:
		void RenderPanel() override
		{
			ImGui::Begin("Profiler", NULL);
			ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			
			float depth = FLT_MAX;
			depth = Im3d::Min(depth, Im3d::GetContext().m_hotDepth);
			
			if (depth != FLT_MAX)
			{
				ImGui::Text("Depth: %f", depth);
			}
			else
			{
				ImGui::Text("Depth: FLT_MAX");
			}
			
			//	ImGui::Text("Mouse: %f.%f", condom_.mouse_.x, condom_.mouse_.y);
			//	ImGui::Text("Cursor: %f.%f", condom_.cursor_.x, condom_.cursor_.y);
			//	ImGui::Text("Screen: %f.%f", condom_.screen_.x, condom_.screen_.y);
			//	ImGui::Text("Final: %f.%f", condom_.final_.x, condom_.final_.y);
			
			ImGui::End();
		}
	};
}