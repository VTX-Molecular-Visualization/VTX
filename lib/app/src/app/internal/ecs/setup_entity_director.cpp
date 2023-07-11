#include "app/internal/ecs/setup_entity_director.hpp"
#include "app/entity/all_entities.hpp"
#include "app/entity/application/scene_entity.hpp"
#include "app/entity/scene/camera_entity.hpp"
#include "app/entity/scene/flat_molecule_entity.hpp"
#include "app/entity/scene/molecule_entity.hpp"

namespace VTX::App::Internal::ECS
{
	void setupEntityDirector()
	{
		using namespace App::Application::ECS;
		using namespace App::Application::ECS::Building;

		// Scene entity
		EntityDirector::addBuildStep(
			Entity::SCENE_ENTITY_ID, PASS_ENUM::ADD_COMPONENT, &Entity::Application::SceneEntityBuilder::addComponent );

		// Camera entity
		EntityDirector::addBuildStep(
			Entity::CAMERA_ENTITY_ID, PASS_ENUM::ADD_COMPONENT, &Entity::Scene::CameraEntityBuilder::addComponent );

		// Molecule entity
		EntityDirector::addBuildStep(
			Entity::MOLECULE_ENTITY_ID, PASS_ENUM::ADD_COMPONENT, &Entity::Scene::MoleculeEntityBuilder::addComponent );
		EntityDirector::addBuildStep(
			Entity::MOLECULE_ENTITY_ID, PASS_ENUM::SETUP, &Entity::Scene::MoleculeEntityBuilder::setup );
		EntityDirector::addBuildStep(
			Entity::MOLECULE_ENTITY_ID, PASS_ENUM::POST_SETUP, &Entity::Scene::MoleculeEntityBuilder::postSetup );

		// Flat Molecule entity
		EntityDirector::addBuildStep( Entity::FLAT_MOLECULE_ENTITY_ID,
									  PASS_ENUM::ADD_COMPONENT,
									  &Entity::Scene::FlatMoleculeEntityBuilder::addComponent );
		EntityDirector::addBuildStep(
			Entity::FLAT_MOLECULE_ENTITY_ID, PASS_ENUM::SETUP, &Entity::Scene::FlatMoleculeEntityBuilder::setup );
		EntityDirector::addBuildStep( Entity::FLAT_MOLECULE_ENTITY_ID,
									  PASS_ENUM::POST_SETUP,
									  &Entity::Scene::FlatMoleculeEntityBuilder::postSetup );
	}

} // namespace VTX::App::Internal::ECS
