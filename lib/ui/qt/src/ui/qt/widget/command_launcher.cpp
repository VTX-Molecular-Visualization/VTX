#include <python_binding/interpretor.hpp>
// pybind MUST be included before any QT because of macro shenanigans
#include "ui/qt/widget/command_launcher.hpp"
#include <app/core/threading/threading_system.hpp>
#include <qevent.h>
#include <util/logger.hpp>

namespace VTX::UI::QT::Widget
{

	CommandLauncher::CommandLauncher( QWidget * p_parent ) : BaseWidget( p_parent )
	{
		setPlaceholderText( "help()" );
		_setupCompleter();
		setClearButtonEnabled( true );

		connect( this, &QLineEdit::returnPressed, this, &CommandLauncher::_launchCommand );
	}

	void CommandLauncher::launchCommand( std::string p_ )
	{
		_history.add( std::move( p_ ) );
		_executeLastHistoryEntry();
	}

	void CommandLauncher::keyPressEvent( QKeyEvent * event )
	{
		if ( not _history.empty() )
		{
			switch ( event->key() )
			{
			case Qt::Key_Up:
				_history.tryMoveBackward();
				setText( QString::fromStdString( _history.currentString() ) );
				return;
			case Qt::Key_Down:
				_history.tryMoveForward();
				setText( QString::fromStdString( _history.currentString() ) );
				return;
			default: break;
			}
		}

		BaseWidget::keyPressEvent( event );
		return;
	}

	void CommandLauncher::focusInEvent( QFocusEvent * event ) {}

	void CommandLauncher::focusOutEvent( QFocusEvent * event ) {}

	void CommandLauncher::_launchCommand()
	{
		if ( text().isEmpty() )
		{
			return;
		}

		// We do not want to register the same command over and over as it give the impression that the browsing doesn't
		// work from the user perspective.
		std::string command = text().toStdString();
		if ( _history.empty() || command != _history.last() )
			_history.add( text().toStdString() );

		clear();
		_executeLastHistoryEntry();
	}

	void CommandLauncher::_executeLastHistoryEntry()
	{
		_history.resetBrowsing();

		// TODO
		_history.last();
	}

	void CommandLauncher::_setupCompleter()
	{
		// TODO : Do it nicely
		//  std::vector<std::string> allCommands = INTERPRETOR().getModule().commands().getFunctionList();

		// QStringList strList;

		// for ( auto & it_cmd : allCommands )
		//{
		//	strList.emplaceBack();
		// }

		//_completer = new QCompleter( strList, this );
		//_completer->setCaseSensitivity( Qt::CaseSensitivity::CaseInsensitive );
		//_completer->setCompletionMode( QCompleter::CompletionMode::InlineCompletion );
		// setCompleter( _completer );
	}

} // namespace VTX::UI::QT::Widget
