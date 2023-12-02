#include "_editor.hpp"

namespace madX::Editor::Panel
{
	void Debug::ProcessPanel(const Editor::Processor&)
	{
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Begin("Debug", NULL);
		ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::Separator();

		const float view_width = ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x;

		////////////////////////////////////////////////////////////////////////////////

		for (const auto& image : images_)
		{
			float tex_w = (float)image.width_;
			float tex_h = (float)image.height_;
			float tex_aspect = tex_h / tex_w;
			
			float size_w = view_width;
			float size_h = (view_width) * tex_aspect;
			
			ImGui::Text("%s %.0fx%.0f", image.name_.c_str(), tex_w, tex_h);

			const ImVec2 uv_min = ImVec2(0.0f, 0.0f);
			const ImVec2 uv_max = ImVec2(1.0f, 1.0f);
			const ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
			const ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);

			const auto handle = image.view_->GetHandleGPU().ptr;
			ImGui::Image((ImTextureID)handle, ImVec2(size_w, size_h), uv_min, uv_max, tint_col, border_col);		
		}

		////////////////////////////////////////////////////////////////////////////////

		ImGui::End();
	}
}
