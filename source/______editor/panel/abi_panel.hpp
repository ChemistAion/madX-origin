#pragma once

namespace madX::Editor::Panel
{
	typedef std::int64_t Type;

	typedef Type PanelFlag;
	enum PanelFlag_ : PanelFlag
	{
		PanelFlag_Modified		= PanelFlag(1) << 0, //the content was modified
		PanelFlag_Warning		= PanelFlag(1) << 1,
		PanelFlag_Error			= PanelFlag(1) << 2,

		PanelFlag_Disabled		= PanelFlag(1) << 3,
		PanelFlag_Locked		= PanelFlag(1) << 4,

		PanelFlag_Changed		= PanelFlag(1) << 5, //the content was _not_ modified, representation only
		PanelFlag_Reserved6		= PanelFlag(1) << 6,
		PanelFlag_Reserved7		= PanelFlag(1) << 7
	};

	constexpr Type cPanelStateNull = 0;

	constexpr Type cPanelABI_FlagCount = 8;
	constexpr Type cPanelABI_FlagBitStart = 0;
	constexpr Type cPanelABI_FlagBitStop = 7;
	constexpr Type cPanelABI_FlagMask = 0xff;
}