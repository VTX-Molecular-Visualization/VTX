#ifndef __VTX_TOOL_EXAMPLE_TOOL_
#define __VTX_TOOL_EXAMPLE_TOOL_

#include <app/tool/base_tool.hpp>

namespace VTX::Tool::Example
{
	class Tool : public App::Tool::BaseTool
	{
	  public:
		Tool();
		virtual ~Tool();
	};

} // namespace VTX::Tool::Example

#endif
