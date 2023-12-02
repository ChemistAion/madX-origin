#pragma once

#include "_editor.hpp"

namespace madX::Editor::Panel
{
	class Profiler final : public PanelClass
	{
	public:
		void RenderPanel(const std::shared_ptr<Root::Component::Project>&) override
		{
			ImGui::Begin("Profiler", NULL);
			ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			
			ImGui::Separator();

			ImGui::Text("Layers:    %u ", Im3d::GetContext().getLayerCount());
			ImGui::Text("Triangles: %u ", Im3d::GetContext().getPrimitiveCount(Im3d::DrawPrimitive_Triangles));
			ImGui::Text("Lines:     %u ", Im3d::GetContext().getPrimitiveCount(Im3d::DrawPrimitive_Lines));
			ImGui::Text("Points:    %u ", Im3d::GetContext().getPrimitiveCount(Im3d::DrawPrimitive_Points));
			ImGui::Text("Texts:     %u ", Im3d::GetContext().getTextCount());

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
			
			ImGui::Separator();
		
			ImGui::End();
		}
	};
}