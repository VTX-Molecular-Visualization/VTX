#include "ui/qt/dock_widget/console.hpp"
#include <QApplication>
#include <QToolBar>

namespace VTX::UI::QT::DockWidget
{

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

		// Get logs and push to main thread.
		_onPrintLogCallbackId = LOGGER::onPrintLog += [ this ]( const ::VTX::Util::LogInfo & p_logInfo )
		{ QMetaObject::invokeMethod( this, [ this, p_logInfo ]() { log( p_logInfo ); }, Qt::QueuedConnection ); };

		// Command launcher.
		_commandLauncher = new UI::QT::Widget::CommandLauncher( this );

		_layout->addWidget( _commandLauncher );
	}

	Console::~Console() { LOGGER::onPrintLog -= _onPrintLogCallbackId; }

	void Console::log( const VTX::Util::LogInfo & p_logInfo )
	{
		const std::string message = fmt::format( "[{}] {}", p_logInfo.date, p_logInfo.message );
		QListWidgetItem * newItem = new QListWidgetItem( QString::fromStdString( message ) );

		// TODO: Use palette color?
		if ( p_logInfo.level == ::VTX::Util::LOG_LEVEL::LOG_ERROR )
		{
			newItem->setForeground( Qt::red );
		}
		else if ( p_logInfo.level == ::VTX::Util::LOG_LEVEL::LOG_WARNING )
		{
			newItem->setForeground( Qt::yellow );
		}
		else if ( p_logInfo.level == ::VTX::Util::LOG_LEVEL::LOG_DEBUG )
		{
			newItem->setForeground( Qt::green );
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
	}

	void Console::clear() { _listWidget->clear(); }

} // namespace VTX::UI::QT::DockWidget
