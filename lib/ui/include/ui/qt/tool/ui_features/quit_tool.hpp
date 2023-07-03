#ifndef __VTX_UI_QT_TOOL_UI_FEATURES_QUIT__
#define __VTX_UI_QT_TOOL_UI_FEATURES_QUIT__

#include "ui/core/tool_registration.hpp"
#include "ui/qt/base_qt_tool.hpp"

namespace VTX::UI::QT::Tool::UIFeatures
{
	class QuitTool : public BaseQtTool
	{
		REGISTER_TOOL( QuitTool, "UI_FEATURE_QUIT" );

	  public:
		QuitTool();
		void instantiateTool() override;

	  private:
		void _addButtonsInMainMenu();

		void _quit();
	};
} // namespace VTX::UI::QT::Tool::UIFeatures

#endif
