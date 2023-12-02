#pragma once

namespace madX::Editor::Panel
{
	typedef Type State;

	struct PanelState : public Meta::State
	{
		mutable Panel::State state_ = cPanelStateNull;

		PanelState(const char* name)
			: Meta::State(name)
		{
		}
	};
}