#ifndef __VTX_UI_QT_LLINE_EDIT_COMMAND_LAUNCHER__
#define __VTX_UI_QT_LLINE_EDIT_COMMAND_LAUNCHER__

#include "ui/qt/core/base_widget.hpp"
#include <QCompleter>
#include <QLineEdit>
#include <QPointer>

namespace VTX::UI::QT::LineEdit
{
	class CommandLauncher : public Core::BaseWidget<CommandLauncher, QLineEdit>
	{
	  public:
		CommandLauncher( QWidget * p_parent );

	  private:
		void _setupCompleter();
		void _launchCommand();

		QPointer<QCompleter> _completer;
	};
} // namespace VTX::UI::QT::LineEdit
#endif
