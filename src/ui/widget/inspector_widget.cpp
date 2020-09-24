#include "inspector_widget.hpp"
#include <QListWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			InspectorWidget::InspectorWidget( QWidget * p_parent ) : QDockWidget( p_parent ), _ui( new Ui_InspectorWidget() )
			{
				Event::BaseEventReceiverVTX::_registerEvents();
				_ui->setupUi( this );
			}

			InspectorWidget::~InspectorWidget()
			{
				Event::BaseEventReceiverVTX::_unregisterEvents();
				delete ( _ui );
			}

			void InspectorWidget::receiveEvent( const Event::VTXEvent & p_event )
			{
				if ( p_event.name == Event::Global::ON_SCENE_CHANGE )
				{
					//TODO
				}
			}

		} // namespace Widget
	}// namespace UI
} // namespace VTX
