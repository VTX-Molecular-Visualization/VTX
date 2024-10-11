#include "ui/qt/dock_widget/console.hpp"
#include <QToolBar>

namespace VTX::UI::QT::DockWidget
{

	Console::Console( QWidget * p_parent ) : Core::BaseDockWidget<Console>( "Console", p_parent )
	{
		setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea );
		setMinimumHeight( 120 );

		_scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
		_scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

		// setBaseSize( 100, 100 );
		//  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::MinimumExpanding );

		_listWidget = new QListWidget( _root );

		//_listWidget->setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents );
		_listWidget->setMinimumHeight( 100 );
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

		LOGGER::onPrintLog += [ this ]( const Util::LogInfo & p_logInfo ) { _appendLog( p_logInfo ); };

		// Command launcher.
		_commandLauncher = new LineEdit::CommandLauncher( this );

		_layout->addWidget( _commandLauncher );
	}

	void Console::clear()
	{
		_listWidgetMutex.lock();
		_listWidget->clear();
		_listWidgetMutex.unlock();
	}

	void Console::_appendLog( const Util::LogInfo & p_logInfo )
	{
		const std::string message = fmt::format( "[{}] {}", p_logInfo.date, p_logInfo.message );

		QListWidgetItem * const newItem = new QListWidgetItem( QString::fromStdString( message ) );

		// TODO: Use palette color?
		if ( p_logInfo.level == Util::LOG_LEVEL::LOG_ERROR )
		{
			newItem->setForeground( Qt::red );
		}
		else if ( p_logInfo.level == Util::LOG_LEVEL::LOG_WARNING )
		{
			newItem->setForeground( Qt::yellow );
		}
		else if ( p_logInfo.level == Util::LOG_LEVEL::LOG_DEBUG )
		{
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

		// _appendLog can be called from a different thread
		// Qt events are not thread safe and need to be called from the main thread
		// We delayed the scrollToBottom on main thread at the end of frame.

		APP::onEndOfFrameOneShot += [ this ]()
		{
			_listWidgetMutex.lock();
			_listWidget->scrollToBottom();
			_listWidgetMutex.unlock();
		};
	}

	void Console::_flush()
	{
		QListWidgetItem * const itemToRemove = _listWidget->takeItem( 0 );
		_listWidgetMutex.lock();
		_listWidget->removeItemWidget( itemToRemove );
		_listWidgetMutex.unlock();
		delete itemToRemove;
	}

} // namespace VTX::UI::QT::DockWidget
