#ifndef __VTX_APP_APPLICATION_ECS_REGISTRY_MANAGER__
#define __VTX_APP_APPLICATION_ECS_REGISTRY_MANAGER__

#include "app/core/ecs/registry.hpp"

namespace VTX::App::Application::ECS
{
	class RegistryManager
	{
	  public:
		RegistryManager() {};
		Core::ECS::Registry & getRegistry() { return _registry; }

	  private:
		Core::ECS::Registry _registry = Core::ECS::Registry();
	};
} // namespace VTX::App::Application::ECS
#endif
