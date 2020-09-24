#include "console_widget.hpp"
#include <QListWidget>
#include <iostream>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			ConsoleWidget::ConsoleWidget( QWidget * p_parent ) : BaseWidget( p_parent )
			{
				_registerEvent( Event::Global::LOG_CONSOLE );
				setWidget( _ui->listWidget );
			}

			void ConsoleWidget::receiveEvent( const Event::VTXEvent & p_event )
			{
				if ( p_event.name == Event::Global::LOG_CONSOLE )
				{
					const Event::VTXEventLog & event = dynamic_cast<const Event::VTXEventLog &>( p_event );
					QListWidget * const		   list	 = _ui->listWidget;
					list->addItem(
						QString( ( "[" + event.date + "] " + "[" + event.level + "] " + event.message ).c_str() ) );
					list->scrollToBottom();
				}
			}

		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
