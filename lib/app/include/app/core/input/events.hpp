#ifndef __VTX_APP_CORE_INPUT_EVENTS__
#define __VTX_APP_CORE_INPUT_EVENTS__

#include "keys.hpp"
#include <util/types.hpp>

namespace VTX::App::Core::Input
{
	struct MouseEvent
	{
		enum struct Type
		{
			Press,
			Release,
			Move,
			DoubleClick
		};

		enum struct Button
		{
			Left,
			Right,
			Middle
		};

		Type   type;
		Button button;
		Vec2i  position;
	};

	struct KeyEvent
	{
		enum struct Type
		{
			Press,
			Release
		};

		Type type;
		Key	 key;
		bool isRepeating;
	};

	struct WheelEvent
	{
		enum struct Type
		{
			Wheel
		};

		Type type;
		// TODO: ??
		Vec2i angleDelta;
	};

} // namespace VTX::App::Core::Input
#endif
