#include "ui/qt/dock_widget/python_editor.hpp"
#include <QFile>
#include <app/services.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::DockWidget
{
	PythonEditor::PythonEditor( QWidget * const p_parent ) : BaseDockWidget( p_parent, "Python editor" )
	{
		setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea | Qt::RightDockWidgetArea );

		_textEdit	 = new QPlainTextEdit( _root );
		_highlighter = new PythonHighlighter( _textEdit->document() );
		_layout->addWidget( _textEdit );

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
			_textEdit->clear();
			return;
		}

		_textEdit->setPlainText( QString::fromUtf8( file.readAll() ) );
	}
} // namespace VTX::UI::QT::DockWidget
