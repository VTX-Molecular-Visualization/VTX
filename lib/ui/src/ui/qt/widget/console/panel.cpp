#include "ui/qt/widget/console/panel.hpp"
#include "app/application/system/action_manager.hpp"
#include "ui/qt/core/custom/dock_window_main_widget.hpp"
#include "ui/qt/widget/console/action.hpp"
#include "ui/qt/widget/pytx/include_python_binding.hpp"
#include "ui/qt/widget_factory.hpp"
#include <QCompleter>
#include <QMenu>
#include <QVBoxLayout>
#include <app/vtx_app.hpp>
#include <util/enum.hpp>

namespace
{
	std::string LogLevelToStr( const VTX::Util::LOG_LEVEL p_level )
	{
		return std::string( VTX::Util::Enum::enumName( p_level ) );
	}
} // namespace

namespace VTX::UI::QT::Widget::Console
{
	Panel::Panel( QWidget * p_parent ) : QtDockablePanel( p_parent )
	{
		Util::Logger::get().onPrintLog += [ this ]( const Util::LogInfo & p_logInfo ) { _appendLog( p_logInfo ); };
	}

	void Panel::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QWidget * const mainWidget = _instantiateMainWidget( CONSOLE_PREFERRED_SIZE, CONSOLE_MINIMUM_SIZE );

		_listWidget = new QListWidget( this );
		_listWidget->setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );

		_promptWidget = WidgetFactory::get().instantiateWidget<CommandLinePrompt>( this, "_commandLineWidget" );

		QVBoxLayout * const mainLayout = new QVBoxLayout( mainWidget );
		mainLayout->setContentsMargins( 0, 0, 0, 0 );
		mainLayout->addWidget( _listWidget );
		mainLayout->addWidget( _promptWidget );

		setWidget( mainWidget );
	}

	void Panel::clearLogs()
	{
		_listWidgetMutex.lock();
		_listWidget->clear();
		_listWidgetMutex.unlock();
	}

	void Panel::_setupSlots()
	{
		connect(
			_listWidget,
			&QListWidget::customContextMenuRequested,
			this,
			[ & ]( const QPoint & p_pos )
			{
				QMenu			menu( this );
				QAction * const clearAction = new QAction( "Clear", this );
				connect( clearAction, &QAction::triggered, this, &Panel::_clearConsoleAction );
				menu.addAction( clearAction );
				menu.exec( _listWidget->mapToGlobal( p_pos ) );
			}
		);
	}

	void Panel::localize()
	{
		// Qt translate (not use currently)
		// setWindowTitle( QCoreApplication::translate( "ConsoleWidget", "Console", nullptr ) );
		setWindowTitle( "Console" );
	}

	void Panel::_appendLog( const Util::LogInfo & p_logInfo )
	{
		const std::string message
			= fmt::format( "[{}] [{}] {}", p_logInfo.date, LogLevelToStr( p_logInfo.level ), p_logInfo.message );

		QListWidgetItem * const newItem = new QListWidgetItem( QString::fromStdString( message ) );
		newItem->setData( Qt::ForegroundRole, _getMessageColor( p_logInfo.level ) );
		newItem->setFlags( Qt::ItemFlag::ItemNeverHasChildren );

		_listWidgetMutex.lock();
		_listWidget->addItem( newItem );
		_listWidgetMutex.unlock();

		if ( _listWidget->count() > _LOG_COUNT )
			_flush();

		// _appendLog can be called from a different thread
		// Qt events are not thread safe and need to be called from the main thread
		// We delayed the scrollToBottom on main thread at the end of frame.
		App::VTXApp::get().onEndOfFrameOneShot += [ this ]()
		{
			_listWidgetMutex.lock();
			_listWidget->scrollToBottom();
			_listWidgetMutex.unlock();
		};
	}
	void Panel::_flush()
	{
		QListWidgetItem * const itemToRemove = _listWidget->takeItem( 0 );
		_listWidgetMutex.lock();
		_listWidget->removeItemWidget( itemToRemove );
		_listWidgetMutex.unlock();
		delete itemToRemove;
	}

	QColor Panel::_getMessageColor( const Util::LOG_LEVEL p_level )
	{
		QColor res;

		switch ( p_level )
		{
		case Util::LOG_LEVEL::LOG_DEBUG: res = CONSOLE_DEBUG_COLOR; break;
		case Util::LOG_LEVEL::LOG_INFO: res = CONSOLE_INFO_COLOR; break;
		case Util::LOG_LEVEL::LOG_WARNING: res = CONSOLE_WARNING_COLOR; break;
		case Util::LOG_LEVEL::LOG_ERROR: res = CONSOLE_ERROR_COLOR; break;
		case Util::LOG_LEVEL::LOG_CRITICAL: res = CONSOLE_CRITICAL_COLOR; break;
		default: res = QColor(); break;
		}

		return res;
	}

	void Panel::_clearConsoleAction() { App::VTX_ACTION().execute<Action::ClearConsole>(); }

} // namespace VTX::UI::QT::Widget::Console