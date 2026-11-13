#include "app/ecs.hpp"
#include "app/python_binding/interpretor.hpp"
#include "app/services.hpp"
#include <string>
#include <util/thread/thread_manager.hpp>
#include <utility>
#include <vector>
//  pybind MUST be included before any QT because of macro shenanigans
#include "ui/qt/helper.hpp"
#include "ui/qt/widget/command_launcher.hpp"
#include <QStringList>
#include <qevent.h>

namespace VTX::UI::QT::Widget
{

	CommandLauncher::CommandLauncher( QWidget * const p_parent ) : BaseWidget( p_parent )
	{
		setPlaceholderText( "help()" );
		setClearButtonEnabled( true );
		setEnabled( false );

		connect( this, &QLineEdit::returnPressed, this, &CommandLauncher::_launchCommand );
		_pythonInitializedConnection
			= App::HUB().connect<App::Events::PythonInitialized, &CommandLauncher::_pythonInitialized>( this );
	}

	void CommandLauncher::_pythonInitialized( const App::Events::PythonInitialized & )
	{
		setEnabled( true );
		_setupCompleter();
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

	void CommandLauncher::_setupCompleter()
	{
#if VTX_PYTHON_BINDING
		if ( not App::ECS::hasCtx<App::PythonBinding::Interpretor>() )
		{
			return;
		}

		App::INTERPRETOR().requestFunctionList(
			[ launcher = QPointer<CommandLauncher>( this ) ]( std::vector<std::string> names )
			{
				App::THREAD().dispatch(
					[ launcher, names = std::move( names ) ]()
					{
						if ( not launcher )
						{
							return;
						}

						QStringList commands;
						commands.reserve( static_cast<qsizetype>( names.size() ) );
						for ( const std::string & name : names )
						{
							commands.append( QString::fromStdString( "vtx." + name ) );
						}

						launcher->_completer = Helper::makeQObject<QCompleter>( commands, launcher.data() );
						launcher->_completer->setCaseSensitivity( Qt::CaseInsensitive );
						launcher->_completer->setCompletionMode( QCompleter::PopupCompletion );
						launcher->setCompleter( launcher->_completer );
					}
				);
			}
		);
#endif
	}

} // namespace VTX::UI::QT::Widget
