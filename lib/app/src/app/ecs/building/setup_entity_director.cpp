#include "app/ecs/building/setup_entity_director.hpp"
#include "app/ecs/entity/application/scene_entity.hpp"
#include "app/ecs/entity/scene/molecule_entity.hpp"

namespace VTX::App::ECS::Building
{
	void setupEntityDirector()
	{
		EntityDirector::addBuildStep( SCENE_ENTITY_ID,
									  EntityBuildStep( &Entity::Application::SceneEntityBuilder::addComponent,
													   &Entity::Application::SceneEntityBuilder::setup ) );

		EntityDirector::addBuildStep( MOLECULE_ENTITY_ID,
									  EntityBuildStep( &Entity::Scene::MoleculeEntityBuilder::addComponent,
													   &Entity::Scene::MoleculeEntityBuilder::setup ) );
	}

} // namespace VTX::App::ECS::Building
