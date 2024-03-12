#include "ui/qt/tool/console/widget/console_widget.hpp"
#include "app/application/system/action_manager.hpp"
#include "ui/qt/tool/console/action/console.hpp"
#include "ui/qt/widget/custom/dock_window_main_widget.hpp"
#include <QHBoxLayout>
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

namespace VTX::UI::QT::Tool::Console::Widget
{
	ConsoleWidget::ConsoleWidget( QWidget * p_parent ) : QtDockablePanel( p_parent )
	{
		Util::Logger::get().onPrintLog += [ this ]( const Util::LogInfo & p_logInfo ) { _appendLog( p_logInfo ); };
	}

	void ConsoleWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QWidget * const mainWidget = _instantiateMainWidget( CONSOLE_PREFERRED_SIZE, CONSOLE_MINIMUM_SIZE );

		_listWidget = new QListWidget( this );
		_listWidget->setObjectName( QString::fromUtf8( "logList" ) );

		QSizePolicy sizePolicy = QSizePolicy(
			QSizePolicy::Policy::MinimumExpanding,
			QSizePolicy::Policy::MinimumExpanding,
			QSizePolicy::ControlType::Frame
		);
		sizePolicy.setHorizontalStretch( 10 );
		sizePolicy.setVerticalStretch( 10 );
		_listWidget->setSizePolicy( sizePolicy );

		_clearWidget = new QPushButton( this );
		_clearWidget->setText( "Clear" );
		_clearWidget->setSizePolicy( QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum );

		QVBoxLayout * const buttonsLayout = new QVBoxLayout();
		buttonsLayout->setContentsMargins( 0, 0, 0, 0 );
		buttonsLayout->addWidget( _clearWidget );
		buttonsLayout->addStretch( 1000 );

		QHBoxLayout * const mainLayout = new QHBoxLayout( mainWidget );
		mainLayout->setSpacing( 5 );
		mainLayout->setContentsMargins( 0, 0, 2, 0 );

		mainLayout->addWidget( _listWidget, 100 );
		mainLayout->addLayout( buttonsLayout, 1 );

		setWidget( mainWidget );
	}

	void ConsoleWidget::clearLogs()
	{
		_listWidgetMutex.lock();
		_listWidget->clear();
		_listWidgetMutex.unlock();
	}

	void ConsoleWidget::_setupSlots()
	{
		connect( _clearWidget, &QPushButton::clicked, this, &ConsoleWidget::_clearConsoleAction );
	};
	void ConsoleWidget::localize()
	{
		// Qt translate (not use currently)
		// setWindowTitle( QCoreApplication::translate( "ConsoleWidget", "Console", nullptr ) );
		setWindowTitle( "Console" );
	};
	void ConsoleWidget::_appendLog( const Util::LogInfo & p_logInfo )
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
		App::VTXApp::get().onEndOfFrameOneShot().addCallback(
			this,
			[ this ]()
			{
				_listWidgetMutex.lock();
				_listWidget->scrollToBottom();
				_listWidgetMutex.unlock();
			}
		);
	}
	void ConsoleWidget::_flush()
	{
		QListWidgetItem * const itemToRemove = _listWidget->takeItem( 0 );
		_listWidgetMutex.lock();
		_listWidget->removeItemWidget( itemToRemove );
		_listWidgetMutex.unlock();
		delete itemToRemove;
	}

	QColor ConsoleWidget::_getMessageColor( const Util::LOG_LEVEL p_level )
	{
		QColor res;

		switch ( p_level )
		{
		case Util::LOG_LEVEL::DEBUG: res = CONSOLE_DEBUG_COLOR; break;
		case Util::LOG_LEVEL::INFO: res = CONSOLE_INFO_COLOR; break;
		case Util::LOG_LEVEL::WARNING: res = CONSOLE_WARNING_COLOR; break;
		case Util::LOG_LEVEL::ERROR: res = CONSOLE_ERROR_COLOR; break;
		case Util::LOG_LEVEL::CRITICAL: res = CONSOLE_CRITICAL_COLOR; break;
		default: res = QColor(); break;
		}

		return res;
	}

	void ConsoleWidget::_clearConsoleAction() { App::VTX_ACTION().execute<Action::ClearConsole>(); }

} // namespace VTX::UI::QT::Tool::Console::Widget
