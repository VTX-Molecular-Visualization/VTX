#ifndef __VTX_UI_QT_TOOL_UI_FEATURES_VISUALIZATION__
#define __VTX_UI_QT_TOOL_UI_FEATURES_VISUALIZATION__

#include "ui/core/tool_registry.hpp"
#include "ui/qt/base_qt_widget.hpp"

namespace VTX::UI::QT::Widget::UIFeatures
{
	class VisualizationTool : public BaseQtTool
	{
	  public:
		inline static const UI::Core::ToolIdentifier TOOL_KEY = "UI_FEATURE_VISUALIZATION";

	  private:
		inline static const UI::Core::ToolRegistry::Registration<VisualizationTool> _reg { TOOL_KEY };

	  public:
		VisualizationTool();
		void instantiateTool() override;

	  private:
		void _addButtonsInMainMenu();
		void _addCommands() const;

		void _resetCamera() const;
		void _orientCamera() const;
		void _setFreefly() const;
		void _setTrackball() const;
	};
} // namespace VTX::UI::QT::Widget::UIFeatures

#endif
