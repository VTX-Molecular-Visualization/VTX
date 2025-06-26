#ifndef __VTX_UI_QT_WIDGET_COMMAND_LAUNCHER__
#define __VTX_UI_QT_WIDGET_COMMAND_LAUNCHER__

#include "ui/qt/core/base_widget.hpp"
#include <QCompleter>
#include <QLineEdit>
#include <QPointer>
#include <util/history_browser.hpp>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Class responsible for displaying a textbox and executing its content when enter is pressed.
	 * Executing means that we send the line to the python interpreter.
	 */
	class CommandLauncher : public Core::BaseWidget<CommandLauncher, QLineEdit>
	{
	  public:
		CommandLauncher( QWidget * p_parent );

		/**
		 * @brief Send a commmand to the command launcher, as if some user would have typed it and hit enter.
		 * @param
		 */
		void launchCommand( std::string );

	  protected:
		virtual void keyPressEvent( QKeyEvent * event ) override;
		virtual void focusInEvent( QFocusEvent * event ) override;
		virtual void focusOutEvent( QFocusEvent * event ) override;

	  private:
		void _setupCompleter();
		void _launchCommand();

		void _executeLastHistoryEntry();

		QPointer<QCompleter>		_completer;
		::VTX::Util::HistoryBrowser _history;
	};
} // namespace VTX::UI::QT::Widget
#endif
