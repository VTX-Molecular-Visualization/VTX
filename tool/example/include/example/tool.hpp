#ifndef __VTX_TOOL_EXAMPLE_TOOL_
#define __VTX_TOOL_EXAMPLE_TOOL_

#include <app/tool/base_tool.hpp>

namespace VTX::Tool::Example
{

	/**
	 * @brief The main class of the tool, inherits from abstract BaseTool.
	 */
	class Tool : public App::Tool::BaseTool
	{
	  public:
		Tool();
		~Tool();

		void init() override;
		void onAppStart() override;
		void createUI() override;
		void onAppStop() override;
	};

} // namespace VTX::Tool::Example

#endif
