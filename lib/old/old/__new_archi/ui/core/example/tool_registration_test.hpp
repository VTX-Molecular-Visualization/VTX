#ifndef __VTX_UI_CORE_EXAMPLE_TOOL_REGISTRATION_TEST__
#define __VTX_UI_CORE_EXAMPLE_TOOL_REGISTRATION_TEST__

#include "__new_archi/ui/core/tool_registration.hpp"

namespace VTX::UI::Core::Example
{
	class VTXUIToolTest : public BaseVTXUITool
	{
		REGISTER_TOOL( VTXUIToolTest, "VTXUIToolTest" );

	  public:
		VTXUIToolTest();
		void instantiateTool() override {};
	};

} // namespace VTX::UI::Core::Example

#endif
