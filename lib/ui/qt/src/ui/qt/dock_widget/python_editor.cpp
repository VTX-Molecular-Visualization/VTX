#include "ui/qt/dock_widget/python_editor.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QFile>
#include <QSaveFile>
#include <QToolBar>
#include <app/action/io.hpp>
#include <app/services.hpp>
#include <util/event_hub.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT::DockWidget
{
	PythonEditor::PythonEditor( QWidget * const p_parent ) : BaseDockWidget( p_parent, "Python editor" )
	{
		setAllowedAreas( Qt::TopDockWidgetArea | Qt::RightDockWidgetArea );

		auto * const toolbar = new QToolBar( _root );
		toolbar->setMovable( false );
		toolbar->setFloatable( false );
		toolbar->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
		_runAction = toolbar->addAction( STYLE().iconFromCodepoint( Style::Icons::SCRIPT_RUN ), "Run" );
		_runAction->setEnabled( false );

		_textEdit	 = new QPlainTextEdit( _root );
		_highlighter = new PythonHighlighter( _textEdit->document() );
		_layout->addWidget( toolbar );
		_layout->addWidget( _textEdit );

		connect( _runAction, &QAction::triggered, this, &PythonEditor::_saveAndRun );
		App::HUB().connect<Events::ScriptSelected, &PythonEditor::_onScriptSelected>( this );
	}

	PythonEditor::~PythonEditor() { App::HUB().disconnectAllOf( *this ); }

	void PythonEditor::_onScriptSelected( const Events::ScriptSelected & p_event )
	{
		show();
		raise();

		QFile file( QString::fromStdString( p_event.path.string() ) );
		if ( not file.open( QIODevice::ReadOnly | QIODevice::Text ) )
		{
			_currentPath.clear();
			_runAction->setEnabled( false );
			_textEdit->clear();
			return;
		}

		_currentPath = p_event.path;
		_runAction->setEnabled( true );
		_textEdit->setPlainText( QString::fromUtf8( file.readAll() ) );
	}

	void PythonEditor::_saveAndRun()
	{
		if ( _currentPath.empty() )
		{
			return;
		}

		QSaveFile file( QString::fromStdString( _currentPath.string() ) );
		if ( not file.open( QIODevice::WriteOnly ) )
		{
			VTX_WARNING( "Unable to open script <{}> for writing.", _currentPath.string() );
			return;
		}

		const QByteArray content = _textEdit->toPlainText().toUtf8();
		if ( file.write( content ) != content.size() || not file.commit() )
		{
			VTX_WARNING( "Unable to save script <{}>.", _currentPath.string() );
			return;
		}

		App::ACTION().execute<App::Action::IO::RunPythonScript>( _currentPath );
	}
} // namespace VTX::UI::QT::DockWidget
