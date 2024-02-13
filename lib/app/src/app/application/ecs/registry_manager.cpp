#include "app/application/ecs/registry_manager.hpp"
#include "app/application/ecs/entity_info.hpp"
#include "app/core/ecs/registry.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Application::ECS
{
	Core::ECS::BaseEntity RegistryManager::createEntity()
	{
		BaseEntity createdEntity = _registry.createEntity();
		_registry.addComponent<EntityInfoComponent>( createdEntity );

		return createdEntity;
	}
} // namespace VTX::App::Application::ECS

namespace VTX::App
{
	Application::ECS::RegistryManager & MAIN_REGISTRY() { return Application::ECS::RegistryManager::system.get(); }
} // namespace VTX::App
