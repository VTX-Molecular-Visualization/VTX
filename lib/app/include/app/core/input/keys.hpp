#ifndef __VTX_APP_CORE_INPUT_KEYS__
#define __VTX_APP_CORE_INPUT_KEYS__

#include <util/enum_flag.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::Input
{

	enum struct Key
	{
		Key_A,
		Key_B,
		Key_C,
		Key_D,
		Key_E,
		Key_F,
		Key_G,
		Key_H,
		Key_I,
		Key_J,
		Key_K,
		Key_L,
		Key_M,
		Key_N,
		Key_O,
		Key_P,
		Key_Q,
		Key_R,
		Key_S,
		Key_T,
		Key_U,
		Key_V,
		Key_W,
		Key_X,
		Key_Y,
		Key_Z,

		Key_0,
		Key_1,
		Key_2,
		Key_3,
		Key_4,
		Key_5,
		Key_6,
		Key_7,
		Key_8,
		Key_9,

		Key_Semicolon,
		Key_Comma,

		Key_Control,
		Key_Shift,
		Key_Alt,
		Key_AltGr,

		Key_Space,
		Key_Enter,

		Key_Up,
		Key_Down,
		Key_Left,
		Key_Right,

		Key_F1,
		Key_F2,
		Key_F3,
		Key_F4,
		Key_F5,
		Key_F6,
		Key_F7,
		Key_F8,
		Key_F9,
		Key_F10,
		Key_F11,
		Key_F12,

		Key_Escape,

		Key_Unkown
	};

	enum ModifierEnum
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

	enum struct KeyboardLayout
	{
		QWERTY,
		AZERTY
	};

} // namespace VTX::App::Core::Input
#endif
