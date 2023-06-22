#include "app/internal/ecs/setup_entity_director.hpp"
#include "app/entity/all_entities.hpp"
#include "app/entity/application/scene_entity.hpp"
#include "app/entity/scene/molecule_entity.hpp"

namespace VTX::App::Internal::ECS
{
	void setupEntityDirector()
	{
		using EntityDirector  = Application::ECS::EntityDirector;
		using EntityBuildStep = Application::ECS::Building::EntityBuildStep;

		EntityDirector::addBuildStep( Entity::SCENE_ENTITY_ID,
									  EntityBuildStep( &Entity::Application::SceneEntityBuilder::addComponent,
													   &Entity::Application::SceneEntityBuilder::setup ) );

		EntityDirector::addBuildStep( Entity::MOLECULE_ENTITY_ID,
									  EntityBuildStep( &Entity::Scene::MoleculeEntityBuilder::addComponent,
													   &Entity::Scene::MoleculeEntityBuilder::setup ) );
	}

} // namespace VTX::App::Internal::ECS
