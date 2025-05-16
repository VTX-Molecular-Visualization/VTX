#ifndef __VTX_UI_QT_WIDGET_COMMAND_LAUNCHER__
#define __VTX_UI_QT_WIDGET_COMMAND_LAUNCHER__

#include "ui/qt/core/base_widget.hpp"
#include <QCompleter>
#include <QLineEdit>
#include <QPointer>

namespace VTX::UI::QT::Widget
{
	class CommandLauncher : public Core::BaseWidget<CommandLauncher, QLineEdit>
	{
	  public:
		CommandLauncher( QWidget * p_parent );

	  protected:
		virtual void keyPressEvent( QKeyEvent * event ) override;

	  private:
		void _updateCompleter();
		void _setupCompleter();
		void _launchCommand();

		bool					 _completerUpdated = false;
		QPointer<QCompleter>	 _completer;
		std::vector<std::string> _history;
		bool					 _browsingHistory = false;
		size_t					 _historyIdx	  = 0;
	};
} // namespace VTX::UI::QT::Widget
#endif
