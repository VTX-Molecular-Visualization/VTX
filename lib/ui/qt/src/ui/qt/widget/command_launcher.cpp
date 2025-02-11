#include "ui/qt/widget/command_launcher.hpp"
// #include "ui/qt/widget/pytx/include_python_binding.hpp"

#include <util/logger.hpp>

namespace VTX::UI::QT::Widget
{
	CommandLauncher::CommandLauncher( QWidget * p_parent ) : BaseWidget( p_parent )
	{
		setPlaceholderText( "/command" );
		_setupCompleter();
		setCompleter( _completer );
		setClearButtonEnabled( true );

		connect( this, &QLineEdit::returnPressed, this, &CommandLauncher::_launchCommand );
	}

	void CommandLauncher::_launchCommand()
	{
		if ( text().isEmpty() )
		{
			return;
		}

		const std::string command = text().toStdString();

		try
		{
			VTX_INFO( "CommandLauncher: {}", command );
			// PythonBinding::INTERPRETOR().runCommand( command );
		}
		catch ( CommandException & p_e )
		{
			clear();
			throw p_e;
		}

		clear();
	}

	void CommandLauncher::_setupCompleter()
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

} // namespace VTX::UI::QT::Widget
