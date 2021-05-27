#include "console_widget.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "style.hpp"
#include "tool/logger.hpp"
#include "util/ui.hpp"
#include "vtx_app.hpp"
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QListWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <iostream>

namespace VTX::UI::Widget::Console
{
	ConsoleWidget::ConsoleWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_registerEvent( Event::Global::LOG_CONSOLE );
		_registerEvent( Event::Global::CLEAR_CONSOLE );
	}

	void ConsoleWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::LOG_CONSOLE )
		{
			const Event::VTXEventLog & event = dynamic_cast<const Event::VTXEventLog &>( p_event );

			const std::string		message = "[" + event.date + "] " + "[" + event.level + "] " + event.message;
			QListWidgetItem * const newItem = new QListWidgetItem( QString::fromStdString( message ) );
			newItem->setData( Qt::ForegroundRole, _getMessageColor( event.level ) );

			_listWidget->addItem( newItem );

			if ( uint( _listWidget->count() ) > _SIZE )
			{
				QListWidgetItem * const itemToRemove = _listWidget->takeItem( 0 );
				_listWidget->removeItemWidget( itemToRemove );
				delete itemToRemove;
			}

			_listWidget->scrollToBottom();
		}
		else if ( p_event.name == Event::Global::CLEAR_CONSOLE )
		{
			_listWidget->clear();
		}
	}

	QColor ConsoleWidget::_getMessageColor( const std::string & p_level )
	{
		QColor res;

		if ( p_level == Tool::Logger::LEVEL_STR[ int( Tool::Logger::LEVEL::LOG_LVL_DEBUG ) ] )
			res = Style::CONSOLE_DEBUG_COLOR;
		else if ( p_level == Tool::Logger::LEVEL_STR[ int( Tool::Logger::LEVEL::LOG_LVL_INFO ) ] )
			res = Style::CONSOLE_INFO_COLOR;
		else if ( p_level == Tool::Logger::LEVEL_STR[ int( Tool::Logger::LEVEL::LOG_LVL_WARNING ) ] )
			res = Style::CONSOLE_WARNING_COLOR;
		else if ( p_level == Tool::Logger::LEVEL_STR[ int( Tool::Logger::LEVEL::LOG_LVL_ERROR ) ] )
			res = Style::CONSOLE_ERROR_COLOR;
		else
			res = QColor();

		return res;
	}

	void ConsoleWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_mainWidget = new CustomWidget::DockWindowMainWidget<QWidget>( this );
		_mainWidget->setSizeHint( Style::CONSOLE_PREFERED_SIZE );
		_mainWidget->setMinimumSizeHint( Style::CONSOLE_MINIMUM_SIZE );

		_listWidget = new CustomWidget::DockWindowMainWidget<QListWidget>( this );
		_listWidget->setObjectName( QString::fromUtf8( "logList" ) );

		QSizePolicy sizePolicy = QSizePolicy( QSizePolicy::Policy::MinimumExpanding,
											  QSizePolicy::Policy::MinimumExpanding,
											  QSizePolicy::ControlType::Frame );
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

		QHBoxLayout * const mainLayout = new QHBoxLayout( _mainWidget );
		mainLayout->setSpacing( 5 );
		mainLayout->setContentsMargins( 0, 0, 0, 0 );

		mainLayout->addWidget( _listWidget, 100 );
		mainLayout->addLayout( buttonsLayout, 1 );

		setWidget( _mainWidget );
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

	void ConsoleWidget::_clearConsoleAction() { VTX_ACTION( new Action::Main::ClearConsoleInterface() ); }

} // namespace VTX::UI::Widget::Console
