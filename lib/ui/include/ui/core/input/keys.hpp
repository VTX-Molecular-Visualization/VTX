#ifndef __VTX_UI_CORE_INPUT_KEYS__
#define __VTX_UI_CORE_INPUT_KEYS__

#include <Qt>
#include <util/generic/enum_flag.hpp>

namespace VTX::UI::Core::Input
{
	// TODO use custom keys instead of Qt::Key
	using Key = Qt::Key;

	enum ModifierEnum : uint
	{
		None  = 0,
		Ctrl  = 1 << 0,
		Shift = 1 << 1,
		Alt	  = 1 << 2,
		AltGr = 1 << 3,

		Ctrl_Shift = Ctrl | Shift,
		Ctrl_Alt   = Ctrl | Alt,

	};
	VTX_FLAG( ModifierFlag, ModifierEnum );

	enum class KeyboardLayout : int
	{
		QWERTY,
		AZERTY
	};

} // namespace VTX::UI::Core::Input
#endif
