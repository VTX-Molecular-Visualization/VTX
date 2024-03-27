#ifndef __VTX_APP_CORE_ACTION_ENUM_ACTION_TAG__
#define __VTX_APP_CORE_ACTION_ENUM_ACTION_TAG__

namespace VTX::App::Core::Action
{
	enum ACTION_TAG
	{
		NONE		 = 0,	   // No tags
		MODIFY_SCENE = 1 << 0, // Specify that this action will modify the scene
	};
} // namespace VTX::App::Core::Action
#endif
