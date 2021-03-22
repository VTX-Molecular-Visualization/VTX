#include "console_widget.hpp"
#include "style.hpp"
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
		const Event::VTXEventLog & event = dynamic_cast<const Event::VTXEventLog &>( p_event );
		QListWidget * const		   list	 = _listWidget;
		list->addItem( QString( ( "[" + event.date + "] " + "[" + event.level + "] " + event.message ).c_str() ) );
		list->scrollToBottom();
	}

	void ConsoleWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_listWidget = new CustomWidget::DockWindowMainWidget<QListWidget>( this );
		_listWidget->setObjectName( QString::fromUtf8( "logList" ) );
		_listWidget->setSizeHint( Style::CONSOLE_PREFERED_SIZE );
		_listWidget->setMinimumSizeHint( Style::CONSOLE_MINIMUM_SIZE );

		this->setWidget( _listWidget );
	}

	void ConsoleWidget::_setupSlots() {};
	void ConsoleWidget::localize()
	{
		// Qt translate (not use currently)
		// setWindowTitle( QCoreApplication::translate( "ConsoleWidget", "Console", nullptr ) );
		setWindowTitle( "Console" );
	};

} // namespace VTX::UI::Widget::Console
