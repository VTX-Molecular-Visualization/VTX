#ifndef __VTX_UI_TOOL_INSPECTOR_WINDOW__
#define __VTX_UI_TOOL_INSPECTOR_WINDOW__

#include "ui/core/tool_registry.hpp"
#include "ui/qt/base_qt_widget.hpp"
#include <string>

namespace VTX::UI::QT::Widget
{
	class InspectorWidget : public BaseQtTool
	{
	  public:
		inline static const UI::Core::ToolIdentifier TOOL_KEY = "INSPECTOR_TOOL_KEY";

	  private:
		inline static const UI::Core::ToolRegistry::Registration<InspectorWidget> _reg { TOOL_KEY };

	  public:
		inline static const std::string INSPECTOR_PANEL_KEY = "INSPECTOR_PANEL_KEY";

	  public:
		InspectorWidget();
		void instantiateTool() override;
	};
} // namespace VTX::UI::QT::Widget

#endif
