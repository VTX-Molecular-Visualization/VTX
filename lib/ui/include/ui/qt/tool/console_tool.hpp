#ifndef __VTX_UI_TOOL_CONSOLE_WINDOW__
#define __VTX_UI_TOOL_CONSOLE_WINDOW__

#include "ui/core/tool_registry.hpp"
#include "ui/qt/base_qt_tool.hpp"
#include <string>

namespace VTX::UI::QT::Tool
{
	class ConsoleTool : public BaseQtTool
	{
	  public:
		inline static const UI::Core::ToolIdentifier TOOL_KEY = "CONSOLE_TOOL_KEY";

	  private:
		inline static const UI::Core::ToolRegistry::Registration<ConsoleTool> _reg { TOOL_KEY };

	  public:
		inline static const std::string CONSOLE_PANEL_KEY = "CONSOLE_PANEL_KEY";

	  public:
		ConsoleTool();
		void instantiateTool() override;
	};
} // namespace VTX::UI::QT::Tool

#endif
