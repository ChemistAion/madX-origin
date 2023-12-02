#include "_editor.hpp"

#include "collection.hpp"

namespace madX::Editor::Panel
{
	void Collection::RenderPanel(const std::shared_ptr<Root::Component::Project>& project)
	{
		ImGui::Begin("Collection", NULL);

		const auto source = project->GetSource();

		////////////////////////////////////////////////////////////////////////////////

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Nodes"))
		{
			for (std::uint32_t index = 0; const auto& node : source->GetNodes())
			{
				std::string name;

				if (node.name_.has_value())
				{
					name = node.name_.value();
				}
				else
				{
					name = std::string("NODE::") + std::to_string(index++);
				}

				if (ImGui::TreeNode(name.c_str()))
				{
					//TODO: add some details
					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

		////////////////////////////////////////////////////////////////////////////////

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Materials"))
		{
			for (std::uint32_t index = 0; const auto& material : source->GetMaterials())
			{
				std::string name;

				if (material.name_.has_value())
				{
					name = material.name_.value();
				}
				else
				{
					name = std::string("MATERIAL::") + std::to_string(index++);
				}

				if (ImGui::TreeNode(name.c_str()))
				{
					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

		ImGui::End();
	}
}