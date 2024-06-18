#include "widget/miscellaneous/orient_on_first_molecule_loaded.hpp"
#include "ui/action/animation.hpp"
#include <app/application/scene.hpp>
#include <app/application/system/action_manager.hpp>
#include <app/application/system/ecs_system.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/scene/aabb_component.hpp>
#include <app/component/scene/scene_item_component.hpp>
#include <app/vtx_app.hpp>

namespace VTX::UI::QT::Widget::Miscellaneous
{
	OrientOnFirstMoleculeLoadedTool::OrientOnFirstMoleculeLoadedTool()
	{
		App::SCENE().onSceneItemAdded += [ this ]( const App::Component::Scene::SceneItemComponent & p_item
										 ) { // Added item not a molecule => return
			if ( not App::MAIN_REGISTRY().hasComponent<App::Component::Chemistry::Molecule>( p_item ) )
			{
				return;
			}

			const size_t moleculeCount
				= App::SCENE().getAllSceneItemsOfType<App::Component::Chemistry::Molecule>().size();

			// Added molecule is not the only one => return
			if ( moleculeCount != 1 )
			{
				return;
			}

			const App::Component::Scene::AABB & moleculeAABB
				= App::MAIN_REGISTRY().getComponent<App::Component::Scene::AABB>( p_item );
			App::VTX_ACTION().execute<Action::Animation::Orient>( moleculeAABB.getWorldAABB() );
		};
	}
} // namespace VTX::UI::QT::Widget::Miscellaneous
