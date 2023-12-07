#ifndef __VTX_APP_CORE_ACTION_BASE_ACTION__
#define __VTX_APP_CORE_ACTION_BASE_ACTION__

#include "enum_action_tag.hpp"
#include <string>
#include <util/exceptions.hpp>

namespace VTX::App::Core::Action
{
	class BaseAction
	{
	  public:
		BaseAction( const ACTION_TAG p_tag = ACTION_TAG::NONE ) : _tag( p_tag ) {};
		virtual ~BaseAction() = default;

		virtual void execute() = 0;
		virtual void displayUsage() {}

		const ACTION_TAG & getTag() const { return _tag; }

	  protected:
		ACTION_TAG _tag;
	};

} // namespace VTX::App::Core::Action
#endif
