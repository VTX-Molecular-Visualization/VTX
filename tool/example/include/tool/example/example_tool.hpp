#ifndef VTX_TOOL_EXAMPLE_EXAMPLE_TOOL_HPP
#define VTX_TOOL_EXAMPLE_EXAMPLE_TOOL_HPP

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
		void registerActions() override;
		void buildUI() override;
	};

} // namespace VTX::Tool::Example

#endif
