#include "scene_widget.hpp"
#include "base_widget.hpp"
#include "model/molecule.hpp"
#include "object3d/scene.hpp"
#include "vtx_app.hpp"
#include <QListWidget>
#include <QTreeWidget>

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
					const Event::VTXEventPtr<Model::Molecule> & event = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
					verticalLayout->addWidget( (QTreeWidget *)( event.ptr->getItem( ID::View::UI_MOLECULE_STRUCTURE ) ) );
				}
				else if ( p_event.name == Event::Global::MOLECULE_REMOVED )
				{
				}
			}

		} // namespace Widget
	}	  // namespace UI

} // namespace VTX
