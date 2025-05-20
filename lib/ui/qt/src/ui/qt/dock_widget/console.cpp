#include "ui/qt/dock_widget/console.hpp"
#include "ui/qt/util.hpp"
#include <QApplication>
#include <QToolBar>

namespace VTX::UI::QT::DockWidget
{

	Console::Console( QWidget * p_parent ) : Core::BaseDockWidget<Console, 0, 0>( "Console", p_parent )
	{
		setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea );
		setMinimumHeight( 100 );

		// setBaseSize( 100, 100 );
		//  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );

		_listWidget = new QListWidget( _root );

		//_listWidget->setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents );
		//_listWidget->setMinimumHeight( 80 );
		//_listWidget->setSPolicy( QSizePolicy::Maximum, QSizePolicy::Minimum );
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

		LOGGER::onPrintLog += [ this ]( const ::VTX::Util::LogInfo & p_logInfo ) { _appendLog( p_logInfo ); };

		// Command launcher.
		_commandLauncher = new UI::QT::Widget::CommandLauncher( this );

		_layout->addWidget( _commandLauncher );

		this->installEventFilter( _filter );
	}

	void Console::clear()
	{
		_listWidgetMutex.lock();
		_listWidget->clear();
		_listWidgetMutex.unlock();
	}

	void Console::scrollToBottom() noexcept
	{
		_listWidgetMutex.lock();
		_listWidget->scrollToBottom();
		_listWidgetMutex.unlock();
	}

	void Console::_appendLog( const ::VTX::Util::LogInfo & p_logInfo )
	{
		const std::string message = fmt::format( "[{}] {}", p_logInfo.date, p_logInfo.message );

		QListWidgetItem * const newItem = new QListWidgetItem( QString::fromStdString( message ) );

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
			return; // Debug shall be seen in the terminal instead of flooding the Qt console
			newItem->setForeground( Qt::green );
		}

		newItem->setFlags( Qt::ItemFlag::ItemNeverHasChildren );

		_listWidgetMutex.lock();
		_listWidget->addItem( newItem );
		_listWidgetMutex.unlock();

		if ( _listWidget->count() > _LOG_COUNT )
		{
			_flush();
		}

		QApplication::postEvent( this, new QEvent( QT::Util::CustomEvent::ConsoleScrollToBottom ) );
	}

	void Console::_flush()
	{
		QListWidgetItem * const itemToRemove = _listWidget->takeItem( 0 );
		_listWidgetMutex.lock();
		_listWidget->removeItemWidget( itemToRemove );
		_listWidgetMutex.unlock();
		delete itemToRemove;
	}

	Console::ScrollToBottomFilter::ScrollToBottomFilter( Console & p_ ) : _console( &p_ ) {}

	bool Console::ScrollToBottomFilter::eventFilter( QObject * object, QEvent * event )
	{
		if ( reinterpret_cast<uint64_t>( object ) == reinterpret_cast<uint64_t>( _console )
			 and event->type() == QT::Util::CustomEvent::ConsoleScrollToBottom )
		{
			_console->scrollToBottom();
			return false;
		}
		return true;
	}

} // namespace VTX::UI::QT::DockWidget
