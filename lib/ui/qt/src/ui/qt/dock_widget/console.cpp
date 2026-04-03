#include "ui/qt/dock_widget/console.hpp"
#include <QApplication>
#include <QToolBar>

namespace VTX::UI::QT::DockWidget
{

	Console::Console( QWidget * p_parent ) : BaseDockWidget( p_parent, "Console" )
	{
		setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea );

		// Log area.
		_textEdit = new QPlainTextEdit( _root );
		_textEdit->setReadOnly( true );
		_textEdit->setWordWrapMode( QTextOption::WrapAnywhere );
		_textEdit->setVerticalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAsNeeded );
		_textEdit->setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );

		// Context menu.
		connect(
			_textEdit,
			&QPlainTextEdit::customContextMenuRequested,
			this,
			[ & ]( const QPoint & p_pos )
			{
				QMenu			menu( this );
				QAction * const clearAction = new QAction( "Clear", &menu );
				connect( clearAction, &QAction::triggered, this, &Console::clear );
				menu.addAction( clearAction );
				menu.exec( _textEdit->mapToGlobal( p_pos ) );
			}
		);

		// Get logs and push to main thread.
		_callbackId = LOGGER::onLog += [ this ]( const ::VTX::Util::LogInfo & p_logInfo )
		{ QMetaObject::invokeMethod( this, [ this, p_logInfo ]() { log( p_logInfo ); }, Qt::QueuedConnection ); };

		// Command launcher.
		_commandLauncher = new UI::QT::Widget::CommandLauncher( this );

		_layout->addWidget( _textEdit );
		_layout->addWidget( _commandLauncher );
	}

	Console::~Console() { LOGGER::onLog -= _callbackId; }

	void Console::log( const VTX::Util::LogInfo & p_logInfo )
	{
		const std::string message = fmt::format( "[{}] {}", p_logInfo.date, p_logInfo.message );

		// TODO: Use palette color.
		QColor color = _textEdit->palette().text().color();
		if ( p_logInfo.hint == VTX::Util::LOG_HINT::STD )
		{
			if ( p_logInfo.level == ::VTX::Util::LOG_LEVEL::LOG_ERROR )
				color = Qt::red;
			else if ( p_logInfo.level == ::VTX::Util::LOG_LEVEL::LOG_WARNING )
				color = Qt::yellow;
			else if ( p_logInfo.level == ::VTX::Util::LOG_LEVEL::LOG_DEBUG )
				color = Qt::darkCyan;
		}
		else if ( p_logInfo.hint == VTX::Util::LOG_HINT::PY_IN )
			color = Qt::green;
		else if ( p_logInfo.hint == VTX::Util::LOG_HINT::PY_OUT )
			color = Qt::darkGreen;

		QTextCharFormat format;
		format.setForeground( color );

		QTextCursor cursor = _textEdit->textCursor();
		cursor.movePosition( QTextCursor::End );
		if ( !_textEdit->document()->isEmpty() )
			cursor.insertBlock();
		cursor.setCharFormat( format );
		cursor.insertText( QString::fromStdString( message ) );

		if ( _textEdit->document()->blockCount() > static_cast<int>( CONSOLE_LOG_COUNT ) )
		{
			QTextCursor removeCursor( _textEdit->document() );
			removeCursor.movePosition( QTextCursor::Start );
			removeCursor.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );
			removeCursor.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor );
			removeCursor.removeSelectedText();
		}

		_textEdit->moveCursor( QTextCursor::End );
		_textEdit->ensureCursorVisible();
	}

	void Console::clear() { _textEdit->clear(); }

} // namespace VTX::UI::QT::DockWidget
