#ifndef __VTX_UI_TOOL_CONSOLE_WINDOW__
#define __VTX_UI_TOOL_CONSOLE_WINDOW__

#include "ui/core/tool_registry.hpp"
#include "ui/qt/base_qt_widget.hpp"
#include <string>

namespace VTX::UI::QT::Widget
{
	class ConsoleWidget : public BaseQtTool
	{
	  public:
		inline static const UI::Core::ToolIdentifier TOOL_KEY = "CONSOLE_TOOL_KEY";

	  private:
		inline static const UI::Core::ToolRegistry::Registration<ConsoleWidget> _reg { TOOL_KEY };

	  public:
		inline static const std::string CONSOLE_PANEL_KEY = "CONSOLE_PANEL_KEY";

	  public:
		ConsoleWidget();
		void instantiateTool() override;
	};
} // namespace VTX::UI::QT::Widget

#endif
