#include "app/application/ecs/registry_manager.hpp"
#include "app/core/ecs/registry.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Application::ECS
{
	Core::ECS::BaseEntity RegistryManager::createEntity()
	{
		BaseEntity createdEntity = _registry.createEntity();
		_registry.addComponent<Component::ECS::EntityInfoComponent>( createdEntity );

		return createdEntity;
	}
} // namespace VTX::App::Application::ECS
