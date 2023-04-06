#ifndef __VTX_BASE_ACTION__
#define __VTX_BASE_ACTION__

#include <string>
#include <util/exceptions.hpp>
#include <vector>

namespace VTX
{
	namespace Action
	{
		enum ACTION_TAG
		{
			NONE		 = 0,
			MODIFY_SCENE = 1 << 0,
		};

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

	} // namespace Action
} // namespace VTX
#endif
