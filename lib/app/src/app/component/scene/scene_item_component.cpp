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
		App::Core::ECS::BaseEntity entity = MAIN_REGISTRY().getEntity( *this );

		if ( !MAIN_REGISTRY().hasComponent<Component::Scene::Updatable>( entity ) )
		{
			MAIN_REGISTRY().addComponent<Component::Scene::Updatable>( entity );
		}

		Component::Scene::Updatable & updatable = MAIN_REGISTRY().getComponent<Component::Scene::Updatable>( entity );

		updatable.addCallback( p_key, p_callback );
	}
	void SceneItemComponent::removeUpdateFunction( const std::string & p_key )
	{
		Core::ECS::BaseEntity entity = MAIN_REGISTRY().getEntity( *this );

		assert( MAIN_REGISTRY().hasComponent<Component::Scene::Updatable>( entity ) );

		Component::Scene::Updatable & updatable = MAIN_REGISTRY().getComponent<Component::Scene::Updatable>( entity );
		updatable.removeCallback( p_key );

		if ( updatable.isEmpty() )
		{
			MAIN_REGISTRY().removeComponent<Component::Scene::Updatable>( entity );
		}
	}

} // namespace VTX::App::Component::Scene
