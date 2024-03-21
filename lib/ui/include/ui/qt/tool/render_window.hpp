#ifndef __VTX_UI_TOOL_DEFAULT_RENDER_WINDOW__
#define __VTX_UI_TOOL_DEFAULT_RENDER_WINDOW__

#include "ui/core/tool_registry.hpp"
#include "ui/qt/base_qt_tool.hpp"

namespace VTX::UI::QT::Tool
{
	class RenderWindow : public BaseQtTool
	{
	  public:
		inline static const UI::Core::ToolIdentifier TOOL_KEY = "RENDER_WINDOW_KEY";

	  private:
		inline static const UI::Core::ToolRegistry::Registration<RenderWindow> _reg { TOOL_KEY };

	  public:
		RenderWindow();

		void instantiateTool() override;

	  private:
		void _setupContextualMenu() const;
	};
} // namespace VTX::UI::QT::Tool

#endif
