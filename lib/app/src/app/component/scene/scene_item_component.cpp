#include "app/component/scene/scene_item_component.hpp"
#include "app/component/scene/updatable.hpp"
#include "app/core/ecs/base_entity.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Component::Scene
{
	void SceneItemComponent::addUpdateFunction(
		const std::string &								  p_key,
		const Application::SceneUtility::UpdateCallback & p_callback
	)
	{
		App::Core::ECS::BaseEntity entity = ECS_REGISTRY().getEntity( *this );

		if ( !ECS_REGISTRY().hasComponent<Component::Scene::Updatable>( entity ) )
		{
			ECS_REGISTRY().addComponent<Component::Scene::Updatable>( entity );
		}

		Component::Scene::Updatable & updatable = ECS_REGISTRY().getComponent<Component::Scene::Updatable>( entity );

		updatable.addCallback( p_key, p_callback );
	}
	void SceneItemComponent::removeUpdateFunction( const std::string & p_key )
	{
		Core::ECS::BaseEntity entity = ECS_REGISTRY().getEntity( *this );

		assert( ECS_REGISTRY().hasComponent<Component::Scene::Updatable>( entity ) );

		Component::Scene::Updatable & updatable = ECS_REGISTRY().getComponent<Component::Scene::Updatable>( entity );
		updatable.removeCallback( p_key );

		if ( updatable.isEmpty() )
		{
			ECS_REGISTRY().removeComponent<Component::Scene::Updatable>( entity );
		}
	}

} // namespace VTX::App::Component::Scene
