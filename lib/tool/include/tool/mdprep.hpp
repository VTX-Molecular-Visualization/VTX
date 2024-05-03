#ifndef __VTX_TOOL_MDPREP_REGISTRATION__
#define __VTX_TOOL_MDPREP_REGISTRATION__

#include "ui/core/tool_registry.hpp"

namespace VTX::Tool
{
	class ToolMdprep : public VTX::UI::Core::BaseVTXUITool
	{
	  public:
		inline static const char NAME[] { "ToolMdprep" };

	  private:
		inline static const VTX::UI::Core::ToolRegistry::Registration<ToolMdprep> _reg { NAME };

	  public:
		ToolMdprep();

		void instantiateTool() override;
	};
} // namespace VTX::Tool

#endif //! __VTX_TOOL_MDPREP_REGISTRATION__
