#include "render_widget.hpp"
#include <QListWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			RenderWidget::RenderWidget( QWidget * p_parent ) : QDockWidget( p_parent ), _ui( new Ui_RenderWidget() )
			{
				Event::BaseEventReceiverVTX::_registerEvents();
				_ui->setupUi( this );
			}

			RenderWidget::~RenderWidget()
			{
				Event::BaseEventReceiverVTX::_unregisterEvents();
				delete ( _ui );
			}

			void RenderWidget::receiveEvent( const Event::VTXEvent & p_event )
			{
				if ( p_event.name == Event::Global::ON_SCENE_CHANGE )
				{
					//TODO
				}
			}

		} // namespace Widget
	}// namespace UI
} // namespace VTX
