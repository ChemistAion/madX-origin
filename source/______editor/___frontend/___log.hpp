#pragma once

#include "imgui/imgui.h"

namespace madX::Editor::Frontend
{
	class Log final : public Core::Singleton<Log>
	{
		ImGuiTextBuffer buffer_;
		bool scroll_ = false;

		friend Singleton<Log>;
	public:
		void AddLog(const char* string)
		{
			buffer_.append(string);
			scroll_ = true;
		}

		void Draw(const char* title, bool* p_open = NULL)
		{
			if (!ImGui::Begin(title, p_open))
			{
				ImGui::End();
				return;
			}

			ImGui::TextUnformatted(buffer_.begin());
			
			if (scroll_)
			{
				ImGui::SetScrollHereY(1.0f);
			}

			scroll_ = false;
			ImGui::End();
		}
	};
}