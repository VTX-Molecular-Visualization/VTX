#ifndef __VTX_UI_QT_TOOL_UI_FEATURES_VISUALIZATION__
#define __VTX_UI_QT_TOOL_UI_FEATURES_VISUALIZATION__

#include <QObject>

namespace VTX::UI::QT::Widget::UIFeatures
{
	class VisualizationTool : public QObject
	{
	  public:
		VisualizationTool();

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
