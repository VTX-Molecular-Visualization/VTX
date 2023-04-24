#ifndef __VTX_ACTION_DEV__
#define __VTX_ACTION_DEV__

#include "app/core/action/base_action.hpp"

namespace VTX::Action::Dev
{
	class CompileShaders : public Core::Action::BaseAction
	{
	  public:
		virtual void execute() override;
	};

} // namespace VTX::Action::Dev

#endif
