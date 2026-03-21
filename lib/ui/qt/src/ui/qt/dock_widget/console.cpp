#include "ui/qt/dock_widget/console.hpp"
#include "ui/qt/util.hpp"
#include <QApplication>
#include <QToolBar>

namespace VTX::UI::QT::DockWidget
{
	Console::_AppendLogEvent::_AppendLogEvent( const ::VTX::Util::LogInfo & p_logInfo ) :
		QEvent( QT::Util::CustomEvent::ConsoleAppendLog ), logInfo( p_logInfo )
	{
	}

	Console::Console( QWidget * p_parent ) : BaseDockWidget( p_parent, "Console" )
	{
		setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea );

		_listWidget = new QListWidget( _root );
		_listWidget->setWordWrap( true );
		_listWidget->setVerticalScrollBarPolicy( Qt::ScrollBarPolicy::ScrollBarAsNeeded );

		_listWidget->setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );

		_layout->addWidget( _listWidget );

		connect(
			_listWidget,
			&QListWidget::customContextMenuRequested,
			this,
			[ & ]( const QPoint & p_pos )
			{
				QMenu			menu( this );
				QAction * const clearAction = new QAction( "Clear", &menu );
				connect( clearAction, &QAction::triggered, this, &Console::clear );
				menu.addAction( clearAction );
				menu.exec( _listWidget->mapToGlobal( p_pos ) );
			}
		);

		_onPrintLogCallbackId = LOGGER::onPrintLog += [ this ]( const ::VTX::Util::LogInfo & p_logInfo )
		{ QApplication::postEvent( this, new _AppendLogEvent( p_logInfo ) ); };

		// Command launcher.
		_commandLauncher = new UI::QT::Widget::CommandLauncher( this );

		_layout->addWidget( _commandLauncher );
	}

	Console::~Console() { LOGGER::onPrintLog -= _onPrintLogCallbackId; }

	void Console::clear() { _listWidget->clear(); }

	bool Console::event( QEvent * p_event )
	{
		if ( p_event->type() == QT::Util::CustomEvent::ConsoleAppendLog )
		{
			const _AppendLogEvent *	   appendLogEvent = static_cast<_AppendLogEvent *>( p_event );
			const VTX::Util::LogInfo & logInfo		  = appendLogEvent->logInfo;
			const std::string		   message		  = fmt::format( "[{}] {}", logInfo.date, logInfo.message );
			QListWidgetItem *		   newItem		  = new QListWidgetItem( QString::fromStdString( message ) );

			// TODO: Use palette color?
			if ( logInfo.level == ::VTX::Util::LOG_LEVEL::LOG_ERROR )
			{
				newItem->setForeground( Qt::red );
			}
			else if ( logInfo.level == ::VTX::Util::LOG_LEVEL::LOG_WARNING )
			{
				newItem->setForeground( Qt::yellow );
			}
			else if ( logInfo.level == ::VTX::Util::LOG_LEVEL::LOG_DEBUG )
			{
				newItem->setForeground( Qt::green );
			}
			else if ( logInfo.level == ::VTX::Util::LOG_LEVEL::LOG_PYTHON_IN )
			{
				newItem->setForeground( Qt::green );
			}
			else if ( logInfo.level == ::VTX::Util::LOG_LEVEL::LOG_PYTHON_OUT )
			{
				newItem->setForeground( Qt::darkGreen );
			}

			newItem->setFlags( Qt::ItemFlag::ItemNeverHasChildren );

			_listWidget->addItem( newItem );

			if ( _listWidget->count() > CONSOLE_LOG_COUNT )
			{
				QListWidgetItem * const itemToRemove = _listWidget->takeItem( 0 );
				_listWidget->removeItemWidget( itemToRemove );
				delete itemToRemove;
			}

			_listWidget->scrollToBottom();

			return true;
		}
		return BaseDockWidget<Console, 0, 0>::event( p_event );
	}

} // namespace VTX::UI::QT::DockWidget
