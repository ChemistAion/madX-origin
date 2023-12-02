#pragma once

#include "imgui.h"
#include "im3d.h"
#include "im3d_math.h"

namespace madX::Editor::Panel
{
	using PanelBase = std::shared_ptr<class PanelClass>;

	template <class Type>
	using PanelType = std::shared_ptr<Type>;

	class PanelClass
	{
	public:
		PanelClass() = default;
		virtual ~PanelClass() = default;

		virtual void UpdatePanel() {};
		virtual void BeginPanel() {};

		virtual void ProcessPanel(const class Editor::Processor&) = 0;
		virtual void EndPanel() {};

		virtual const PanelState& GetPanelState() const noexcept = 0;
	};
}