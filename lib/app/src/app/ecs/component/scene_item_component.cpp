#include "app/ecs/component/scene_item_component.hpp"
#include "app/ecs/component/updatable.hpp"

namespace VTX::App::ECS::Component
{
	void SceneItemComponent::addUpdateFunction( const std::string &					 p_key,
												const SceneUtility::UpdateCallback & p_callback )
	{
		// ECS::Core::BaseEntity entity = ECS::Manager::ECSManager::get().getEntity( *this );

		// if ( !ECS::Manager::ECSManager::get().hasComponent<Component::Updatable>( entity ) )
		//{
		//	ECS::Manager::ECSManager::get().addComponent<Component::Updatable>( entity );
		// }

		// Component::Updatable & updatable = ECS::Manager::ECSManager::get().getComponent<Component::Updatable>( entity
		// );

		// updatable.addCallback( p_key, p_callback );
	}
	void SceneItemComponent::removeUpdateFunction( const std::string & p_key )
	{
		// ECS::Core::BaseEntity entity = ECS::Manager::ECSManager::get().getEntity( *this );

		// assert( ECS::Manager::ECSManager::get().hasComponent<Component::Updatable>( entity ) );

		// Component::Updatable & updatable = ECS::Manager::ECSManager::get().getComponent<Component::Updatable>( entity
		// ); updatable.removeCallback( p_key );

		// if ( updatable.isEmpty() )
		//{
		//	ECS::Manager::ECSManager::get().removeComponent<Component::Updatable>( entity );
		// }
	}

} // namespace VTX::App::ECS::Component
