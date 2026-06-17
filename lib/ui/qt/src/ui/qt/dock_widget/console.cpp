#include "ui/qt/dock_widget/console.hpp"
#include "ui/qt/helper.hpp"
#include <QApplication>
#include <QDesktopServices>
#include <QRegularExpression>
#include <QToolBar>
#include <QUrl>
#include <app/constants.hpp>
#include <filesystem>
#include <string>
#include <system_error>

namespace VTX::UI::QT::DockWidget
{
	namespace
	{
		// Get color from log level/hint.
		// TODO: Use palette color.
		QColor _getLogColor( const VTX::Util::LogInfo & p_logInfo, const QColor & p_defaultColor )
		{
			if ( p_logInfo.hint == VTX::Util::LOG_HINT::STD )
			{
				if ( p_logInfo.level == ::VTX::Util::LOG_LEVEL::LOG_ERROR )
				{
					return Qt::red;
				}
				if ( p_logInfo.level == ::VTX::Util::LOG_LEVEL::LOG_WARNING )
				{
					return Qt::yellow;
				}
				if ( p_logInfo.level == ::VTX::Util::LOG_LEVEL::LOG_DEBUG )
				{
					return Qt::darkCyan;
				}
			}
			else if ( p_logInfo.hint == VTX::Util::LOG_HINT::PY_IN )
			{
				return Qt::green;
			}
			else if ( p_logInfo.hint == VTX::Util::LOG_HINT::PY_OUT )
			{
				return Qt::darkGreen;
			}

			return p_defaultColor;
		}

		// Print log and insert links.
		void _insertLogText( QTextCursor & p_cursor, const QString & p_text, const QTextCharFormat & p_format )
		{
			static const QRegularExpression linkRegex( Helper::toQString( ::VTX::App::LOG_LINK_REGEX ) );

			qsizetype						current = 0;
			QRegularExpressionMatchIterator matches = linkRegex.globalMatch( p_text );

			while ( matches.hasNext() )
			{
				const QRegularExpressionMatch match = matches.next();
				const QString				  label = match.captured( 1 );
				const QUrl					  url	= QUrl::fromLocalFile( label );

				p_cursor.insertText( p_text.sliced( current, match.capturedStart() - current ), p_format );

				QTextCharFormat linkFormat = p_format;
				linkFormat.setAnchor( true );
				linkFormat.setAnchorHref( url.toString() );
				linkFormat.setFontUnderline( true );
				p_cursor.insertText( label, linkFormat );

				current = match.capturedEnd();
			}

			p_cursor.insertText( p_text.sliced( current ), p_format );
		}

		// Open link.
		void _openConsoleLink( const QUrl & p_url )
		{
			if ( not p_url.isLocalFile() )
			{
				return;
			}

			const FilePath path = p_url.toLocalFile().toStdString();

			std::error_code error;
			const FilePath	target = std::filesystem::is_directory( path, error ) ? path : path.parent_path();
			if ( target.empty() )
			{
				return;
			}

			QDesktopServices::openUrl( QUrl::fromLocalFile( QString::fromStdString( target.string() ) ) );
		}

		// Purge.
		void _purgeOldLogs( QTextDocument & p_document )
		{
			if ( p_document.blockCount() <= static_cast<int>( CONSOLE_LOG_COUNT ) )
			{
				return;
			}

			QTextCursor removeCursor( &p_document );
			removeCursor.movePosition( QTextCursor::Start );
			removeCursor.movePosition( QTextCursor::EndOfBlock, QTextCursor::KeepAnchor );
			removeCursor.movePosition( QTextCursor::NextCharacter, QTextCursor::KeepAnchor );
			removeCursor.removeSelectedText();
		}
	} // namespace

	Console::Console( QWidget * p_parent ) : BaseDockWidget( p_parent, "Console" )
	{
		setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea );

		// Log area.
		_textEdit = new QTextBrowser( _root );
		_textEdit->setReadOnly( true );
		_textEdit->setWordWrapMode( QTextOption::WrapAnywhere );
		_textEdit->setVerticalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAsNeeded );
		_textEdit->setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );
		_textEdit->setOpenLinks( false );
		_textEdit->setOpenExternalLinks( false );

		connect(
			_textEdit, &QTextBrowser::anchorClicked, this, []( const QUrl & p_url ) { _openConsoleLink( p_url ); }
		);

		// Context menu.
		connect(
			_textEdit,
			&QTextBrowser::customContextMenuRequested,
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
		if ( p_logInfo.message == "\n" )
		{
			return;
		}
		const std::string message = fmt::format( "[{}] {}", p_logInfo.date, p_logInfo.message );
		const QColor	  color	  = _getLogColor( p_logInfo, _textEdit->palette().text().color() );

		QTextCharFormat format;
		format.setForeground( color );

		QTextCursor cursor = _textEdit->textCursor();
		cursor.movePosition( QTextCursor::End );
		if ( not _textEdit->document()->isEmpty() )
		{
			cursor.insertBlock();
		}
		cursor.setCharFormat( format );
		_insertLogText( cursor, QString::fromStdString( message ), format );

		_purgeOldLogs( *_textEdit->document() );

		_textEdit->moveCursor( QTextCursor::End );
		_textEdit->ensureCursorVisible();
	}

	void Console::clear() { _textEdit->clear(); }

} // namespace VTX::UI::QT::DockWidget
