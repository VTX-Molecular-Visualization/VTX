#ifndef __VTX_UI_QT_TOOL_UI_FEATURES_QUIT__
#define __VTX_UI_QT_TOOL_UI_FEATURES_QUIT__

#include <QObject>

namespace VTX::UI::QT::Widget::UIFeatures
{
	class QuitTool : public QObject
	{
	  public:
		QuitTool();

	  private:
		void _addButtonsInMainMenu();
		void _addCommands() const;

		void _quit();
	};
} // namespace VTX::UI::QT::Widget::UIFeatures

#endif
