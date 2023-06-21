#include "app/internal/ecs/setup_entity_director.hpp"
#include "app/entity/application/scene_entity.hpp"
#include "app/entity/scene/molecule_entity.hpp"

namespace VTX::App::Internal::ECS
{
	void setupEntityDirector()
	{
		Application::ECS::Building::EntityDirector::addBuildStep(
			SCENE_ENTITY_ID,
			Application::ECS::Building::EntityBuildStep( &Entity::Application::SceneEntityBuilder::addComponent,
														 &Entity::Application::SceneEntityBuilder::setup ) );

		Application::ECS::Building::EntityDirector::addBuildStep(
			MOLECULE_ENTITY_ID,
			Application::ECS::Building::EntityBuildStep( &Entity::Scene::MoleculeEntityBuilder::addComponent,
														 &Entity::Scene::MoleculeEntityBuilder::setup ) );
	}

} // namespace VTX::App::Internal::ECS
