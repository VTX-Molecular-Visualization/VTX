#include "scene_widget.hpp"
#include "base_widget.hpp"
#include "vtx_app.hpp"
#include "object3d/scene.hpp"
#include "model/molecule.hpp"
#include <QListWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			SceneWidget::SceneWidget( QWidget * p_parent ) : BaseWidget( p_parent )
			{
				_registerEvent( Event::Global::ON_SCENE_CHANGE );
			}

			void SceneWidget::receiveEvent( const Event::VTXEvent & p_event )
			{
				if ( p_event.name == Event::Global::ON_SCENE_CHANGE )
				{
					// TODO
				}
			}

		} // namespace Widget
	}// namespace UI
} // namespace VTX


