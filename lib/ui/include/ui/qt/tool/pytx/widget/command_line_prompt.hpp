#ifndef __VTX_UI_TOOL_PYTX_WIDGET_COMMAND_LINE_PROMPT__
#define __VTX_UI_TOOL_PYTX_WIDGET_COMMAND_LINE_PROMPT__

#include "ui/qt/widget/base_manual_widget.hpp"
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

namespace VTX::UI::QT::Tool::PyTX::Widget
{
	class CommandLinePrompt : public QT::Widget::BaseManualWidget<QLineEdit>
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		void localize() override;

	  protected:
		CommandLinePrompt( QWidget * p_parent = nullptr );
		~CommandLinePrompt() = default;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		void _setupCompleter();
		void _launchCommand();

		QCompleter * _completer = nullptr;
	};
} // namespace VTX::UI::QT::Tool::PyTX::Widget
#endif
