#include "console.hpp"
#include <QListWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			Console::Console( QWidget * p_parent ) : QDockWidget( this ), _ui( new Ui_Console() )
			{
				Event::BaseEventReceiverVTX::_registerEvents();
				_ui->setupUi( this );
				//_ui->bottomDockWidget->setWidget( _ui->listWidget );
			}

			Console::~Console()
			{
				Event::BaseEventReceiverVTX::_unregisterEvents();
				delete ( _ui );
			}

			void Console::receiveEvent( const Event::VTXEvent & p_event )
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
