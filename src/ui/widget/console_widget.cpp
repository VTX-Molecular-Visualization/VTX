#include "console_widget.hpp"
#include <QCoreApplication>
#include <QListWidget>
#include <QWidget>
#include <iostream>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			ConsoleWidget::ConsoleWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) { _registerEvent( Event::Global::LOG_CONSOLE ); }

			ConsoleWidget::~ConsoleWidget() { delete _listWidget; }

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

				_listWidget = new QListWidget();
				_listWidget->setObjectName( QString::fromUtf8( "logList" ) );
				this->setWidget( _listWidget );

				QSizePolicy sizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
				this->setSizePolicy( sizePolicy );
			}

			void ConsoleWidget::_setupSlots() {};
			void ConsoleWidget::localize()
			{
				// Qt translate (not use currently)
				// setWindowTitle( QCoreApplication::translate( "ConsoleWidget", "Console", nullptr ) );
				setWindowTitle( "Console" );
			};

		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
