#include "console_widget.hpp"
#include <QCoreApplication>
#include <QListWidget>
#include <QtWidgets/QWidget>
#include <iostream>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			ConsoleWidget::ConsoleWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) { _registerEvent( Event::Global::LOG_CONSOLE ); }

			void ConsoleWidget::receiveEvent( const Event::VTXEvent & p_event )
			{
				const Event::VTXEventLog & event = dynamic_cast<const Event::VTXEventLog &>( p_event );
				QListWidget * const		   list	 = this->listWidget;
				list->addItem( QString( ( "[" + event.date + "] " + "[" + event.level + "] " + event.message ).c_str() ) );
				list->scrollToBottom();
			}

			void ConsoleWidget::setupUi()
			{
				setObjectName( "consoleWidget" );

				listWidget = new QListWidget();
				listWidget->setObjectName( QString::fromUtf8( "logList" ) );
				this->setWidget( listWidget );

				QSizePolicy sizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
				this->setSizePolicy( sizePolicy );
			}

			void ConsoleWidget::setupSlots() {};
			void ConsoleWidget::localize()
			{
				// Qt translate (not use currently)
				// setWindowTitle( QCoreApplication::translate( "ConsoleWidget", "Console", nullptr ) );
				setWindowTitle( "Console" );
			};

		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
