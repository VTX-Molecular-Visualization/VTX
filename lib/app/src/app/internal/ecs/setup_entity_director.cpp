#include "app/internal/ecs/setup_entity_director.hpp"
#include "app/application/system/ecs_system.hpp"
#include "app/entity/all_entities.hpp"
#include "app/entity/application/scene_entity.hpp"
#include "app/entity/scene/camera_entity.hpp"
#include "app/entity/scene/molecule_entity.hpp"
#include "app/entity/scene/viewpoint_entity.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Internal::ECS
{

	void setupEntityDirector()
	{
		using namespace App::Application::ECS;
		using namespace App::Application::ECS::Building;

		EntityDirector & entityDirector = ENTITY_DIRECTOR();

		// Scene entity
		entityDirector.addBuildStep(
			Entity::SCENE_ENTITY_ID, PASS_ENUM::ADD_COMPONENT, &Entity::Application::SceneEntityBuilder::addComponent
		);

		// Camera entity
		entityDirector.addBuildStep(
			Entity::CAMERA_ENTITY_ID, PASS_ENUM::ADD_COMPONENT, &Entity::Scene::CameraEntityBuilder::addComponent
		);
		entityDirector.addBuildStep(
			Entity::CAMERA_ENTITY_ID, PASS_ENUM::SETUP, &Entity::Scene::CameraEntityBuilder::setup
		);

		// Molecule entity
		entityDirector.addBuildStep(
			Entity::MOLECULE_ENTITY_ID, PASS_ENUM::ADD_COMPONENT, &Entity::Scene::MoleculeEntityBuilder::addComponent
		);
		entityDirector.addBuildStep(
			Entity::MOLECULE_ENTITY_ID, PASS_ENUM::SETUP, &Entity::Scene::MoleculeEntityBuilder::setup
		);
		entityDirector.addBuildStep(
			Entity::MOLECULE_ENTITY_ID, PASS_ENUM::POST_SETUP, &Entity::Scene::MoleculeEntityBuilder::postSetup
		);

		// Viewpoint entity
		entityDirector.addBuildStep(
			Entity::VIEWPOINT_ENTITY_ID, PASS_ENUM::ADD_COMPONENT, &Entity::Scene::ViewpointEntityBuilder::addComponent
		);
		entityDirector.addBuildStep(
			Entity::VIEWPOINT_ENTITY_ID, PASS_ENUM::SETUP, &Entity::Scene::ViewpointEntityBuilder::setup
		);
	}

} // namespace VTX::App::Internal::ECS
