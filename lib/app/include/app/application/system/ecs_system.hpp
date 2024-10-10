#ifndef __VTX_APP_APPLICATION_SYSTEM_ECS_SYSTEM__
#define __VTX_APP_APPLICATION_SYSTEM_ECS_SYSTEM__

#include "app/application/ecs/registry_manager.hpp"
#include <util/singleton.hpp>

namespace VTX::App::Application::System
{
	class ECSSystem
	{
	  public:
		Application::ECS::RegistryManager registryManager;
	};
} // namespace VTX::App::Application::System

namespace VTX::App
{
	inline Application::System::ECSSystem & ECS() { return Util::Singleton<Application::System::ECSSystem>::get(); }

	// MAIN_REGISTRY give access to the registry to create / get / delete entities and components
	inline Application::ECS::RegistryManager & MAIN_REGISTRY() { return ECS().registryManager; }
} // namespace VTX::App

#endif
