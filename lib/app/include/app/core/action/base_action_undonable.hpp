#ifndef __VTX_APP_CORE_ACTION_BASE_ACTION_UNDONABLE__
#define __VTX_APP_CORE_ACTION_BASE_ACTION_UNDONABLE__

#include "app/core/action/base_action.hpp"

namespace VTX::App::Core::Action
{
	class BaseActionUndonable : public App::Core::Action::BaseAction
	{
	  public:
		virtual ~BaseActionUndonable() = default;

		virtual void undo() = 0;
		virtual void redo() { execute(); }
	};
} // namespace VTX::App::Core::Action
#endif
