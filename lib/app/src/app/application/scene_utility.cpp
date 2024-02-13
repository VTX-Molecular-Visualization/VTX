#include "app/application/scene_utility.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/scene.hpp"
#include "app/component/scene/uid_component.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Application::SceneUtility
{
	Core::ECS::BaseEntity findItemByUID( const Core::UID::uid & p_uid )
	{
		return SCENE().findItem(
			[ p_uid ]( const Core::ECS::BaseEntity & p_entity )
			{
				if ( !MAIN_REGISTRY().hasComponent<Component::Scene::UIDComponent>( p_entity ) )
					return false;

				const Component::Scene::UIDComponent & uidComponent
					= MAIN_REGISTRY().getComponent<Component::Scene::UIDComponent>( p_entity );

				return uidComponent.contains( p_uid );
			}
		);
	}
} // namespace VTX::App::Application::SceneUtility