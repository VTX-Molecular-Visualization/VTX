#ifndef __VTX_APP_OLD_ACTION_DEV__
#define __VTX_APP_OLD_ACTION_DEV__

#include "app/old/action.hpp"
#include "app/old/core/action/base_action.hpp"

namespace VTX::App::Old::Action::Dev
{
	class CompileShaders : public App::Old::Core::Action::BaseAction
	{
	  public:
		virtual void execute() override;
	};

} // namespace VTX::App::Old::Action::Dev

#endif
