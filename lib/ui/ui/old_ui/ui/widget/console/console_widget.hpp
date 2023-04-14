#ifndef __VTX_UI_WIDGET_CONSOLE__
#define __VTX_UI_WIDGET_CONSOLE__

#include "old_ui/ui/widget/base_manual_widget.hpp"
#include "old_ui/ui/widget/custom_widget/dock_window_main_widget.hpp"
#include <QColor>
#include <QDockWidget>
#include <QListWidget>
#include <QPushButton>
#include <string>

namespace VTX::UI::Widget::Console
{
	class ConsoleWidget : public BaseManualWidget<QDockWidget>
	{
		VTX_WIDGET

	  public:
		void receiveEvent( const VTX::Event::VTXEvent & p_event ) override;

	  protected:
		ConsoleWidget( QWidget * );
		~ConsoleWidget() = default;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void localize() override;

	  private:
		inline static uint _SIZE = 50;

		CustomWidget::DockWindowMainWidget<QWidget> * _mainWidget  = nullptr;
		QListWidget *								  _listWidget  = nullptr;
		QPushButton *								  _clearWidget = nullptr;

		void _clearConsoleAction();

		QColor _getMessageColor( const std::string & p_level );
	};
} // namespace VTX::UI::Widget::Console
#endif
