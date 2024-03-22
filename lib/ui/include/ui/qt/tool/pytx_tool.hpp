#ifndef __VTX_UI_TOOL_PYTX_TOOL__
#define __VTX_UI_TOOL_PYTX_TOOL__

#include "ui/core/tool_registry.hpp"
#include "ui/qt/base_qt_tool.hpp"

namespace VTX::UI::QT::Tool
{
	class PyTXTool : public BaseQtTool
	{
	  public:
		inline static const UI::Core::ToolIdentifier TOOL_KEY = "PYTX_TOOL_KEY";

	  private:
		inline static const UI::Core::ToolRegistry::Registration<PyTXTool> _reg { TOOL_KEY };

	  public:
		PyTXTool();
		void instantiateTool() override;
	};
} // namespace VTX::UI::QT::Tool

#endif
