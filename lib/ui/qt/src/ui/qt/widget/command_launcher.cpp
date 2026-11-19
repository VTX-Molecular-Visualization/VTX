#include "ui/qt/widget/command_launcher.hpp"
#include "app/python_binding/interpretor.hpp"
#include "app/services.hpp"
#include "ui/qt/helper.hpp"
#include <QAbstractItemView>
#include <qevent.h>
#include <string>
#include <utility>

namespace VTX::UI::QT::Widget
{

	CommandLauncher::CommandLauncher( QWidget * const p_parent ) :
		BaseWidget( p_parent ), _completer( Helper::makeQObject<PythonCompleter>( this ) )
	{
		setPlaceholderText( "help()" );
		setClearButtonEnabled( true );
		setEnabled( false );

		connect( this, &QLineEdit::returnPressed, this, &CommandLauncher::_launchCommand );
		connect(
			_completer, qOverload<const QString &>( &QCompleter::activated ), this, &CommandLauncher::_insertCompletion
		);
		_pythonInitializedConnection
			= App::HUB().connect<App::Events::PythonInitialized, &CommandLauncher::_pythonInitialized>( this );
	}

	void CommandLauncher::_pythonInitialized( const App::Events::PythonInitialized & ) { setEnabled( true ); }

	void CommandLauncher::launchCommand( std::string p_ )
	{
		_history.add( std::move( p_ ) );
		_executeLastHistoryEntry();
	}

	void CommandLauncher::keyPressEvent( QKeyEvent * event )
	{
		if ( _completer->popup()->isVisible() )
		{
			switch ( event->key() )
			{
			case Qt::Key_Enter:
			case Qt::Key_Return:
			case Qt::Key_Escape:
			case Qt::Key_Tab:
			case Qt::Key_Backtab: event->ignore(); return;
			default: break;
			}
		}

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

		if ( event->text().isEmpty() || not _completer->updateCompletions( text().left( cursorPosition() ), {} ) )
		{
			_completer->popup()->hide();
			return;
		}

		_completer->showPopup( cursorRect() );
	}

	void CommandLauncher::_insertCompletion( const QString & p_completion )
	{
		const int prefixSize = static_cast<int>( _completer->completionPrefix().size() );
		setSelection( cursorPosition() - prefixSize, prefixSize );
		insert( p_completion );
	}

	void CommandLauncher::_launchCommand()
	{
		if ( text().isEmpty() )
		{
			return;
		}

		// We do not want to register the same command over and over as it give the impression that the browsing doesn't
		// work from the user perspective.
		const std::string command = text().toStdString();
		if ( _history.empty() || command != _history.last() )
		{
			_history.add( text().toStdString() );
		}

		clear();
		_executeLastHistoryEntry();
	}

	void CommandLauncher::_executeLastHistoryEntry()
	{
		_history.resetBrowsing();

		App::INTERPRETOR().runCommand( _history.last() );
	}

} // namespace VTX::UI::QT::Widget
