#ifndef __VTX_UI_CORE_EXAMPLE_TOOL_REGISTRATION_TEST__
#define __VTX_UI_CORE_EXAMPLE_TOOL_REGISTRATION_TEST__

#include "ui/core/tool_registry.hpp"

namespace VTX::UI::Core::Example
{
	class VTXUIToolTest : public BaseVTXUITool
	{
		inline static const ToolRegistry::Registration<VTXUIToolTest> _reg { "VTXUIToolTest" };

	  public:
		VTXUIToolTest();

		void instantiateTool() override {};
	};

} // namespace VTX::UI::Core::Example

#endif
