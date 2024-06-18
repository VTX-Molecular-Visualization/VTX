#include "widget/pytx/line_edit.hpp"
#include "widget/pytx/include_python_binding.hpp"
#include <QCompleter>
#include <QHBoxLayout>
#include <QLabel>
#include <util/logger.hpp>

namespace VTX::UI::QT::Widget::PyTX
{
	CommandLinePrompt::CommandLinePrompt( QWidget * p_parent ) : BaseManualWidget( p_parent ) { _setupCompleter(); }

	void CommandLinePrompt::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		setPlaceholderText( ">" );
		setCompleter( _completer );
		setClearButtonEnabled( true );
	}

	void CommandLinePrompt::_setupSlots()
	{
		connect( this, &QLineEdit::returnPressed, this, &CommandLinePrompt::_launchCommand );
	};
	void CommandLinePrompt::localize() {}

	void CommandLinePrompt::_launchCommand()
	{
		if ( text().isEmpty() )
			return;

		const std::string command = text().toStdString();

		try
		{
			PythonBinding::INTERPRETOR().runCommand( command );
		}
		catch ( CommandException & p_e )
		{
			clear();
			throw p_e;
		}

		clear();
	}

	void CommandLinePrompt::_setupCompleter()
	{
		// std::vector<std::string> allCommands = PythonBinding::INTERPRETOR().getModule().commands().getFunctionList();

		QStringList strList = QStringList();

		strList.emplaceBack( "resetCamera()" );
		strList.emplaceBack( "quit()" );

		// for ( const std::string & str : allCommands )
		//{
		//	strList.emplaceBack( QString::fromStdString( str ) );
		// }

		_completer = new QCompleter( strList, this );
		_completer->setCaseSensitivity( Qt::CaseSensitivity::CaseInsensitive );
		_completer->setCompletionMode( QCompleter::CompletionMode::InlineCompletion );
	}

} // namespace VTX::UI::QT::Widget::PyTX
