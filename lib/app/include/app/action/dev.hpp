#ifndef __VTX_APP_ACTION_DEV__
#define __VTX_APP_ACTION_DEV__

#include "app/action.hpp"
#include "app/core/action/base_action.hpp"

namespace VTX::App::Action::Dev
{
	class CompileShaders : public App::Core::Action::BaseAction
	{
	  public:
		virtual void execute() override;
	};

} // namespace VTX::App::Action::Dev

#endif
