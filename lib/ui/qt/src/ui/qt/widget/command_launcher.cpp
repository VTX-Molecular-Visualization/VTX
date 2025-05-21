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
		setClearButtonEnabled( true );

		connect( this, &QLineEdit::returnPressed, this, &CommandLauncher::_launchCommand );
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

	void CommandLauncher::_launchCommand()
	{
		_history.resetBrowsing();
		if ( text().isEmpty() )
		{
			return;
		}

		_history.add( text().toStdString() );

		if ( _history.last() == "/command" )
		{
			try
			{
				auto			  functions = INTERPRETOR().getModule().commands().getFunctionList();
				std::stringstream out;
				out << "Here is the list of the functions : " << std::endl; // TODO : Do it better
				for ( auto & function : functions )
					out << function << ", ";
				out << std::endl;

				VTX_INFO( "{}", out.str() );
			}
			catch ( CommandException & p_e )
			{
				clear();
				throw p_e;
			}
		}
		else
		{
			try
			{
				VTX_PYTHON_IN( "{}", _history.last() );
				std::string rslt = INTERPRETOR().runCommand( _history.last() );
				if ( not rslt.empty() )
					VTX_PYTHON_OUT( "{}", rslt );
			}
			catch ( CommandException & p_e )
			{
				clear();
				throw p_e;
			}
		}

		clear();
	}

	void CommandLauncher::_setupCompleter()
	{
		std::vector<std::string> allCommands = INTERPRETOR().getModule().commands().getFunctionList();

		QStringList strList;

		VTX_INFO( "Found functions :" );
		for ( auto & it_cmd : allCommands )
		{
			VTX_INFO( "{}", it_cmd );
			strList.emplaceBack();
		}

		_completer = new QCompleter( strList, this );
		_completer->setCaseSensitivity( Qt::CaseSensitivity::CaseInsensitive );
		_completer->setCompletionMode( QCompleter::CompletionMode::InlineCompletion );
		setCompleter( _completer );
	}

} // namespace VTX::UI::QT::Widget
