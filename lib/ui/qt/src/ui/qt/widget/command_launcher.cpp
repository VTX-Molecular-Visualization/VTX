#include <python_binding/interpretor.hpp>
// pybind MUST be included before any QT because of macro shenanigans
#include "ui/qt/widget/command_launcher.hpp"
#include <util/logger.hpp>

namespace VTX::UI::QT::Widget
{
	CommandLauncher::CommandLauncher( QWidget * p_parent ) : BaseWidget( p_parent )
	{
		setPlaceholderText( "/command" );
		_setupCompleter();
		setCompleter( _completer );
		setClearButtonEnabled( true );

		connect( this, &QLineEdit::textChanged, this, &CommandLauncher::_updateCompleter );
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
			INTERPRETOR().runCommand( command );
		}
		catch ( CommandException & p_e )
		{
			clear();
			throw p_e;
		}

		clear();
	}

	void CommandLauncher::_updateCompleter()
	{
		if ( _completerUpdated )
			return;

		_completerUpdated					 = true;
		std::vector<std::string> allCommands = INTERPRETOR().getModule().commands().getFunctionList();

		QStringList strList;

		strList.emplaceBack( "resetCamera()" );
		strList.emplaceBack( "quit()" );

		for ( auto & it_cmd : allCommands )
			strList.emplaceBack();

		_completer = new QCompleter( strList, this );
		_completer->setCaseSensitivity( Qt::CaseSensitivity::CaseInsensitive );
		_completer->setCompletionMode( QCompleter::CompletionMode::InlineCompletion );
		setCompleter( _completer );
	}
	void CommandLauncher::_setupCompleter()
	{
		// This method wont be looking for the list of available functions as they are unavailable when the
		// command_launcher is created
		QStringList strList = QStringList();

		strList.emplaceBack( "resetCamera()" );
		strList.emplaceBack( "quit()" );

		_completer = new QCompleter( strList, this );
		_completer->setCaseSensitivity( Qt::CaseSensitivity::CaseInsensitive );
		_completer->setCompletionMode( QCompleter::CompletionMode::InlineCompletion );
	}

} // namespace VTX::UI::QT::Widget
