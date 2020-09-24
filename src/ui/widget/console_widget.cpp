#include "console_widget.hpp"
#include <QListWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			ConsoleWidget::ConsoleWidget( QWidget * p_parent ) : QDockWidget( p_parent ), _ui( new Ui_ConsoleWidget() )
			{
				Event::BaseEventReceiverVTX::_registerEvents();
				_ui->setupUi( this );
				setWidget( _ui->listWidget );
			}

			ConsoleWidget::~ConsoleWidget()
			{
				Event::BaseEventReceiverVTX::_unregisterEvents();
				delete ( _ui );
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