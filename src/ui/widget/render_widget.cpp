#include "render_widget.hpp"


namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			RenderWidget::RenderWidget( QWidget * p_parent ) : BaseWidget( p_parent )
			{
				_registerEvent( Event::Global::ON_SCENE_CHANGE );
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
