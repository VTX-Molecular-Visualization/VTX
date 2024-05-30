#ifndef __VTX_UI_TOOL_PYTX_TOOL__
#define __VTX_UI_TOOL_PYTX_TOOL__

#include "ui/core/tool_registry.hpp"
#include "ui/qt/base_qt_widget.hpp"

namespace VTX::UI::QT::Widget
{
	class PyTXWidget : public BaseQtTool
	{
	  public:
		inline static const UI::Core::ToolIdentifier TOOL_KEY = "PYTX_TOOL_KEY";

	  private:
		inline static const UI::Core::ToolRegistry::Registration<PyTXWidget> _reg { TOOL_KEY };

	  public:
		PyTXWidget();
		void instantiateTool() override;
	};
} // namespace VTX::UI::QT::Widget

#endif
