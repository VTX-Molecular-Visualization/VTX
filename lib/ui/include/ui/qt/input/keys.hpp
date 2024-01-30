#ifndef __VTX_UI_QT_INPUT_KEYS__
#define __VTX_UI_QT_INPUT_KEYS__

#include <Qt>
#include <util/generic/enum_flag.hpp>

namespace VTX::UI::QT::Input
{
	using Key = Qt::Key;

	enum ModifierEnum : uint
	{
		None	= 0,
		Control = 1 << 0,
		Shift	= 1 << 1,
		Alt		= 1 << 2,
		AltGr	= 1 << 3,
	};
	VTX_FLAG( ModifierFlag, ModifierEnum );

	enum class KeyboardLayout : int
	{
		QWERTY,
		AZERTY
	};

} // namespace VTX::UI::QT::Input
#endif
