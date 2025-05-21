#ifndef __VTX_UI_QT_WIDGET_COMMAND_LAUNCHER__
#define __VTX_UI_QT_WIDGET_COMMAND_LAUNCHER__

#include "ui/qt/core/base_widget.hpp"
#include <QCompleter>
#include <QLineEdit>
#include <QPointer>
#include <util/history_browser.hpp>

namespace VTX::UI::QT::Widget
{
	class CommandLauncher : public Core::BaseWidget<CommandLauncher, QLineEdit>
	{
	  public:
		CommandLauncher( QWidget * p_parent );

	  protected:
		virtual void keyPressEvent( QKeyEvent * event ) override;

	  private:
		void _setupCompleter();
		void _launchCommand();

		QPointer<QCompleter>		_completer;
		::VTX::Util::HistoryBrowser _history;
	};
} // namespace VTX::UI::QT::Widget
#endif
