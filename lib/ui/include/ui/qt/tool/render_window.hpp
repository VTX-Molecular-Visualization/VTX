#ifndef __VTX_UI_TOOL_DEFAULT_RENDER_WINDOW__
#define __VTX_UI_TOOL_DEFAULT_RENDER_WINDOW__

#include "ui/core/tool_registration.hpp"
#include "ui/qt/base_qt_tool.hpp"

namespace VTX::UI::QT::Tool
{
	class RenderWindow : public BaseQtTool
	{
		REGISTER_TOOL( RenderWindow, "RENDER_WINDOW_KEY" );

	  public:
		RenderWindow();

		void instantiateTool() override;
		void setupContextualMenu() const;
	};
} // namespace VTX::UI::QT::Tool

#endif
