#include "console_widget.hpp"
#include "style.hpp"
#include "tool/logger.hpp"
#include "util/ui.hpp"
#include "vtx_app.hpp"
#include <QCoreApplication>
#include <QListWidget>
#include <QWidget>
#include <iostream>

namespace VTX::UI::Widget::Console
{
	ConsoleWidget::ConsoleWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
	{
		_registerEvent( Event::Global::LOG_CONSOLE );
	}

	void ConsoleWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		const Event::VTXEventLog & event   = dynamic_cast<const Event::VTXEventLog &>( p_event );
		QListWidget * const		   list	   = _listWidget;
		const std::string		   message = "[" + event.date + "] " + "[" + event.level + "] " + event.message;
		QListWidgetItem * const	   newItem = new QListWidgetItem( QString::fromStdString( message ) );

		newItem->setData( Qt::ForegroundRole, _getMessageColor( event.level ) );
		list->addItem( newItem );

		list->scrollToBottom();
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

		_listWidget = new CustomWidget::DockWindowMainWidget<QListWidget>( this );
		_listWidget->setObjectName( QString::fromUtf8( "logList" ) );
		_listWidget->setSizeHint( Style::CONSOLE_PREFERED_SIZE );
		_listWidget->setMinimumSizeHint( Style::CONSOLE_MINIMUM_SIZE );

		QSizePolicy sizePolicy = QSizePolicy( QSizePolicy::Policy::MinimumExpanding,
											  QSizePolicy::Policy::MinimumExpanding,
											  QSizePolicy::ControlType::Frame );
		sizePolicy.setHorizontalStretch( 10 );
		sizePolicy.setVerticalStretch( 10 );
		_listWidget->setSizePolicy( sizePolicy );

		setWidget( _listWidget );
	}

	void ConsoleWidget::_setupSlots() {};
	void ConsoleWidget::localize()
	{
		// Qt translate (not use currently)
		// setWindowTitle( QCoreApplication::translate( "ConsoleWidget", "Console", nullptr ) );
		setWindowTitle( "Console" );
	};

} // namespace VTX::UI::Widget::Console
