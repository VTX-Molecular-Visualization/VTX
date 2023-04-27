#ifndef __VTX_APP_ACTION_ENUM_VISIBILITY_MODE__
#define __VTX_APP_ACTION_ENUM_VISIBILITY_MODE__

namespace VTX::App::Action
{
	enum class VISIBILITY_MODE : int
	{
		HIDE = 0,
		SHOW,
		TOGGLE,
		SOLO,
		ALL
	};
} // namespace VTX::App::Action
#endif
