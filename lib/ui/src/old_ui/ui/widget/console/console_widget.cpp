#include "ui/old_ui/ui/widget/console/console_widget.hpp"
#include "ui/old_ui/action/main.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/util/ui.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QListWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <app/old/action/main.hpp>
#include <app/old/event/global.hpp>
#include <iostream>
#include <util/logger.hpp>

namespace VTX::UI::Widget::Console
{
	ConsoleWidget::ConsoleWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_registerEvent( VTX::App::Old::Event::Global::LOG_CONSOLE );
		_registerEvent( VTX::App::Old::Event::Global::CLEAR_CONSOLE );
	}

	void ConsoleWidget::receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Old::Event::Global::LOG_CONSOLE )
		{
			const VTX::App::Old::Core::Event::VTXEventArg<std::string, std::string, std::string> & event
				= dynamic_cast<const VTX::App::Old::Core::Event::VTXEventArg<std::string, std::string, std::string> &>(
					p_event );

			const std::string logLevel	 = event.get<0>();
			const std::string logDate	 = event.get<1>();
			const std::string logMessage = event.get<2>();

			const std::string		message = "[" + logDate + "] " + "[" + logLevel + "] " + logMessage;
			QListWidgetItem * const newItem = new QListWidgetItem( QString::fromStdString( message ) );
			newItem->setData( Qt::ForegroundRole, _getMessageColor( logLevel ) );

			_listWidget->addItem( newItem );

			if ( uint( _listWidget->count() ) > _SIZE )
			{
				QListWidgetItem * const itemToRemove = _listWidget->takeItem( 0 );
				_listWidget->removeItemWidget( itemToRemove );
				delete itemToRemove;
			}

			_listWidget->scrollToBottom();
		}
		else if ( p_event.name == VTX::App::Old::Event::Global::CLEAR_CONSOLE )
		{
			_listWidget->clear();
		}
	}

	QColor ConsoleWidget::_getMessageColor( const std::string & p_level )
	{
		/*
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
		*/

		return Style::CONSOLE_INFO_COLOR;
	}

	void ConsoleWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_mainWidget = new CustomWidget::DockWindowMainWidget<QWidget>(
			Style::CONSOLE_PREFERRED_SIZE, Style::CONSOLE_MINIMUM_SIZE, this );

		_listWidget = new QListWidget( this );
		_listWidget->setObjectName( QString::fromStdString( "logList" ) );

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
		mainLayout->setContentsMargins( 0, 0, 2, 0 );

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
