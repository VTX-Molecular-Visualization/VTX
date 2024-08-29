#ifndef __VTX_TOOL_EXAMPLE_TOOL_
#define __VTX_TOOL_EXAMPLE_TOOL_

#include <app/tool/base_tool.hpp>

namespace VTX::Tool::Example
{

	/**
	 * @brief The main class of the tool, inherits from abstract BaseTool.
	 */
	class ExampleTool : public App::Tool::BaseTool
	{
	  public:
		ExampleTool();
		~ExampleTool();

		virtual void init() override;
		virtual void onAppStart() override;
		virtual void createUI() override;
		virtual void onAppStop() override;
	};

} // namespace VTX::Tool::Example

#endif
