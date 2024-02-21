#include "app/application/system/ecs_system.hpp"

namespace VTX::App
{
	Application::ECS::RegistryManager & MAIN_REGISTRY()
	{
		return Application::System::ECSSystem::SYSTEM.get().registryManager;
	}
	Application::ECS::ComponentMetaFunction & COMPONENT_META_FUNCTION()
	{
		return Application::System::ECSSystem::SYSTEM.get().componentMetaFunction;
	}
	Application::ECS::EntityDirector & ENTITY_DIRECTOR()
	{
		return Application::System::ECSSystem::SYSTEM.get().entityDirector;
	}
} // namespace VTX::App
