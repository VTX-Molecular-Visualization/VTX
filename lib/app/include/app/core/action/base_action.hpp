#ifndef __VTX_APP_CORE_ACTION_BASE_ACTION__
#define __VTX_APP_CORE_ACTION_BASE_ACTION__

#include <string>
#include <util/exceptions.hpp>

namespace VTX::App::Core::Action
{
	// Abstract action from which all action must inherits.
	// Can specify
	class BaseAction
	{
	  public:
		virtual ~BaseAction()  = default;
		virtual void execute() = 0;

		// TODO: add static callback there?
	};

} // namespace VTX::App::Core::Action
#endif
