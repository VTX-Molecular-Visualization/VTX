#ifndef __VTX_APP_OLD_CORE_ACTION_BASE_ACTION_UNDONABLE__
#define __VTX_APP_OLD_CORE_ACTION_BASE_ACTION_UNDONABLE__

#include "app/old/core/action/base_action.hpp"

namespace VTX::App::Old::Core::Action
{
	class BaseActionUndonable : public App::Old::Core::Action::BaseAction
	{
	  public:
		virtual ~BaseActionUndonable() = default;

		virtual void undo() = 0;
		virtual void redo() { execute(); }
	};
} // namespace VTX::App::Old::Core::Action
#endif
