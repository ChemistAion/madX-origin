#pragma once

#include "_editor.hpp"

//	#define IMGUI_DEFINE_MATH_OPERATORS
//	#include "imgui.h"
//	#include "imgui_internal.h"

#include "ImTextEdit.h"

namespace madX::Editor::Panel
{
	class CodeEditor final : public PanelClass
	{
		ImTextEdit editor_;
	public:
		CodeEditor()
		{
			editor_.SetLanguageDefinition(ImTextEdit::LanguageDefinition::HLSL());
		}

		void RenderPanel(const std::shared_ptr<Root::Component::Project>&) override
		{
			std::string name("CodeEditor");
			ImGui::Begin(name.c_str(), NULL);
			
			ImFont* font = ImGui::GetFont();
			ImGui::PushFont(font);
			editor_.Render(name.c_str());
			ImGui::PopFont();
			
			ImGui::End();
		}
	};
}