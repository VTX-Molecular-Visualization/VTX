#include "scene_widget.hpp"
#include "base_widget.hpp"
#include "model/molecule.hpp"
#include "object3d/scene.hpp"
#include "vtx_app.hpp"
#include <QListWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			SceneWidget::SceneWidget( QWidget * p_parent ) : BaseWidget( p_parent )
			{
				_registerEvent( Event::Global::MOLECULE_ADDED );
				_registerEvent( Event::Global::MOLECULE_REMOVED );
			}

			void SceneWidget::receiveEvent( const Event::VTXEvent & p_event )
			{
				if ( p_event.name == Event::Global::MOLECULE_ADDED )
				{
					// TODO: add molecule view as child widget.
				}
				else if ( p_event.name == Event::Global::MOLECULE_REMOVED )
				{
				}
			}

		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
