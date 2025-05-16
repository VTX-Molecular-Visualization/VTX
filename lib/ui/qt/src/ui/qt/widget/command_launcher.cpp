#include <python_binding/interpretor.hpp>
// pybind MUST be included before any QT because of macro shenanigans
#include "ui/qt/widget/command_launcher.hpp"
#include <qevent.h>
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
		// connect( this, &CommandLauncher::keyPressEvent, this, &CommandLauncher::_launchCommand );
	}

	void CommandLauncher::keyPressEvent( QKeyEvent * event )
	{
		if ( not _history.empty() )
		{
			switch ( event->key() )
			{
			case Qt::Key_Up:
				if ( _browsingHistory )
				{
					_historyIdx = std::min( _history.size() - 1, _historyIdx + 1 );
				}
				goto browsing;
				break;
			case Qt::Key_Down:
				if ( _browsingHistory )
				{
					_historyIdx = _historyIdx == 0 ? 0 : _historyIdx - 1;
				}
				goto browsing;
				break;
			default: break;
			}
		}

		BaseWidget::keyPressEvent( event );
		return;
	browsing:
		_browsingHistory = true;
		setText( QString::fromStdString( _history.at( _history.size() - 1 - _historyIdx ) ) );
	}

	void CommandLauncher::_launchCommand()
	{
		_browsingHistory = false;
		_historyIdx		 = 0;
		if ( text().isEmpty() )
		{
			return;
		}

		_history.push_back( text().toStdString() );

		try
		{
			VTX_INFO( "CommandLauncher: {}", _history.back() );
			INTERPRETOR().runCommand( _history.back() );
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
