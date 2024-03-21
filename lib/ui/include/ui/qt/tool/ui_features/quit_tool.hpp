#ifndef __VTX_UI_QT_TOOL_UI_FEATURES_QUIT__
#define __VTX_UI_QT_TOOL_UI_FEATURES_QUIT__

#include "ui/core/tool_registry.hpp"
#include "ui/qt/base_qt_tool.hpp"

namespace VTX::UI::QT::Tool::UIFeatures
{
	class QuitTool : public BaseQtTool
	{
	  public:
		inline static const UI::Core::ToolIdentifier TOOL_KEY = "UI_FEATURE_QUIT";

	  private:
		inline static const UI::Core::ToolRegistry::Registration<QuitTool> _reg { TOOL_KEY };

	  public:
		QuitTool();
		void instantiateTool() override;

	  private:
		void _addButtonsInMainMenu();
		void _addCommands() const;

		void _quit();
	};
} // namespace VTX::UI::QT::Tool::UIFeatures

#endif
